#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <windows.h>
#include <memory>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include "Renderer.h"

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 콘솔창이 있으면 디버깅에 유리합니다.
int main()
{
	const int width = 3840, height = 2160;
	// const int width =1280, height = 720;

	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0L,
		0L,
		GetModuleHandle(NULL),
		NULL,
		NULL,
		NULL,
		NULL,
		L"Rendering Example", // lpszClassName, L-string
		NULL };

	RegisterClassEx(&wc);

	// 실제로 그려지는 해상도를 설정하기 위해
	RECT wr = { 0, 0, width, height };				   // set the size, but not the position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); // adjust the size

	HWND hwnd = CreateWindow(
		wc.lpszClassName,
		L"Rendering Example",
		WS_OVERLAPPEDWINDOW,
		100,				// 윈도우 좌측 상단의 x 좌표
		100,				// 윈도우 좌측 상단의 y 좌표
		wr.right - wr.left, // 윈도우 가로 방향 해상도
		wr.bottom - wr.top, // 윈도우 세로 방향 해상도
		NULL,
		NULL,
		wc.hInstance,
		NULL);

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	auto example = std::make_unique<Renderer>(hwnd, width, height);
	
	// Example* example = new Example(....);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.DisplaySize = ImVec2(width, height);
	ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplDX11_Init(example->mDevice, example->mDeviceContext);
	ImGui_ImplWin32_Init(hwnd);

	// Part 3. Make Object
	using namespace kyun;
	const float cubeCenterX = 0.0f;
	const float cubeCenterY = 0.0f;
	const float cubeCenterZ = 2.0f;
	const float cubeSize = 20.0f;

	auto groundPosXTexture = std::make_shared<Texture>("Image/posx.jpg");
	auto groundPosYTexture = std::make_shared<Texture>("Image/posy.jpg");
	auto groundPosZTexture = std::make_shared<Texture>("Image/posz.jpg");
	auto groundNegXTexture = std::make_shared<Texture>("Image/negx.jpg");
	auto groundNegYTexture = std::make_shared<Texture>("Image/negy.jpg");
	auto groundNegZTexture = std::make_shared<Texture>("Image/negz.jpg");
	auto tableTexture = std::make_shared<Texture>("Image/table_texture2.jpg");
	
	// back
	auto groundPosX = std::make_shared<Square>(vec3(cubeCenterX + cubeSize, cubeCenterY + cubeSize, cubeCenterZ - cubeSize), vec3(cubeCenterX - cubeSize, cubeCenterY + cubeSize, cubeCenterZ - cubeSize), vec3(cubeCenterX - cubeSize, cubeCenterY - cubeSize, cubeCenterZ - cubeSize), vec3(cubeCenterX + cubeSize, cubeCenterY - cubeSize, cubeCenterZ - cubeSize));
	groundPosX->SetAmbient(vec3(1.0f));
	groundPosX->SetDiffuse(vec3(1.0f));
	groundPosX->SetSpecular(vec3(0.0f));
	groundPosX->SetReflection(0.0f);
	groundPosX->SetTransparency(0.0f);
	groundPosX->SetAlpha(10.0f);
	groundPosX->SetAmbTexture(groundPosXTexture);
	groundPosX->SetDifTexture(groundPosXTexture);

	// top
	auto groundPosY = std::make_shared<Square>(vec3(cubeCenterX - cubeSize, cubeCenterY + cubeSize, cubeCenterZ + cubeSize), vec3(cubeCenterX - cubeSize, cubeCenterY + cubeSize, cubeCenterZ - cubeSize), vec3(cubeCenterX + cubeSize, cubeCenterY + cubeSize, cubeCenterZ - cubeSize), vec3(cubeCenterX + cubeSize, cubeCenterY + cubeSize, cubeCenterZ + cubeSize));
	groundPosY->SetAmbient(vec3(1.0f));
	groundPosY->SetDiffuse(vec3(0.0f));
	groundPosY->SetSpecular(vec3(0.0f));
	groundPosY->SetReflection(0.0f);
	groundPosY->SetTransparency(0.0f);
	groundPosY->SetAlpha(10.0f);
	groundPosY->SetAmbTexture(groundPosYTexture);
	groundPosY->SetDifTexture(groundPosYTexture);

	// right
	auto groundPosZ = std::make_shared<Square>(vec3(cubeCenterX + cubeSize, cubeCenterY + cubeSize, cubeCenterZ + cubeSize), vec3(cubeCenterX + cubeSize, cubeCenterY + cubeSize, cubeCenterZ - cubeSize), vec3(cubeCenterX + cubeSize, cubeCenterY - cubeSize, cubeCenterZ - cubeSize), vec3(cubeCenterX + cubeSize, cubeCenterY - cubeSize, cubeCenterZ + cubeSize));
	groundPosZ->SetAmbient(vec3(1.0f));
	groundPosZ->SetDiffuse(vec3(0.0f));
	groundPosZ->SetSpecular(vec3(0.0f));
	groundPosZ->SetReflection(0.0f);
	groundPosZ->SetTransparency(0.0f);
	groundPosZ->SetAlpha(10.0f);
	groundPosZ->SetAmbTexture(groundPosZTexture);
	groundPosZ->SetDifTexture(groundPosZTexture);

	// center
	auto groundNegX = std::make_shared<Square>(vec3(cubeCenterX - cubeSize, cubeCenterY + cubeSize, cubeCenterZ + cubeSize), vec3(cubeCenterX + cubeSize, cubeCenterY + cubeSize, cubeCenterZ + cubeSize), vec3(cubeCenterX + cubeSize, cubeCenterY - cubeSize, cubeCenterZ + cubeSize), vec3(cubeCenterX - cubeSize, cubeCenterY - cubeSize, cubeCenterZ + cubeSize));
	groundNegX->SetAmbient(vec3(1.0f));
	groundNegX->SetDiffuse(vec3(0.0f));
	groundNegX->SetSpecular(vec3(0.0f));
	groundNegX->SetReflection(0.0f);
	groundNegX->SetTransparency(0.0f);
	groundNegX->SetAlpha(10.0f);
	groundNegX->SetAmbTexture(groundNegXTexture);
	groundNegX->SetDifTexture(groundNegXTexture);

	// botton
	auto groundNegY = std::make_shared<Square>(vec3(cubeCenterX + cubeSize, cubeCenterY - cubeSize, cubeCenterZ + cubeSize), vec3(cubeCenterX + cubeSize, cubeCenterY - cubeSize, cubeCenterZ - cubeSize), vec3(cubeCenterX - cubeSize, cubeCenterY - cubeSize, cubeCenterZ - cubeSize), vec3(cubeCenterX - cubeSize, cubeCenterY - cubeSize, cubeCenterZ + cubeSize));
	groundNegY->SetAmbient(vec3(1.0f));
	groundNegY->SetDiffuse(vec3(0.0f));
	groundNegY->SetSpecular(vec3(0.0f));
	groundNegY->SetReflection(0.0f);
	groundNegY->SetTransparency(0.0f);
	groundNegY->SetAlpha(10.0f);
	groundNegY->SetAmbTexture(groundNegYTexture);
	groundNegY->SetDifTexture(groundNegYTexture);

	// left
	auto groundNegZ = std::make_shared<Square>(vec3(cubeCenterX - cubeSize, cubeCenterY + cubeSize, cubeCenterZ - cubeSize), vec3(cubeCenterX - cubeSize, cubeCenterY + cubeSize, cubeCenterZ + cubeSize), vec3(cubeCenterX - cubeSize, cubeCenterY - cubeSize, cubeCenterZ + cubeSize), vec3(cubeCenterX - cubeSize, cubeCenterY - cubeSize, cubeCenterZ - cubeSize));
	groundNegZ->SetAmbient(vec3(1.0f));
	groundNegZ->SetDiffuse(vec3(0.0f));
	groundNegZ->SetSpecular(vec3(0.0f));
	groundNegZ->SetReflection(0.0f);
	groundNegZ->SetTransparency(0.0f);
	groundNegZ->SetAlpha(10.0f);
	groundNegZ->SetAmbTexture(groundNegZTexture);
	groundNegZ->SetDifTexture(groundNegZTexture);

	
	auto table = std::make_shared<Square>(vec3(-6.0f, -4.0f, 10.0f), vec3(6.0f, -4.0f, 10.0f), vec3(6.0f, -4.0f, 1.0f), vec3(-6.0f, -4.0f, 1.0f));
	table->SetAmbient(vec3(1.0f));
	table->SetDiffuse(vec3(0.0f));
	table->SetSpecular(vec3(0.0f));
	table->SetReflection(0.0f);
	table->SetTransparency(0.0f);
	table->SetAlpha(10.0f);
	table->SetAmbTexture(tableTexture);
	table->SetDifTexture(tableTexture);

	auto sphere1 = std::make_shared<Sphere>(vec3(-3.0f, -1.5f, 6.0f), 1.5f);
	sphere1->SetAmbient(vec3(0.0f));
	sphere1->SetDiffuse(vec3(0.0f, 0.0f, 0.0f));
	sphere1->SetSpecular(vec3(0.0f));
	sphere1->SetAlpha(50.0f);
	sphere1->SetReflection(0.0f);
	sphere1->SetTransparency(1.0f);

	auto sphere2 = std::make_shared<Sphere>(vec3(3.0f, -2.0f, 4.0f), 2.0f);
	sphere2->SetAmbient(vec3(1.0f, 0.0f, 0.0f));
	sphere2->SetDiffuse(vec3(1.0f, 0.0f, 0.0f));
	sphere2->SetSpecular(vec3(1.0f, 0.0f, 0.0f));
	sphere2->SetAlpha(50.0f);
	sphere2->SetReflection(0.6f);
	sphere2->SetTransparency(0.0f);

	auto sphere3 = std::make_shared<Sphere>(vec3(-0.1f, 3.0f, 10.0f), 3.0f);
	sphere3->SetAmbient(vec3(0.0f, 0.0f, 1.0f));
	sphere3->SetDiffuse(vec3(0.0f, 0.0f, 1.0f));
	sphere3->SetSpecular(vec3(0.0f, 0.0f, 1.0f));
	sphere3->SetAlpha(50.0f);
	sphere3->SetReflection(0.6f);
	sphere3->SetTransparency(0.0f);
	
	example->SetLight(vec3(-1.5f, 8.0f, 8.0f));
	example->PushObject(groundPosX);
	example->PushObject(groundPosY);
	example->PushObject(groundPosZ);
	example->PushObject(groundNegX);
	example->PushObject(groundNegY);
	example->PushObject(groundNegZ);
	// example->PushObject(table);
	example->PushObject(sphere1);
	example->PushObject(sphere2);
	example->PushObject(sphere3);


	// Part 4. Main Loop
	// Main message loop
	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//// Start the Dear ImGui frame
			//ImGui_ImplDX11_NewFrame();
			//ImGui_ImplWin32_NewFrame();
			//ImGui::NewFrame();
			//ImGui::Begin("Scene Controller");
			//ImGui::End();
			//ImGui::Render();

			example->Update();
			example->Render();

			//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			// switch the back buffer and the front buffer
			example->mSwapChain->Present(1, 0);
		}
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	example->Clean();
	DestroyWindow(hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Windows procedure
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		// Reset and resize swapchain
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_MOUSEMOVE:
		// std::cout << "Mouse " << LOWORD(lParam) << " " << HIWORD(lParam) << std::endl;
		break;
	case WM_LBUTTONUP:
		// std::cout << "WM_LBUTTONUP Left mouse button" << std::endl;
		break;
	case WM_RBUTTONUP:
		// std::cout << "WM_RBUTTONUP Right mouse button" << std::endl;
		break;
	case WM_KEYDOWN:
		// std::cout << "WM_KEYDOWN " << (int)wParam << std::endl;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
