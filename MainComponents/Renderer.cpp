#include "Renderer.h"

using namespace DirectX;

namespace Renderer {

	// Private namespace for various methods and stored variables
	namespace {
		std::vector<std::shared_ptr<IRenderable>> sortedRenderables;
		Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
		std::shared_ptr<Camera> currentCamera;
		std::vector<std::shared_ptr<Light>> lights;

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

			switch (gameObj->GetRenderType()) {
			case IRenderable::RenderType::ParticleSys:
				

				break;

			case IRenderable::RenderType::Object:

				XMFLOAT4X4 worldmatrix = gameObj->GetTransform()->GetWorldMatrix();
				XMFLOAT4X4 projMatrix = currentCamera->GetProjection();
				XMFLOAT4X4 invWorldMatrix = gameObj->GetTransform()->GetWorldInverseTransposeMatrix();

				ExternalData data{};
				data.worldMatrix = worldmatrix;
				data.viewMatrix = viewMatrix;
				data.projMatrix = projMatrix;
				data.invWorldMatrix = invWorldMatrix;

				mat->GetVS()->SetShader();
				mat->GetPS()->SetShader();

				LightStruct lightStructs[MAX_LIGHTS];

				for (int i = 0; i < min(lights.size(), MAX_LIGHTS); i++)
				{
					lightStructs[i] = lights[0]->GetStruct();
				}

				mat->GetPS()->SetData("lights", &lightStructs[0], sizeof(LightStruct) * MAX_LIGHTS);

				mat->SetDefaultShaderParam(data, gameObj->GetTransform().get(), currentCamera->GetTransform().get());

				mat->GetVS()->CopyAllBufferData();
				mat->GetPS()->CopyAllBufferData();

				break;
			}

			
		}
		#pragma endregion

	}
}

void Renderer::Init()
{

}

void Renderer::AddObjectToRender(std::shared_ptr<IRenderable> gameObj)
{
	sortedRenderables.push_back(gameObj);

	SortObjectsViaDistance();
}

void Renderer::SetCurrentCamera(std::shared_ptr<Camera> camera) { currentCamera = camera; }
void Renderer::SetLights(std::vector<std::shared_ptr<Light>> lightList) { lights = lightList; }

void Renderer::DrawRenderables()
{
	for (int i = 0; i < sortedRenderables.size(); i++)
	{
		IRenderable* gameObj = sortedRenderables[i].get();

		BindDataToDraw(gameObj);
		gameObj->Draw();
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
