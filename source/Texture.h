#pragma once

#include <string_view>

namespace dae
{
	class Texture final
	{
	public:
		Texture(ID3D11Device* pDevice, const std::string_view& filepath);
		~Texture();

		Texture(const Texture&)				= delete;
		Texture& operator=(const Texture&)	= delete;
		Texture(Texture&&)					= delete;
		Texture& operator=(Texture&&)		= delete;

		ID3D11ShaderResourceView* GetShaderResourceView() const;

	private:
		ID3D11Texture2D* m_pBuffer;
		ID3D11ShaderResourceView* m_pShaderResourceView{ nullptr };
	};
}
