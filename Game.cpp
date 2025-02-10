#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include "Debug.h"

#include <DirectXMath.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include "Transform.h"

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

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);

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
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}


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

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in CPU memory
	//    over to a Direct3D-controlled data structure on the GPU (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	Vertex vertices[] =
	{
		{ XMFLOAT3(-0.5f, +0.5f, +0.0f), XMFLOAT4(0.0f,0.0f,0.0f,1.0f)},
		{ XMFLOAT3(+0.5f, +0.5f, +0.0f), magenta },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), yellow },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), cyan },
	};

	// Set up indices, which tell us which vertices to use and in which order
	// - This is redundant for just 3 vertices, but will be more useful later
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	unsigned int indices2[] = { 0, 1, 2 };

	meshesSize = 3;
	meshes = new shared_ptr<Mesh>[meshesSize];
	meshes[0] = make_shared<Mesh>("Square", vertices, indices, 4, 6);

	Vertex vertices2[] =
	{
		{ XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(+0.5f, +0.0f, +0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(+0.0f, -0.5f, +0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
	};

	Vertex vertices3[] =
	{
		{ XMFLOAT3(+0.0f, +0.0f, +0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, +0.0f, +0.0f), XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f) },
	};

	meshes[1] = make_shared<Mesh>("TriangleOne", vertices2, indices2, 3, 3);
	meshes[2] = make_shared<Mesh>("TriangleTwo", vertices3, indices2, 3, 3);

	gameObjsSize = 5;

	gameObjs = new shared_ptr<GameObject>[gameObjsSize];

	float Degree90 = 90.0f * (3.1415f / 180.0f);

	gameObjs[0] = make_shared<GameObject>("MiddleSquare", meshes[0], nullptr);
	gameObjs[0].get()->GetTransform().get()->SetRotation(0, 0, Degree90 / 2);

	gameObjs[1] = make_shared<GameObject>("TopLeft", meshes[1], gameObjs[0]);
	gameObjs[1].get()->GetTransform().get()->SetPosition(-0.5f, 0.5f, 0);
	gameObjs[1].get()->GetTransform().get()->SetRotation(0, 0, Degree90 * 2);
	gameObjs[2] = make_shared<GameObject>("TopRight", meshes[2], nullptr);
	gameObjs[2].get()->GetTransform().get()->SetPosition(1.0f, 1.0f, 0);
	gameObjs[2].get()->GetTransform().get()->SetRotation(0, 0, Degree90);
	gameObjs[3] = make_shared<GameObject>("BottomRight", meshes[1], gameObjs[0]);
	gameObjs[3].get()->GetTransform().get()->SetPosition(0.5f, -0.5f, 0);
	//gameObjs[3].get()->GetTransform().get()->SetRotation(0, 0, Degree90 * 2);
	gameObjs[4] = make_shared<GameObject>("BottomLeft", meshes[2], nullptr);
	gameObjs[4].get()->GetTransform().get()->SetPosition(-1.0f, -1.0f, 0);
	gameObjs[4].get()->GetTransform().get()->SetRotation(0, 0, -Degree90);

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
	for (int i = 0; i < gameObjsSize; i++)
	{
		gameObjs[i].get()->GetTransform().get()->SetDirty(true);
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

	float primaryFrequency = 4.0f; 
	float secondaryFrequency = 2.0f; 
	float primaryAmplitude = 0.75f;
	float secondaryAmplitude = 0.5f;

	float beat = primaryAmplitude * sin(2 * 3.1415f * primaryFrequency * totalTime);
	beat += secondaryAmplitude * sin(2 * 3.1415f * secondaryFrequency * totalTime);

	float noise = 0.1f * ((float)rand() / RAND_MAX - 0.5f);
	beat += noise;
	beat = (beat + 1.0f) * 0.25f + 0.5f;

	int speed = 10;

	Transform* transform = gameObjs[0].get()->GetTransform().get();
	XMFLOAT3 pyr = transform->GetPitchYawRoll();
	XMFLOAT3 scale = transform->GetScale();
	transform->SetRotation(pyr.x, pyr.y , totalTime + beat / 2);
	transform->SetScale(beat, beat, scale.z);
	

	// Build custom UI
	BuildUI(deltaTime);

	//printf("Mouse pos: {%d,%d}\n", Input::GetMouseX(), Input::GetMouseY());
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

	Renderer::DrawObjects();

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
		}

		// Current frame data
		{
			float currentFPS = ImGui::GetIO().Framerate;

			char label[64];

			if (getFrameTimer < 0) {
				maxFrameValue = currentFPS > maxFrameValue ? currentFPS : maxFrameValue;

				frameValues.insert(frameValues.begin(), currentFPS);
				frameValues.pop_back();

				deltaValues.insert(deltaValues.begin(), deltaTime);
				deltaValues.pop_back();

				getFrameTimer = .0525f;

				curFPS = currentFPS;
				curDT = deltaTime * 1000;
			}

			std::snprintf(label, sizeof(label), "Current Framerate: %.1f\n", curFPS);
			ImGui::Text(label);
			ImGui::PlotHistogram("FPS", frameValues.data(), frameValueCount, 0, nullptr, 0.0f, maxFrameValue, ImVec2(200, 100));

			std::snprintf(label, sizeof(label), "Current DeltaTime: %.2f ms\n", curDT);
			ImGui::Text(label);
			ImGui::PlotHistogram("Delta", deltaValues.data(), frameValueCount, 0, nullptr, 0.0f, 0.01f, ImVec2(200, 100));

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


	ImGui::End();
}
