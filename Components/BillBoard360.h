#pragma once
#include "BillBoard.h"
#include "../MainComponents/Renderer.h"
#include "../Components/GameObject.h"
#include "../Helper/GlobalVar.h"

#include <DirectXMath.h>

class BillBoard360 :
    public GameObject
{
private:
    int numOfPhases;
    int perRow;
    int perCol;

public:
    BillBoard360(const char* name, std::shared_ptr<IDrawable> drawable, 
        std::shared_ptr<GameObject> parentObj, 
        std::shared_ptr<Material> material, 
        int numOfPhases, int perRow, int perCol);

    void Draw() override;
    void Update(float deltaTime) override;
    void SetLook(DirectX::XMFLOAT2 direction);
};

