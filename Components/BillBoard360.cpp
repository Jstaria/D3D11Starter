#include "BillBoard360.h"

BillBoard360::BillBoard360(const char* name, std::shared_ptr<IDrawable> drawable, std::shared_ptr<GameObject> parentObj, std::shared_ptr<Material> material, int numOfPhases, int perRow, int perCol)
	: GameObject(name, drawable, parentObj, material), numOfPhases(numOfPhases), perRow(perRow), perCol(perCol)
{
	material->SetUVScale({ 1.0f / (float)perRow, 1.0f / (float)perCol});
}

void BillBoard360::Draw()
{
	// Swap angle img

	GameObject::Draw();
}

void BillBoard360::Update(float deltaTime)
{
	std::shared_ptr<Camera> currentCam = Renderer::GetCamera();

	DirectX::XMFLOAT3 tFor = transform->GetForward();
	DirectX::XMVECTOR transformForward = DirectX::XMLoadFloat3(&tFor);
	DirectX::XMFLOAT3 tRi = transform->GetRight();
	DirectX::XMVECTOR transformRight = DirectX::XMLoadFloat3(&tRi);

	DirectX::XMFLOAT3 cPos = currentCam->GetTransform()->GetPosition();
	DirectX::XMVECTOR camPosition = DirectX::XMLoadFloat3(&cPos);
	DirectX::XMFLOAT3 tPos = transform->GetPosition();
	DirectX::XMVECTOR transformPosition = DirectX::XMLoadFloat3(&tPos);

	DirectX::XMVECTOR toObjPos = DirectX::XMVectorSubtract(transformPosition, camPosition);
	toObjPos = DirectX::XMVector3Normalize(toObjPos);

	float forward = DirectX::XMVectorGetX(DirectX::XMVector3Dot(transformForward, toObjPos));
	float right = DirectX::XMVectorGetX(DirectX::XMVector3Dot(transformRight, toObjPos));

	float angle = atan2f(right, forward);
	angle = angle > 0 ? angle : (2 * DirectX::XM_PI + angle);
	angle = DirectX::XMConvertToDegrees(angle);

	float section = 360.0f / numOfPhases;
	int phase = static_cast<int>((angle + (section * 0.5f)) / section) % numOfPhases;

	float uvX = (phase % perRow) / static_cast<float>(perRow);
	float uvY = (phase / perRow) / static_cast<float>(perCol);

	material->SetUVOffset({ uvX, uvY });
}

void BillBoard360::SetLook(DirectX::XMFLOAT2 direction) 
{ 
	transform->SetRotation(direction.x, 0, direction.y, Angle::DEGREES);
}
