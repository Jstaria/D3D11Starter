#pragma once

#include "../Interfaces/IDrawable.h"
#include "../Structures/BillBoardVertex.h"
#include "../MainComponents/Graphics.h"
#include "../MainComponents/Renderer.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>

#include <vector>

class BillBoard : public IDrawable
{
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> layout;

	void CreateBillboard();

public:
	BillBoard(const char* name, Microsoft::WRL::ComPtr<ID3D11InputLayout> layout);

	void Draw() override;
	const char* GetName() override;
};

