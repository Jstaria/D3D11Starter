#include "Renderer.h"

using namespace DirectX;

namespace Renderer {

	// Private namespace for various methods and stored variables
	namespace {
		std::vector<std::shared_ptr<IRenderable>> sortedRenderables;
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

		void SetDefaultVar(IRenderable* obj, ExternalData data) {
			
			Material* mat = obj->GetMaterial().get();
			mat->GetPS()->SetFloat("iTime", GlobalVar::Time::getElapsedTime());
			mat->GetPS()->SetFloat2("iResolution", DirectX::XMFLOAT2((float)Window::Width(), (float)Window::Height()));
			mat->GetPS()->SetFloat3("iEyePosition", currentCamera->GetTransform()->GetPosition());
			mat->GetPS()->SetFloat3("iEyeDirection", currentCamera->GetTransform()->GetForward());
			mat->GetPS()->SetFloat3("iPosition", obj->GetTransform()->GetPosition());
			mat->GetVS()->SetFloat3("iPosition", obj->GetTransform()->GetPosition());
			mat->GetVS()->SetFloat("iTime", GlobalVar::Time::getElapsedTime());

			mat->GetVS()->SetData("data", &data, sizeof(ExternalData));
			mat->GetPS()->SetFloat3("iTint", { 1,1,1 });
		}

		void BindDataToDraw(IRenderable* gameObj) {
			
			XMFLOAT4X4 viewMatrix = currentCamera->GetView();
			
			Material* mat = gameObj->GetMaterial().get();

			switch (gameObj->GetRenderType()) {
			case IRenderable::RenderType::ParticleSys:
				

				break;

			case IRenderable::RenderType::Object:

				XMFLOAT4X4 worldmatrix = gameObj->GetTransform()->GetWorldMatrix();
				XMFLOAT4X4 projMatrix = currentCamera->GetProjection();;

				ExternalData data{};
				data.worldMatrix = worldmatrix;
				data.viewMatrix = viewMatrix;
				data.projMatrix = projMatrix;

				mat->GetVS()->SetShader();
				mat->GetPS()->SetShader();

				SetDefaultVar(gameObj, data);

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

void Renderer::SetCurrentCamera(std::shared_ptr<Camera> camera)
{
	currentCamera = camera;
}

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
