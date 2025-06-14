#include "Renderer.h"

using namespace DirectX;

namespace Renderer {

	// Private namespace for various methods and stored variables
	namespace {
		std::vector<std::shared_ptr<IRenderable>> sortedRenderables;
		Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
		std::shared_ptr<Camera> currentCamera;
		std::shared_ptr<Sky> currentSky;
		std::vector<std::shared_ptr<Light>> lights;

		std::shared_ptr<SimpleVertexShader> vsShadow;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
		DirectX::XMFLOAT4X4 lightViewMatrix;
		DirectX::XMFLOAT4X4 lightProjectionMatrix;

		// Sorting method(s)
#pragma region Sort
		void SortObjectsViaDistance() {
			// Yet to be implemented, isn't necessary yet, just adding it for the future
		}
#pragma endregion

		// Culling method(s)
#pragma region Culling

		bool Cull(Transform* transform) {
			// List of culling methods that ultimately return whether the obj should be culled
		}

#pragma endregion

		// Initialization: 
		//   > Buffers
#pragma region Buffers

		void BindDataToDraw(IRenderable* gameObj) {

			XMFLOAT4X4 viewMatrix = currentCamera->GetView();

			Material* mat = gameObj->GetMaterial().get();

			//Graphics::Context->PSSetShaderResources(0, 1, texture.GetAddressOf());

			XMFLOAT4X4 worldmatrix = gameObj->GetTransform()->GetWorldMatrix();
			XMFLOAT4X4 projMatrix = currentCamera->GetProjection();
			XMFLOAT4X4 invWorldMatrix = gameObj->GetTransform()->GetWorldInverseTransposeMatrix();

			ExternalData data{};
			data.worldMatrix = worldmatrix;
			data.viewMatrix = viewMatrix;
			data.projMatrix = projMatrix;
			data.invWorldMatrix = invWorldMatrix;

			// ==============================================
			// I should find a way to make this auto do this via IRenderable honestly
			// An Idea: Make SetDefaultShaderParam() an IRenderable function and then overwrite it in Light
			Light* light = dynamic_cast<Light*>(gameObj);
			if (light != nullptr)
				light->UpdateLightColor();

			mat->GetVS()->SetShader();
			mat->GetPS()->SetShader();

			if (light == nullptr)
			{
				int offset = 0;

				LightStruct lightStructs[MAX_LIGHTS];
				for (int i = 0; i < min(lights.size(), MAX_LIGHTS); i++)
				{
					if (lights[i]->GetActive())
					{
						lightStructs[i - offset] = lights[i]->GetStruct();
					}
					else offset++;
				}

				mat->GetPS()->SetData("lights", &lightStructs[0], sizeof(LightStruct) * MAX_LIGHTS);
			}

			mat->GetVS()->SetMatrix4x4("lightView", lightViewMatrix);
			mat->GetVS()->SetMatrix4x4("lightProjection", lightProjectionMatrix);
			mat->GetPS()->SetShaderResourceView("ShadowMap", shadowSRV.Get());
			mat->GetPS()->SetSamplerState("ShadowSampler", shadowSampler.Get());
			mat->SetDefaultShaderParam(data, gameObj->GetTransform().get(), currentCamera->GetTransform().get());

			mat->GetVS()->CopyAllBufferData();
			mat->GetPS()->CopyAllBufferData();
		}

		void BindAndDrawSkyData() {

			XMFLOAT4X4 viewMatrix = currentCamera->GetView();
			XMFLOAT4X4 projMatrix = currentCamera->GetProjection();

			ExternalData data{};
			data.viewMatrix = viewMatrix;
			data.projMatrix = projMatrix;

			currentSky->Draw(data);
		}

		void BindAndDrawShadowMap() {
			XMFLOAT3 lightDir = lights[0]->GetStruct().Direction;
			XMVECTOR lightDirection = XMLoadFloat3(&lightDir);

			XMMATRIX lightView = XMMatrixLookToLH(
				-lightDirection * 20, // Position: "Backing up" 20 units from origin
				lightDirection, // Direction: light's direction
				XMVectorSet(0, 1, 0, 0)); // Up: World up vector (Y axis)
			XMStoreFloat4x4(&lightViewMatrix, lightView);

			float lightProjectionSize = Debug::ShadowMapSize; // Tweak for your scene!
			XMMATRIX lightProjection = XMMatrixOrthographicLH(
				lightProjectionSize, lightProjectionSize, 1.0f, 100.0f);
			XMStoreFloat4x4(&lightProjectionMatrix, lightProjection);

			Graphics::Context->ClearDepthStencilView(shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
			ID3D11RenderTargetView* nullRTV{};
			Graphics::Context->OMSetRenderTargets(1, &nullRTV, shadowDSV.Get());
			Graphics::Context->PSSetShader(0, 0, 0);

			D3D11_VIEWPORT viewport = {};
			viewport.Width = (float)Debug::ShadowMapResolution;
			viewport.Height = (float)Debug::ShadowMapResolution;
			viewport.MaxDepth = 1.0f;
			Graphics::Context->RSSetViewports(1, &viewport);

			vsShadow->SetShader();
			vsShadow->SetMatrix4x4("view", lightViewMatrix);
			vsShadow->SetMatrix4x4("projection", lightProjectionMatrix);
			vsShadow->SetSamplerState("ShadowSampler", shadowSampler.Get());

			Graphics::Context->RSSetState(shadowRasterizer.Get());

			// Loop and draw all entities
			for (auto& e : sortedRenderables)
			{
				Light* light = dynamic_cast<Light*>(e.get());
				if (light != nullptr)
					continue;

				vsShadow->SetMatrix4x4("world", e->GetTransform()->GetWorldMatrix());
				vsShadow->CopyAllBufferData();
				// Draw the mesh directly to avoid the entity's material
				// Note: Your code may differ significantly here!
				e->GetDrawable()->Draw();
			}

			viewport.Width = (float)Window::Width();
			viewport.Height = (float)Window::Height();
			Graphics::Context->RSSetViewports(1, &viewport);
			Graphics::Context->OMSetRenderTargets(
				1,
				Graphics::BackBufferRTV.GetAddressOf(),
				Graphics::DepthBufferDSV.Get());

			Graphics::Context->RSSetState(0);
		}
#pragma endregion

	}
}

/// <summary>
/// Initializes renderer specific variables, must be called after setting renderer information
/// </summary>
void Renderer::Init()
{
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = Debug::ShadowMapResolution; // Ideally a power of 2 (like 1024)
	shadowDesc.Height = Debug::ShadowMapResolution; // Ideally a power of 2 (like 1024)
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;
	Graphics::Device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());


	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	Graphics::Device->CreateDepthStencilView(
		shadowTexture.Get(),
		&shadowDSDesc,
		shadowDSV.GetAddressOf());
	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	Graphics::Device->CreateShaderResourceView(
		shadowTexture.Get(),
		&srvDesc,
		shadowSRV.GetAddressOf());

	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Min. precision units, not world units!
	shadowRastDesc.SlopeScaledDepthBias = 1.0f; // Bias more based on slope
	Graphics::Device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	Graphics::Device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	PostProcessManager::Init();
}

void Renderer::AddObjectToRender(std::shared_ptr<IRenderable> gameObj)
{
	sortedRenderables.push_back(gameObj);

	SortObjectsViaDistance();
}

void Renderer::SetCurrentCamera(std::shared_ptr<Camera> camera) { currentCamera = camera; }
void Renderer::SetCurrentSky(std::shared_ptr<Sky> sky) { currentSky = sky; }
void Renderer::SetLights(std::vector<std::shared_ptr<Light>> lightList) {
	for (auto& light : lightList) sortedRenderables.push_back(light);
	lights = lightList;
}
void Renderer::SetShadowVS(std::shared_ptr<SimpleVertexShader> shadowVS) { vsShadow = shadowVS; }

void Renderer::DrawRenderables()
{
	BindAndDrawShadowMap();

	PostProcessManager::SetInitialTarget();

	for (int i = 0; i < sortedRenderables.size(); i++)
	{
		IRenderable* gameObj = sortedRenderables[i].get();

		// Get the drawable object
		std::shared_ptr<IDrawable> drawable = gameObj->GetDrawable();

		// Check if it's a mesh (using proper C++ RTTI)
		Mesh* mesh = dynamic_cast<Mesh*>(drawable.get());
		if (mesh) {
			// Determine which rasterizer state to use
			if (mesh->GetToggleWireFrame() && Debug::ShowWireFrame) {
				Graphics::Context->RSSetState(Debug::RasterizerWFState.Get());
			}
			else if (mesh->GetToggleMesh() && Debug::ShowMesh) {
				Graphics::Context->RSSetState(Debug::RasterizerFillState.Get());
			}
		}

		BindDataToDraw(gameObj);
		gameObj->Draw();
	}

	BindAndDrawSkyData();

	PostProcessManager::DrawFinal();

	ID3D11ShaderResourceView* nullSRVs[128] = {};
	Graphics::Context->PSSetShaderResources(0, 128, nullSRVs);
}

void Renderer::DrawImGui()
{
	if (ImGui::CollapsingHeader("Renderer Details")) {
		PostProcessManager::DrawImGui();
	}
}

void Renderer::UpdateRenderableList()
{
	// Right now would only call for objects to be sorted
	SortObjectsViaDistance();
}

std::shared_ptr<Camera> Renderer::GetCamera()
{
	return currentCamera;
}
