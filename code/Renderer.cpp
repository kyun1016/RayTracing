#include "Renderer.h"

Renderer::Renderer(HWND window, int width, int height)
	: mWidth(width)
	, mHeight(height)
	, mRaytracer(width, height) // 컴퓨터가 느릴 경우 mRaytracer(width/4, height/4) 같이 해상도 조절 가능
{
	Initialize(window);
}

Renderer::~Renderer()
{
	Clean();
}

void Renderer::Update()
{
	static int count = 0;
	if (count == 0) // 한 번만 렌더링
	{
		mPixels.resize(mRaytracer.mWidth * mRaytracer.mHeight);

		mRaytracer.Render(mPixels);

		D3D11_MAPPED_SUBRESOURCE ms;
		mDeviceContext->Map(mCanvasTexture, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, mPixels.data(), mPixels.size() * sizeof(glm::vec4));
		mDeviceContext->Unmap(mCanvasTexture, NULL);
	}
	count++;
}

// https://docs.microsoft.com/en-us/windows/win32/direct3d11/how-to--compile-a-shader
void Renderer::InitShaders()
{
	ID3DBlob* vertexBlob = nullptr;
	ID3DBlob* pixelBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	if (FAILED(D3DCompileFromFile(L"VS.hlsl", 0, 0, "main", "vs_5_0", 0, 0, &vertexBlob, &errorBlob)))
	{
		if (errorBlob)
		{
			std::cout << "Vertex shader compile error\n"
				<< (char*)errorBlob->GetBufferPointer() << std::endl;
		}
	}

	if (FAILED(D3DCompileFromFile(L"PS.hlsl", 0, 0, "main", "ps_5_0", 0, 0, &pixelBlob, &errorBlob)))
	{
		if (errorBlob)
		{
			std::cout << "Pixel shader compile error\n"
				<< (char*)errorBlob->GetBufferPointer() << std::endl;
		}
	}

	mDevice->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), NULL, &mVertexShader);
	mDevice->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, &mPixelShader);

	// Create the input mLayout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	mDevice->CreateInputLayout(ied, 2, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &mLayout);
	mDeviceContext->IASetInputLayout(mLayout);
}

void Renderer::Initialize(HWND window)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferDesc.Width = mWidth;					  // set the back buffer width
	swapChainDesc.BufferDesc.Height = mHeight;					  // set the back buffer height
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
	swapChainDesc.BufferCount = 2;								  // one back buffer
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how swap chain is to be used
	swapChainDesc.OutputWindow = window;						  // the window to be used
	swapChainDesc.SampleDesc.Count = 1;							  // how many multisamples
	swapChainDesc.Windowed = TRUE;								  // windowed/full-screen mode
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	// createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

	const D3D_FEATURE_LEVEL featureLevelArray[1] = { D3D_FEATURE_LEVEL_11_0 };
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,
		featureLevelArray,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mSwapChain,
		&mDevice,
		NULL,
		&mDeviceContext)))
	{
		std::cout << "D3D11CreateDeviceAndSwapChain() error" << std::endl;
	}

	// CreateRenderTarget
	ID3D11Texture2D* pBackBuffer;
	mSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer)
	{
		mDevice->CreateRenderTargetView(pBackBuffer, NULL, &mRenderTargetView);
		pBackBuffer->Release();
	}
	else
	{
		std::cout << "CreateRenderTargetView() error" << std::endl;
		exit(-1);
	}

	// Set the mViewport
	ZeroMemory(&mViewport, sizeof(D3D11_VIEWPORT));
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = static_cast<float>(mWidth);
	mViewport.Height = static_cast<float>(mHeight);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f; // Note: important for depth buffering
	mDeviceContext->RSSetViewports(1, &mViewport);

	InitShaders();

	// Create texture and rendertarget
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // D3D11_FILTER_MIN_MAG_MIP_LINEAR
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the Sample State
	mDevice->CreateSamplerState(&sampDesc, &mColorSampler);

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.MipLevels = textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.MiscFlags = 0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureDesc.Width = mRaytracer.mWidth;
	textureDesc.Height = mRaytracer.mHeight;

	mDevice->CreateTexture2D(&textureDesc, nullptr, &mCanvasTexture);

	if (mCanvasTexture)
	{
		mDevice->CreateShaderResourceView(mCanvasTexture, nullptr, &mCanvasTextureView);

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		mDevice->CreateRenderTargetView(mCanvasTexture, &renderTargetViewDesc, &mCanvasRenderTargetView);
	}
	else
	{
		std::cout << "CreateRenderTargetView() error" << std::endl;
	}

	// Create a vertex buffer
	{
		const std::vector<Vertex> vertices =
		{
			{
				{ -1.0f, -1.0f, 0.0f, 1.0f },
			{ 0.f, 1.f },
			},
				{
					{ 1.0f, -1.0f, 0.0f, 1.0f },
			{ 1.f, 1.f },
				},
				{
					{ 1.0f, 1.0f, 0.0f, 1.0f },
			{ 1.f, 0.f },
				},
				{
					{ -1.0f, 1.0f, 0.0f, 1.0f },
			{ 0.f, 0.f },
				},
		};

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;						   // write access access by CPU and GPU
		bufferDesc.ByteWidth = UINT(sizeof(Vertex) * vertices.size()); // size is the VERTEX struct * 3
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			   // use as a vertex buffer
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;			   // allow CPU to write in buffer
		bufferDesc.StructureByteStride = sizeof(Vertex);

		D3D11_SUBRESOURCE_DATA mVertexBufferData = {
			0,
		};
		mVertexBufferData.pSysMem = vertices.data();
		mVertexBufferData.SysMemPitch = 0;
		mVertexBufferData.SysMemSlicePitch = 0;

		const HRESULT hr = mDevice->CreateBuffer(&bufferDesc, &mVertexBufferData, &mVertexBuffer);
		if (FAILED(hr))
		{
			std::cout << "CreateBuffer() failed. " << std::hex << hr << std::endl;
		};
	}

	// Create an index buffer
	{
		const std::vector<uint16_t> indices =
		{
			3,
			1,
			0,
			2,
			1,
			3,
		};

		mIndexCount = UINT(indices.size());

		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // write access access by CPU and GPU
		bufferDesc.ByteWidth = UINT(sizeof(uint16_t) * indices.size());
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;		// use as a vertex buffer
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // allow CPU to write in buffer
		bufferDesc.StructureByteStride = sizeof(uint16_t);

		D3D11_SUBRESOURCE_DATA mIndexBufferData = { 0 };
		mIndexBufferData.pSysMem = indices.data();
		mIndexBufferData.SysMemPitch = 0;
		mIndexBufferData.SysMemSlicePitch = 0;

		mDevice->CreateBuffer(&bufferDesc, &mIndexBufferData, &mIndexBuffer);
	}
}

void Renderer::Render()
{
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	mDeviceContext->RSSetViewports(1, &mViewport);
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, clearColor);

	// set the shader objects
	mDeviceContext->VSSetShader(mVertexShader, 0, 0);
	mDeviceContext->PSSetShader(mPixelShader, 0, 0);

	// select which vertex buffer to display
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	mDeviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// https://github.com/Microsoft/DirectXTK/wiki/Getting-Started
	// https://github.com/microsoft/Xbox-ATG-Samples/tree/main/PCSamples/IntroGraphics/SimpleTexturePC
	mDeviceContext->PSSetSamplers(0, 1, &mColorSampler); // TODO: samplers to array
	mDeviceContext->PSSetShaderResources(0, 1, &mCanvasTextureView);
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDeviceContext->DrawIndexed(mIndexCount, 0, 0);
}

void Renderer::Clean()
{
	if (mLayout)
	{
		mLayout->Release();
		mLayout = NULL;
	}
	if (mVertexShader)
	{
		mVertexShader->Release();
		mVertexShader = NULL;
	}
	if (mPixelShader)
	{
		mPixelShader->Release();
		mPixelShader = NULL;
	}
	if (mVertexBuffer)
	{
		mVertexBuffer->Release();
		mVertexBuffer = NULL;
	}
	if (mIndexBuffer)
	{
		mIndexBuffer->Release();
		mIndexBuffer = NULL;
	}
	if (mCanvasTexture)
	{
		mCanvasTexture->Release();
		mCanvasTexture = NULL;
	}
	if (mCanvasTextureView)
	{
		mCanvasTextureView->Release();
		mCanvasTextureView = NULL;
	}
	if (mCanvasRenderTargetView)
	{
		mCanvasRenderTargetView->Release();
		mCanvasRenderTargetView = NULL;
	}
	if (mColorSampler)
	{
		mColorSampler->Release();
		mColorSampler = NULL;
	}
	if (mRenderTargetView)
	{
		mRenderTargetView->Release();
		mRenderTargetView = NULL;
	}
	if (mSwapChain)
	{
		mSwapChain->Release();
		mSwapChain = NULL;
	}
	if (mDeviceContext)
	{
		mDeviceContext->Release();
		mDeviceContext = NULL;
	}
	if (mDevice)
	{
		mDevice->Release();
		mDevice = NULL;
	}
}

void Renderer::PushObject(std::shared_ptr<kyun::Object> object)
{
	mRaytracer.mObjects.push_back(object);
}

void Renderer::SetLight(const glm::vec3 pos)
{
	mRaytracer.mLight.mPos = pos;
}
