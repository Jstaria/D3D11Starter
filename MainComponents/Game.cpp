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
	Debug::Initialize();
	CreateGeometry();

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

	Renderer::Init();

	isInVsync = true;
	Graphics::SetVsyncState(isInVsync);

	camerasSize = 3;
	cameras = new shared_ptr<Camera>[camerasSize];

	cameras[0] = make_shared<FPSCamera>(
		"FPSCamera",
		XMFLOAT3(0, 0, -5.0f),
		5.0f,
		0.002f,
		80,
		Window::AspectRatio(),
		0.01f,
		1000.0f);
	cameras[1] = make_shared<Camera>(
		"StationaryCamera01",
		XMFLOAT3(5, 0, -5.0f),
		90,
		Window::AspectRatio(),
		0.01f,
		1000.0f);
	cameras[1]->GetTransform()->SetRotation(0, -45, 0, Angle::DEGREES);
	cameras[2] = make_shared<Camera>(
		"StationaryCamera02",
		XMFLOAT3(-5, 0, -5.0f),
		20,
		Window::AspectRatio(),
		0.01f,
		1000.0f);
	cameras[2]->GetTransform()->SetRotation(0, 45, 0,Angle::DEGREES);
	Renderer::SetCurrentCamera(cameras[0]);
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

	delete[] meshes;
	delete[] gameObjs;
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
void Game::CreateGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 magenta = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 yellow = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 cyan = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);

	shared_ptr<SimpleVertexShader> vs = make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	shared_ptr<SimpleVertexShader> MorphVS = make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"MorphVS.cso").c_str());
	shared_ptr<SimplePixelShader> RainbowFlowPS = make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"RainbowNoisePS.cso").c_str());
	shared_ptr<SimplePixelShader> RainbowPS = make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"RainbowPS.cso").c_str());
	shared_ptr<SimplePixelShader> NormalPS = make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"NormalPS.cso").c_str());
	shared_ptr<SimpleComputeShader> cs = make_shared<SimpleComputeShader>(Graphics::Device, Graphics::Context, FixPath(L"ComputeShader.cso").c_str());

	meshesSize = 3;
	meshes = new shared_ptr<Mesh>[meshesSize];
	meshes[0] = make_shared<Mesh>("Cube", FixPath("../../Assets/Models/quad.obj").c_str());
	meshes[1] = make_shared<Mesh>("Sphere", FixPath("../../Assets/Models/sphere.obj").c_str());
	meshes[2] = make_shared<Mesh>("Helix", FixPath("../../Assets/Models/helix.obj").c_str());

	gameObjsSize = 3;

	gameObjs = new shared_ptr<GameObject>[gameObjsSize];

	shared_ptr<Material> rainbowFlow_mat = make_shared<Material>(vs, RainbowFlowPS, magenta);
	shared_ptr<Material> rainbow_mat = make_shared<Material>(vs, RainbowPS, yellow);
	shared_ptr<Material> normal_mat = make_shared<Material>(MorphVS, NormalPS, cyan);

	float scale = 1;

	gameObjs[0] = make_shared<GameObject>("Plane", meshes[0], nullptr, rainbowFlow_mat);
	gameObjs[0]->GetTransform()->SetRotation(-90, 0, 0, Angle::DEGREES);
	gameObjs[0]->GetTransform()->SetScale(scale);
	gameObjs[1] = make_shared<GameObject>("Sphere", meshes[1], nullptr, rainbow_mat);
	gameObjs[1]->GetTransform()->SetPosition(-3, 0.0f, 0);
	//gameObjs[1]->GetTransform()->SetRotation(0, 0, 180, Angle::DEGREES);
	gameObjs[1]->GetTransform()->SetScale(scale);
	gameObjs[2] = make_shared<GameObject>("Helix", meshes[2], nullptr, normal_mat);
	gameObjs[2]->GetTransform()->SetPosition(3.0f, 0.0f, 0);
	gameObjs[2]->GetTransform()->SetRotation(0, 135, 0, Angle::DEGREES);
	gameObjs[2]->GetTransform()->SetScale(scale);
	//gameObjs[3] = make_shared<GameObject>("BottomRight", meshes[1], gameObjs[0], mat);
	//gameObjs[3]->GetTransform()->SetPosition(0.5f, -0.5f, 0);
	//gameObjs[3]->GetTransform()->SetScale(scale);
	//gameObjs[4] = make_shared<GameObject>("BottomLeft", meshes[2], nullptr, mat);
	//gameObjs[4]->GetTransform()->SetPosition(-1.0f, -1.0f, 0);
	//gameObjs[4]->GetTransform()->SetRotation(0, 0, -90, Angle::DEGREES);
	//gameObjs[4]->GetTransform()->SetScale(scale);

	for (int i = 0; i < gameObjsSize; i++)
	{
		Renderer::AddObjectToRender(gameObjs[i]);
	}
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
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
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

	//float primaryFrequency = 1.5f; 
	//float secondaryFrequency = 1.0f; 
	//float primaryAmplitude = 0.75f;
	//float secondaryAmplitude = 0.5f;

	//float beat = primaryAmplitude * sin(2 * 3.1415f * primaryFrequency * totalTime);
	//beat += secondaryAmplitude * sin(2 * 3.1415f * secondaryFrequency * totalTime);

	//float noise = 0.1f * ((float)rand() / RAND_MAX - 0.5f);
	//beat += noise;
	//beat = (beat + 1.0f) * 0.15f + 0.5f;

	//int speed = 10;

	//{
	//	Transform* transform = gameObjs[0].get()->GetTransform().get();
	//	XMFLOAT3 pyr = transform->GetPitchYawRoll();
	//	XMFLOAT3 scale = transform->GetScale();
	//	transform->SetRotation(pyr.x, pyr.y, totalTime + beat / 2, Angle::PI);
	//	transform->SetScale(beat, beat, scale.z);
	//}

	//{
	//	Transform* triTr1 = gameObjs[1].get()->GetTransform().get();
	//	Transform* triTr2 = gameObjs[3].get()->GetTransform().get();

	//	XMFLOAT3 scale = triTr1->GetScale();
	//	float speed = 10;
	//	triTr1->SetScale((cos(totalTime * speed) + 2) / 2 * beat, (sin(totalTime * speed) + 2) / 2* beat, scale.z);
	//	triTr2->SetScale((cos(totalTime * speed) + 2) / 2 * beat, (sin(totalTime * speed) + 2) / 2* beat, scale.z);
	//}


	// Build custom UI
	BuildUI(deltaTime);

	Renderer::GetCamera()->Update(deltaTime);
	GlobalVar::Time::setDeltaTime(deltaTime);
	GlobalVar::Time::setElapsedTime(totalTime);
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{

	// Frame START
	// - These things should happen ONCE PER FRAME
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

			ImGui::Checkbox("Toggle Vsync", &isInVsync);

			Graphics::SetVsyncState(isInVsync);
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
		if (ImGui::CollapsingHeader("Debug Information")) {
			ImGui::SetWindowFontScale(0.9f);
			ImGui::Text("(Note: Meshes override this debug toggle)");
			ImGui::SetWindowFontScale(1.0f);
			ImGui::Checkbox("Show Wireframes", &Debug::ShowWireFrame);
			ImGui::Checkbox("Show Meshes", &Debug::ShowMesh);

			if (ImGui::Button("Change Window Size: 720x720")) {
				Window::AdjustWindowSize(720, 720);
			}
		}
	}

	// Game Object Data
	{
		if (ImGui::CollapsingHeader("All Current GameObject Details")) {
			for (int i = 0; i < gameObjsSize; i++)
			{
				gameObjs[i].get()->DrawImGui();
			}
		}
	}

	// Mesh Data and Toggles
	{
		if (ImGui::CollapsingHeader("All Current Mesh Details")) {
			for (int i = 0; i < meshesSize; i++)
			{
				if (ImGui::TreeNode(meshes[i]->GetName())) {
					ImGui::Text("Triangles: %d", meshes[i]->GetIndexCount() / 3);
					ImGui::Text("Vertices: %d", meshes[i]->GetVertexCount());
					ImGui::Text("Indices: %d", meshes[i]->GetIndexCount());

					// Adding unique identifiers to each checkbox because ImGUI yelled at me
					ImGui::Checkbox(("Show Wireframe##" + to_string(i)).c_str(), meshes[i]->GetToggleWireFrame());
					ImGui::Checkbox(("Show Mesh##" + to_string(i)).c_str(), meshes[i]->GetToggleMesh());
					ImGui::TreePop();
				}
			}
		}
	}

	{
		if (ImGui::CollapsingHeader("Current Camera Details")) {
			shared_ptr<Camera> cam = Renderer::GetCamera();
			cam->UIDraw();

			if (ImGui::TreeNode("Camera Select")) {
				const char** items = new const char* [camerasSize];

				for (int i = 0; i < camerasSize; i++)
				{
					items[i] = cameras[i]->GetName();
				}

				ImGui::ListBox(" ", &currentCam, items, camerasSize);

				Renderer::SetCurrentCamera(cameras[currentCam]);

				delete[] items;

				ImGui::TreePop();
			}
		}
	}


	ImGui::End();
}
