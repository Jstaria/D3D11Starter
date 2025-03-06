#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <memory>
#include <map>
#include <DirectXMath.h>
#include "Renderer.h"
#include "WICTextureLoader.h"

#include "../Components/Mesh.h"
#include "../Components/Camera.h"
#include "../Components/FPSCamera.h"
#include "../Components/Material.h"

#include "../Helper/SimpleShader.h"
#include "../Helper/GlobalVar.h"

#include "../Structures/BufType.h"

class Game
{
public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void BuildUI(float deltaTime);
	void OnResize();

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateGeometry();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	// UI Data
	bool isImGuiDemoOpen;
	bool isInVsync;
	bool inDebugMode;
	std::vector<float> frameValues;
	std::vector<float> deltaValues;
	int frameValueCount = 100;
	float maxFrameValue = 0;
	float getFrameTimer;
	int getResetTimer;
	float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };
	float curFPS = 0;
	float curDT = 0;

	// Arrays
	int meshesSize;
	int gameObjsSize;
	int camerasSize;

	std::shared_ptr<Mesh>* meshes;
	std::shared_ptr<GameObject>* gameObjs;
	std::shared_ptr<Camera>* cameras;
	std::map<const char*, std::shared_ptr<Material>> materials;
	std::vector<const char*> materialsKeys;
	
	int currentCam;
};

