#include "Renderer.h"

using namespace DirectX;

namespace Renderer {

	// Private namespace for various methods and stored variables
	namespace {
		std::vector<std::shared_ptr<GameObject>> sortedGameObjs;
		Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
		std::shared_ptr<Camera> currentCamera;

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
		void SetupConstantBuffer() {
			// Create constant buffer 
			{
				unsigned int bufferSize = (sizeof(ExternalData) + 15) / 16 * 16;
				D3D11_BUFFER_DESC cbDesc{};
				cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
				cbDesc.ByteWidth = bufferSize,
					cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				cbDesc.MiscFlags = 0;
				cbDesc.StructureByteStride = 0;
				cbDesc.Usage = D3D11_USAGE_DYNAMIC;

				Graphics::Device->CreateBuffer(&cbDesc, 0, constantBuffer.GetAddressOf());

				// Bind buffer
				Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
			}
		}

		void BindDataToDraw(Transform* tr, DirectX::XMFLOAT4 tint) {
			// Get Data
			XMMATRIX aspectScaleMatrix = XMMatrixScaling((float)Window::Height() / (float)Window::Width(), 1, 1);
			
			XMFLOAT4X4 worldmatrix = tr->GetWorldMatrix();
			XMFLOAT4X4 projMatrix = currentCamera->GetProjection();; //XMStoreFloat4x4(&projMatrix,XMMatrixIdentity());//
			XMFLOAT4X4 viewMatrix = currentCamera->GetView(); //XMStoreFloat4x4(&viewMatrix,XMMatrixIdentity());//currentCamera->GetView();

			ExternalData data{};
			data.tint = tint;
			data.worldMatrix = worldmatrix;
			data.viewMatrix = viewMatrix;
			data.projMatrix = projMatrix;

			// Map the buffer
			D3D11_MAPPED_SUBRESOURCE mapped{};

			Graphics::Context->Map(
				constantBuffer.Get(),
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&mapped);

			// Copy to GPU
			memcpy(mapped.pData, &data, sizeof(ExternalData));

			// Unmap data
			Graphics::Context->Unmap(constantBuffer.Get(), 0);
		}
		#pragma endregion

	}
}

void Renderer::Init()
{
	SetupConstantBuffer();
}

void Renderer::AddObjectToRender(std::shared_ptr<GameObject> gameObj)
{
	sortedGameObjs.push_back(gameObj);

	SortObjectsViaDistance();
}

void Renderer::SetCurrentCamera(std::shared_ptr<Camera> camera)
{
	currentCamera = camera;
}

void Renderer::DrawObjects()
{
	for (int i = 0; i < sortedGameObjs.size(); i++)
	{
		GameObject* gameObj = sortedGameObjs[i].get();
		Transform* transform = gameObj->GetTransform().get();
		Mesh* mesh = gameObj->GetMesh().get();

		BindDataToDraw(transform, gameObj->GetTint());
		mesh->Draw();
	}
}

void Renderer::UpdateObjectList()
{
	// Right now would only call for objects to be sorted
	SortObjectsViaDistance();
}

std::shared_ptr<Camera> Renderer::GetCamera()
{
	return currentCamera;
}
