#include "pch.h"
#include "Renderer.h"

namespace dae
{
	Renderer::Renderer(SDL_Window* pWindow)
		: m_pWindow(pWindow)
		, m_Camera{ Vector3::Zero, 45.0f }
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize Camera
		float aspectRatio = static_cast<float>(m_Width) / m_Height;
		m_Camera.Initialize(aspectRatio, 0.001f, 1000.0f, 45.0f, { 0.0f, 0.0f, -50.0f });
		m_Camera.walkSpeed = 30.0f;

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		//Create test mesh
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		Utils::ParseOBJ("Resources/vehicle.obj", vertices, indices);

		m_pTestMesh = std::make_unique<Mesh>(m_pDevice, vertices, indices);
		m_pTestMesh->SetDiffuseMap("Resources/vehicle_diffuse.png");
	}

	Renderer::~Renderer()
	{
		if (m_pRenderTargetView) m_pRenderTargetView->Release();
		if (m_pRenderTargetBuffer) m_pRenderTargetBuffer->Release();
		if (m_pDepthStencilView) m_pDepthStencilView->Release();
		if (m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();
		if (m_pSwapChain) m_pSwapChain->Release();

		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}

		if (m_pDevice) m_pDevice->Release();
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_Camera.Update(pTimer);
	}

	void Renderer::Render() const
	{
		if (!m_IsInitialized)
			return;

		constexpr float clearColor[4] = { 0.0f, 0.0f, 0.3f, 1.0f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		m_pTestMesh->Render(m_Camera, m_pDeviceContext);

		m_pSwapChain->Present(0, 0);
	}

	HRESULT Renderer::InitializeDirectX()
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;
#if defined (_DEBUG) || defined(DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &featureLevel,
										   1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

		if (FAILED(result))
		{
			return result;
		}

		IDXGIFactory1* pDxgiFactory;
		
		result = CreateDXGIFactory1(IID_PPV_ARGS(&pDxgiFactory));
		if (FAILED(result))
		{
			return result;
		}

		SDL_SysWMinfo sysWMinfo{};
		SDL_GetVersion(&sysWMinfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMinfo);

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width						= m_Width;
		swapChainDesc.BufferDesc.Height						= m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator		= 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator	= 60;
		swapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count						= 1;
		swapChainDesc.SampleDesc.Quality					= 0;
		swapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount							= 1;
		swapChainDesc.Windowed								= true;
		swapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags									= 0;
		swapChainDesc.OutputWindow							= sysWMinfo.info.win.window;

		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
		{
			return result;
		}

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width					= m_Width;
		depthStencilDesc.Height					= m_Height;
		depthStencilDesc.MipLevels				= 1;
		depthStencilDesc.ArraySize				= 1;
		depthStencilDesc.Format					= DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count		= 1;
		depthStencilDesc.SampleDesc.Quality		= 0;
		depthStencilDesc.Usage					= D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags				= D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags			= 0;
		depthStencilDesc.MiscFlags				= 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format				= depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice	= 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
		{
			return result;
		}

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
		{
			return result;
		}

		result = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_pRenderTargetBuffer));
		if (FAILED(result))
		{
			return result;
		}

		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
		{
			return result;
		}

		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		D3D11_VIEWPORT viewport{};
		viewport.Width		= static_cast<float>(m_Width);
		viewport.Height		= static_cast<float>(m_Height);
		viewport.TopLeftX	= 0;
		viewport.TopLeftY	= 0;
		viewport.MaxDepth	= 1.0f;
		viewport.MinDepth	= 0.0f;

		m_pDeviceContext->RSSetViewports(1, &viewport);

		pDxgiFactory->Release();

		return S_OK;
	}
}
