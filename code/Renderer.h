#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "Raytracer.h"

#include <windows.h>
#include <memory>
#include <iostream>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>
#include <chrono>
#include <algorithm>

struct Vertex
{
	glm::vec4 pos;
	glm::vec2 mUV;
};

class Renderer
{
public:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	IDXGISwapChain* mSwapChain;

	Renderer(HWND window, int width, int height);
	virtual ~Renderer();
	void Update();
	void InitShaders();
	void Initialize(HWND window);
	void Render();
	void Clean();
	void PushObject(std::shared_ptr<kyun::Object> object);
	void SetLight(const glm::vec3 pos);
private:
	int mWidth, mHeight;
	kyun::Raytracer mRaytracer;
	std::vector<glm::vec4> mPixels;


	D3D11_VIEWPORT mViewport;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11InputLayout* mLayout;

	ID3D11Buffer* mVertexBuffer = nullptr;
	ID3D11Buffer* mIndexBuffer = nullptr;
	ID3D11Texture2D* mCanvasTexture = nullptr;
	ID3D11ShaderResourceView* mCanvasTextureView = nullptr;
	ID3D11RenderTargetView* mCanvasRenderTargetView = nullptr;
	ID3D11SamplerState* mColorSampler;
	UINT mIndexCount;
};