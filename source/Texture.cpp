#include "pch.h"
#include "Texture.h"

#include <cassert>
#include <iostream>

namespace dae
{
	Texture::Texture(ID3D11Device* pDevice, const std::string_view& filepath)
	{
		SDL_Surface* pSurface = IMG_Load(filepath.data());
		if (pSurface == nullptr)
		{
			std::cout << "Failed to load image \"" << filepath << "\" into memory!\n";
			assert(false);
		}

		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

		D3D11_TEXTURE2D_DESC desc{};
		desc.Width					= pSurface->w;
		desc.Height					= pSurface->h;
		desc.MipLevels				= 1;
		desc.ArraySize				= 1;
		desc.Format					= format;
		desc.SampleDesc.Count		= 1;
		desc.SampleDesc.Quality		= 0;
		desc.Usage					= D3D11_USAGE_DEFAULT;
		desc.BindFlags				= D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags			= 0;
		desc.MiscFlags				= 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

		HRESULT hr = pDevice->CreateTexture2D(&desc, &initData, &m_pBuffer);
		if (FAILED(hr))
		{
			std::cout << "Failed to create Texture2D! (" << filepath << ")\n";
			assert(false);
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format				= format;
		srvDesc.ViewDimension		= D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels	= 1;

		hr = pDevice->CreateShaderResourceView(m_pBuffer, &srvDesc, &m_pShaderResourceView);
		if (FAILED(hr))
		{
			std::cout << "Failed to create ShaderResourceView! (" << filepath << ")\n";
			assert(false);
		}

		SDL_FreeSurface(pSurface);
	}

	Texture::~Texture()
	{
		if (m_pShaderResourceView) m_pShaderResourceView->Release();
		if (m_pBuffer) m_pBuffer->Release();
	}

	ID3D11ShaderResourceView* Texture::GetShaderResourceView() const
	{
		return m_pShaderResourceView;
	}
}

