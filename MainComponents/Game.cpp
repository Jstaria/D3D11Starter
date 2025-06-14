#include "Game.h"
#include "Graphics.h"
#include "Input.h"
#include "Window.h"
#include <DirectXMath.h>

#include "../Structures/Vertex.h"
#include "../Helper/PathHelpers.h"
#include "../Helper/Debug.h"
#include "../Components/Transform.h"

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui/imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	//Settings::InitializeSettings();
	Debug::Initialize();
	CreateObjects();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Initialize ImGui itself & platform/renderer backends
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(Window::Handle());
		ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
		// Pick a style (uncomment one of these 3)
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();
		//ImGui::StyleColorsClassic();

		isImGuiDemoOpen = false;

		for (int i = 0; i < frameValueCount; i++)
		{
			frameValues.push_back(0);
			deltaValues.push_back(0);
		}

		Debug::ShowMesh = true;
		Debug::ShowWireFrame = false;
		//transform = XMFLOAT3(0, 0, 0);
		//tint = XMFLOAT4(1, 1, 1, 1);
	}

	Settings::UsingVsync = true;
	Graphics::SetVsyncState(Settings::UsingVsync);

	camerasSize = 3;
	cameras = new shared_ptr<Camera>[camerasSize];

	cameras[0] = make_shared<FPSCamera>("FPSCamera", XMFLOAT3(0, 0, -5.0f), 5.0f, 0.002f, 80, Window::AspectRatio(), 0.01f, 1000.0f);
	cameras[1] = make_shared<Camera>("StationaryCamera01", XMFLOAT3(5, 0, -5.0f), 90, Window::AspectRatio(), 0.01f, 1000.0f);
	cameras[1]->GetTransform()->SetRotation(0, -45, 0, Angle::DEGREES);
	cameras[2] = make_shared<Camera>("StationaryCamera02", XMFLOAT3(-5, 0, -5.0f), 20, Window::AspectRatio(), 0.01f, 1000.0f);
	cameras[2]->GetTransform()->SetRotation(0, 45, 0, Angle::DEGREES);
	Renderer::SetCurrentCamera(cameras[0]);

	lights.push_back(make_shared<Light>("Sun", false, true, XMFLOAT3(1, 1, .8f), XMFLOAT3(0, -1, 0), 1.0f)); // 1
	lights.push_back(make_shared<Light>("Red", false, false, XMFLOAT3(0, -2.0f, 6.0f), XMFLOAT3(1, 0, 0), XMFLOAT3(0, .5, 1), 30, 3.0f, 1.4f, 1.41f)); // 2
	lights.push_back(make_shared<Light>("Green", false, false, XMFLOAT3(2, 0.0f, 8.0f), XMFLOAT3(0, 1, 0), XMFLOAT3(0, 0, -1), 30, 3.0f, 1.4f, 1.41f));  // 3
	lights.push_back(make_shared<Light>("Blue", false, false, XMFLOAT3(0, 2, -7), XMFLOAT3(0, 0, 1), 4.0f, 10)); // 4
	lights.push_back(make_shared<Light>("White1", false, false, XMFLOAT3(0, 2, -5), XMFLOAT3(1, 1, 1), 4.0f, 10)); // 5
	lights.push_back(make_shared<Light>("White2", false, false, XMFLOAT3(0, 2, 5), XMFLOAT3(1, 1, 1), 4.0f, 10)); // 5
	lights.push_back(make_shared<Light>("White3", false, false, XMFLOAT3(-5, 2, 0), XMFLOAT3(1, 1, 1), 4.0f, 10)); // 5
	lights.push_back(make_shared<Light>("White4", false, false, XMFLOAT3(5, 2, 0), XMFLOAT3(1, 1, 1), 4.0f, 10)); // 5
	lights.push_back(make_shared<Light>("red", true, true, XMFLOAT3(0, 2, 0), XMFLOAT3(1, 0, 0), 5.0f, 10)); // 5
	lights[8]->GetTransform()->SetParentTransform(gameObjs[0]->GetTransform());

	for (auto& light : lights)
	{
		light->SetDrawable(drawables[2]);
		light->SetMaterial(materials["light_mat"]);
		light->GetTransform()->SetScale(.25f);
	}

	Renderer::SetLights(lights);
	Renderer::Init();
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	delete[] cameras;
}


// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateObjects()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 magenta(1.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 yellow(1.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 cyan(0.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 white(1, 1, 1, 1);

	vector<D3D11_INPUT_ELEMENT_DESC> billboardLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UNITID", 0, DXGI_FORMAT_R32_UINT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	D3DReadFileToBlob(FixPath(L"BillBoardVS.cso").c_str(), shaderBlob.GetAddressOf());

	ComPtr<ID3D11InputLayout> bbLayout;

	Graphics::Device->CreateInputLayout(
		billboardLayout.data(),
		(unsigned int)billboardLayout.size(),
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		bbLayout.GetAddressOf());

	shared_ptr<SimpleVertexShader> vs = make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	shared_ptr<SimpleVertexShader> MorphVS = make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"MorphVS.cso").c_str());
	shared_ptr<SimplePixelShader> RainbowFlowPS = make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"RainbowNoisePS.cso").c_str());
	shared_ptr<SimplePixelShader> ps = make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());
	//shared_ptr<SimplePixelShader> RainbowPS = make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"RainbowPS.cso").c_str());
	//shared_ptr<SimplePixelShader> NormalPS = make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"NormalPS.cso").c_str());
	shared_ptr<SimplePixelShader> PBRTexturePS = make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"PBRTexturePS.cso").c_str());
	//shared_ptr<SimplePixelShader> TexturePS = make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"TexturePS.cso").c_str());
	//shared_ptr<SimplePixelShader> DecaledTexturePS = make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"DecaledTexturePS.cso").c_str());
	//shared_ptr<SimpleComputeShader> cs = make_shared<SimpleComputeShader>(Graphics::Device, Graphics::Context, FixPath(L"ComputeShader.cso").c_str());
	shared_ptr<SimpleVertexShader> skyVS = make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"SkyVS.cso").c_str());
	shared_ptr<SimplePixelShader> skyPS = make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"SkyPS.cso").c_str());
	shared_ptr<SimpleVertexShader> shadowVS = make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"ShadowVS.cso").c_str());
	shared_ptr<SimpleVertexShader> billboardVS = make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"BillBoardVS.cso").c_str(), bbLayout, false);

	string assetsPath = "../../Assets/Models/";

	drawables.push_back(make_shared<Mesh>("Plane", FixPath(assetsPath + "quad.obj").c_str()));

	drawables.push_back(make_shared<Mesh>("Cube", FixPath(assetsPath + "cube.obj").c_str()));
	drawables.push_back(make_shared<BillBoard>("Billboard", bbLayout));
	drawables.push_back(make_shared<Mesh>("Sphere", FixPath(assetsPath + "sphere.obj").c_str()));
	shared_ptr<Mesh> mazeMesh = make_shared<Mesh>("Maze", FixPath(assetsPath + "maze2.obj").c_str());
	drawables.push_back(mazeMesh);

	ComPtr<ID3D11SamplerState> baseSampler;

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::Device->CreateSamplerState(&samplerDesc, baseSampler.GetAddressOf());

	ComPtr<ID3D11SamplerState> pixelSampler;

	D3D11_SAMPLER_DESC pixelDesc = {};
	pixelDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	pixelDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	pixelDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pixelDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pixelDesc.MaxAnisotropy = 1;
	pixelDesc.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::Device->CreateSamplerState(&pixelDesc, pixelSampler.GetAddressOf());

	string path = "../../Assets/PBR/";
	ComPtr<ID3D11ShaderResourceView> cobblestoneTextureSRV = LoadHelper::LoadTexture(path + "cobblestone_albedo.png");
	ComPtr<ID3D11ShaderResourceView> cobblestoneNormalSRV = LoadHelper::LoadTexture(path + "cobblestone_normals.png");
	ComPtr<ID3D11ShaderResourceView> cobblestoneRoughnessSRV = LoadHelper::LoadTexture(path + "cobblestone_roughness.png");
	ComPtr<ID3D11ShaderResourceView> cobblestoneMetalnessSRV = LoadHelper::LoadTexture(path + "cobblestone_metal.png");
	ComPtr<ID3D11ShaderResourceView> roughAlbedoSRV = LoadHelper::LoadTexture(path + "rough_albedo.png");
	ComPtr<ID3D11ShaderResourceView> roughNormalSRV = LoadHelper::LoadTexture(path + "rough_normals.png");
	ComPtr<ID3D11ShaderResourceView> roughRoughnessSRV = LoadHelper::LoadTexture(path + "rough_roughness.png");
	ComPtr<ID3D11ShaderResourceView> roughMetalnessSRV = LoadHelper::LoadTexture(path + "rough_metal.png");
	ComPtr<ID3D11ShaderResourceView> lightAlbedoSRV = LoadHelper::LoadTexture("../../Assets/Images/light.png");
	ComPtr<ID3D11ShaderResourceView> emojiAlbedoSRV = LoadHelper::LoadTexture(path + "emoji_albedo.png");
	ComPtr<ID3D11ShaderResourceView> mazeAlbedoSRV = LoadHelper::LoadTexture("../../Assets/Images/maze.png");
	ComPtr<ID3D11ShaderResourceView> skullAlbedoSRV = LoadHelper::LoadTexture("../../Assets/Images/skull-sheet.png");
	ComPtr<ID3D11ShaderResourceView> skullNormalSRV = LoadHelper::LoadTexture("../../Assets/Images/skull-normals-sheet.png");
	ComPtr<ID3D11ShaderResourceView> skullRoughSRV = LoadHelper::LoadTexture("../../Assets/Images/skull-rough-sheet.png");
	ComPtr<ID3D11ShaderResourceView> blankSRV = LoadHelper::LoadTexture(path + "cobblestone_metal.png");
	ComPtr<ID3D11ShaderResourceView> roughSRV = LoadHelper::LoadTexture(path + "bronze_metal.png");
	ComPtr<ID3D11ShaderResourceView> testSRV = LoadHelper::LoadTexture("../../Assets/Images/3d.jpg");

	materials.emplace("light_mat", make_shared<Material>("light_mat", billboardVS, ps, white));
	materials["light_mat"]->AddTextureSRV("SurfaceColorTexture", lightAlbedoSRV);
	materials["light_mat"]->AddSampler("BasicSampler", baseSampler);

	materials.emplace("cobblestone_mat", make_shared<Material>("cobblestone_mat", vs, PBRTexturePS, white));
	materials["cobblestone_mat"]->AddTextureSRV("Albedo", cobblestoneTextureSRV);
	materials["cobblestone_mat"]->AddTextureSRV("NormalMap", cobblestoneNormalSRV);
	materials["cobblestone_mat"]->AddTextureSRV("RoughnessMap", cobblestoneRoughnessSRV);
	materials["cobblestone_mat"]->AddTextureSRV("MetalnessMap", cobblestoneMetalnessSRV);
	materials["cobblestone_mat"]->AddSampler("BasicSampler", baseSampler);

	materials.emplace("emoji_mat", make_shared<Material>("emoji_mat", billboardVS, PBRTexturePS, white));
	materials["emoji_mat"]->AddTextureSRV("Albedo", emojiAlbedoSRV);
	materials["emoji_mat"]->AddTextureSRV("NormalMap", blankSRV);
	materials["emoji_mat"]->AddTextureSRV("RoughnessMap", blankSRV);
	materials["emoji_mat"]->AddTextureSRV("MetalnessMap", blankSRV);
	materials["emoji_mat"]->AddSampler("BasicSampler", baseSampler);

	materials.emplace("maze_mat", make_shared<Material>("maze_mat", vs, PBRTexturePS, white));
	materials["maze_mat"]->AddTextureSRV("Albedo", roughAlbedoSRV);
	materials["maze_mat"]->AddTextureSRV("NormalMap", roughNormalSRV);
	materials["maze_mat"]->AddTextureSRV("RoughnessMap", roughRoughnessSRV);
	materials["maze_mat"]->AddTextureSRV("MetalnessMap", roughMetalnessSRV);
	materials["maze_mat"]->AddSampler("BasicSampler", baseSampler);

	materials.emplace("skull_mat", make_shared<Material>("skull_mat", billboardVS, PBRTexturePS, white));
	materials["skull_mat"]->AddTextureSRV("Albedo", skullAlbedoSRV);
	materials["skull_mat"]->AddTextureSRV("NormalMap", skullNormalSRV);
	materials["skull_mat"]->AddTextureSRV("RoughnessMap", skullRoughSRV);
	materials["skull_mat"]->AddTextureSRV("MetalnessMap", blankSRV);
	materials["skull_mat"]->AddSampler("BasicSampler", pixelSampler);

	materials.emplace("lamp_mat", make_shared<Material>("lamp_mat", vs, PBRTexturePS, white));
	materials["lamp_mat"]->AddTextureSRV("Albedo", emojiAlbedoSRV);
	materials["lamp_mat"]->AddTextureSRV("NormalMap", blankSRV);
	materials["lamp_mat"]->AddTextureSRV("RoughnessMap", roughSRV);
	materials["lamp_mat"]->AddTextureSRV("MetalnessMap", blankSRV);
	materials["lamp_mat"]->AddSampler("BasicSampler", baseSampler);

	materials.emplace("tint_mat", make_shared<Material>("tint_mat", vs, ps, white));
	materials["tint_mat"]->AddTextureSRV("SurfaceColorTexture", roughSRV);
	materials["tint_mat"]->AddSampler("BasicSampler", baseSampler);

	materials.emplace("nav_mat", make_shared<Material>("nav_mat", vs, ps, white));

	for (auto& material : materials) {
		material.second->SetIndex();
		materialKeys.push_back(material.first);
	}

	// Cube Maps
	path = "../../Assets/Images/Skyboxes/";

	// Pass in path to create srvs
	//ComPtr<ID3D11ShaderResourceView> cm_PinkCloudsSRV = LoadHelper::CreateCubemap(L"../../Assets/Images/Skyboxes/Clouds Pink/");
	//ComPtr<ID3D11ShaderResourceView> cm_PlanetSRV = LoadHelper::CreateCubemap(L"../../Assets/Images/Skyboxes/Planet/");
	ComPtr<ID3D11ShaderResourceView> cm_DarkSRV = LoadHelper::CreateCubemap(L"../../Assets/Images/Skyboxes/Planet/");

	// Add the (for now) one sky to a vector
	skies.push_back(make_shared<Sky>(baseSampler, cm_DarkSRV, skyVS, skyPS, drawables[1]));

	// Set that as my current sky to then bind data and draw after objects
	Renderer::SetCurrentSky(skies[0]);

	float scale = 1;

	gameObjs.push_back(make_shared<BillBoard360>
		("Billboard360", drawables[2], nullptr, materials["skull_mat"], 250, 250, 1));
	gameObjs[0]->GetTransform()->SetScale(2);


	//gameObjs[1]->GetTransform()->SetScale(2);

	PhysicsData data{};
	data.frictionCoeff = .5f;
	data.mass = 1.0f;
	data.maxSpeed = .2f;
	data.radius = 1.0f;
	data.transform = gameObjs[0]->GetTransform();
	data.useGravity = false;
	data.useFriction = false;

	Agent agent(0.5f, data);
	entity = make_shared<Entity>(gameObjs[0], agent);

	//gameObjs.push_back(make_shared<GameObject>("Floor", drawables[0], nullptr, materials["cobblestone_mat"]));
	//gameObjs[1]->GetTransform()->SetPosition(0, -3.0f, 0);
	//gameObjs[1]->GetTransform()->SetScale(40, 1, 40);
	//

	gameObjs.push_back(make_shared<GameObject>("Maze", drawables[4], nullptr, materials["maze_mat"]));
	gameObjs[1]->GetTransform()->SetPosition(0, -1, 0);
	gameObjs[1]->GetMaterial()->SetUVScale({ 20,20 });

	gameObjs.push_back(make_shared<GameObject>("Indicator", drawables[3], gameObjs[0], materials["tint_mat"]));
	gameObjs[2]->GetTransform()->SetScale(.01f, .01f, .1f);
	gameObjs[2]->GetTransform()->SetPosition({ 0,0, -1 });


	gameObjs.push_back(make_shared<GameObject>("Position", drawables[3], nullptr, materials["tint_mat"]));
	gameObjs[3]->GetTransform()->SetScale(.1f, .1f, .1f);
	gameObjs[3]->GetTransform()->SetPosition({ 0,0, -1 });

	navMesh = make_shared<NavMesh>(mazeMesh->GetMeshData(), 
		materials["nav_mat"], gameObjs[1]->GetTransform(), 30);
	//gameObjs[1]->GetTransform()->SetScale(.02f);

	Renderer::SetShadowVS(shadowVS);
	for (int i = 0; i < gameObjs.size(); i++)
	{
		Renderer::AddObjectToRender(gameObjs[i]);
	}
	Renderer::AddObjectToRender(navMesh);
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	shared_ptr<Camera> cam = Renderer::GetCamera();
	if (cam != nullptr) {
		cam->UpdateProjectionMatrix(Window::AspectRatio());
	}

	PostProcessManager::OnResize();
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	//gameObjs[4]->GetTransform()->Rotate(0, .1f, 0, DEGREES);

	const float fixedDeltaTime = 1.0f / 60.0f;

	acummulator += deltaTime;

	while (acummulator >= fixedDeltaTime) {
		FixedUpdate(deltaTime, totalTime);
		acummulator -= fixedDeltaTime;
	}

	XMFLOAT3 directionToCam;
	XMFLOAT3 camPos = Renderer::GetCamera()->GetTransform()->GetPosition();
	XMFLOAT3 objPos = gameObjs[0]->GetTransform()->GetPosition();
	XMStoreFloat3(&directionToCam, XMVector3Normalize(XMLoadFloat3(&camPos) - XMLoadFloat3(&objPos)) * deltaTime);


	entity->Seek(targetPosition);
	entity->Update(deltaTime);

	//cameras[0]->GetTransform()->Rotate({ 0,1,0 }, deltaTime * 50, DEGREES, gameObjs[0]->GetTransform()->GetPosition());
	//cameras[0]->GetTransform()->LookAt(gameObjs[0]->GetTransform()->GetPosition());

	//lights[7]->GetTransform()->Rotate({ 0,1,0 }, deltaTime * 50, DEGREES, gameObjs[0]->GetTransform()->GetPosition());

	//gameObjs[0]->GetTransform()->Rotate({ 0, 50.0f * deltaTime, 0 }, DEGREES);

	if ((int)round(totalTime * 100.0f) % 200 == 0) {

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> angleDist(0, DirectX::XM_2PI);
		std::uniform_real_distribution<float> radiusDist(5, 40);

		DirectX::XMFLOAT3 center{ 0, 0, 0 };

		float angle = angleDist(gen);
		float r = radiusDist(gen);

		DirectX::XMFLOAT3 pos{
			center.x + r * cosf(angle),
			center.y,
			center.z + r * sinf(angle)
		};
		targetPosition = pos;
		gameObjs[3]->GetTransform()->SetPosition(targetPosition);
	}

	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();

	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);

	// Build custom UI
	BuildUI(deltaTime);

	Renderer::GetCamera()->Update(deltaTime);
	GlobalVar::Time::setDeltaTime(deltaTime);
	GlobalVar::Time::setElapsedTime(totalTime);
}

void Game::FixedUpdate(float deltaTime, float totalTime)
{
	entity->FixedUpdate(deltaTime);

	for (auto& light : lights)
		light->FixedUpdate(deltaTime);
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{

	// Frame START
	// - These things should happen ONCE PER FRAMEs
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(), color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	Renderer::DrawRenderables();

	{
		ImGui::Render(); // Turns this frame’s UI into renderable triangles
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen
	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}

// --------------------------------------------------------
// Begin custom ImGui window
// --------------------------------------------------------
void Game::BuildUI(float deltaTime) {
	ImGui::Begin("Host Window");
	if (ImGui::CollapsingHeader("Host Details")) {
		// Window data
		{
			ImGui::Text("Window Client Size: %dx%d", Window::Width(), Window::Height());

		}

		// Checkbox for toggling demo window
		{
			ImGui::Checkbox("Toggle Demo", &isImGuiDemoOpen);

			if (isImGuiDemoOpen) {
				ImGui::ShowDemoWindow(&isImGuiDemoOpen);
			}

			ImGui::Checkbox("Toggle Vsync", &Settings::UsingVsync);

			Graphics::SetVsyncState(Settings::UsingVsync);
		}

		// Current frame data
		{
			float currentFPS = ImGui::GetIO().Framerate;

			char label[64];

			if (getResetTimer < 0) {
				maxFrameValue = 0;
			}

			if (getFrameTimer < 0) {

				if (currentFPS > maxFrameValue) {
					maxFrameValue = currentFPS;
					getResetTimer = 100;
				}

				frameValues.insert(frameValues.begin(), currentFPS);
				frameValues.pop_back();

				deltaValues.insert(deltaValues.begin(), deltaTime);
				deltaValues.pop_back();

				getFrameTimer = .0525f;
				getResetTimer--;

				curFPS = currentFPS;
				curDT = deltaTime * 1000;
			}

			std::snprintf(label, sizeof(label), "Current Framerate: %.1f\n", curFPS);
			ImGui::Text(label);
			ImGui::PlotHistogram("FPS", frameValues.data(), frameValueCount, 0, nullptr, 0.0f, maxFrameValue * 1.25f, ImVec2(200, 100));

			std::snprintf(label, sizeof(label), "Current DeltaTime: %.2f ms\n", curDT);
			ImGui::Text(label);
			ImGui::PlotHistogram("Delta", deltaValues.data(), frameValueCount, 0, nullptr, 0.0f, 0.02f, ImVec2(200, 100));

			getFrameTimer -= deltaTime;
		}

		// Color picker for background
		{
			ImGui::ColorEdit4(("Color Picker##" + to_string(0)).c_str(), color);
		}
	}
	// Toggle Debug UI
	{
		Debug::DrawImGui();
	}

	// Light Obj Data
	{
		if (ImGui::CollapsingHeader("All Current Lights")) {
			for (auto& light : lights) light->DrawImGui();
		}
	}

	// Game Object Data
	{
		if (ImGui::CollapsingHeader("All Current GameObject Details")) {
			for (int i = 0; i < gameObjs.size(); i++)
			{
				gameObjs[i].get()->DrawImGui(materials, materialKeys);
			}
		}
	}

	// Mesh Data and Toggles
	{
		if (ImGui::CollapsingHeader("All Current Mesh Details")) {
			for (int i = 0; i < drawables.size(); i++)
			{
				Mesh* drawable = dynamic_cast<Mesh*>(drawables[i].get());;

				if (drawable != nullptr && ImGui::TreeNode(drawable->GetName())) {
					ImGui::Text("Triangles: %d", drawable->GetIndexCount() / 3);
					ImGui::Text("Vertices: %d", drawable->GetVertexCount());
					ImGui::Text("Indices: %d", drawable->GetIndexCount());

					// Adding unique identifiers to each checkbox because ImGUI yelled at me
					drawable->DrawImGui(i);
					ImGui::TreePop();
				}
			}
		}
	}

	{
		if (ImGui::CollapsingHeader("Current Camera Details")) {
			shared_ptr<Camera> cam = Renderer::GetCamera();
			cam->UIDraw();

			const char** items = new const char* [camerasSize];

			for (int i = 0; i < camerasSize; i++)
			{
				items[i] = cameras[i]->GetName();
			}

			ImGui::Text("Camera Select: ");
			ImGui::SameLine();
			if (ImGui::Combo("##MapCombo", &currentCam, items, camerasSize)) {
				Renderer::SetCurrentCamera(cameras[currentCam]);
			}

			delete[] items;
		}
	}

	entity->DrawImGui();

	Renderer::DrawImGui();

	ImGui::End();
}