#pragma once

#include "Matrix.h"

#include <string_view>

namespace dae
{
	class Texture;

	class Effect final
	{
	public:
		enum class Technique
		{
			TexturePoint,
			TextureLinear,
			TextureAnisotropic
		};

	public:
		Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
		~Effect();

		Effect(const Effect&)				= delete;
		Effect& operator=(const Effect&)	= delete;
		Effect(Effect&&)					= delete;
		Effect& operator=(Effect&&)			= delete;

		ID3DX11Effect* GetEffect() const;
		ID3DX11EffectTechnique* GetTechnique() const;
		ID3D11InputLayout* GetInputLayout() const;

		void SetWorldViewProjMatrix(const Matrix& matrix);
		void SetDiffuseMap(const Texture* pTexture);

		static void CycleTechnique();

	private:
		ID3DX11Effect* m_pEffect{ nullptr };
		ID3DX11EffectTechnique* m_pTexturePointTechnique{ nullptr };
		ID3DX11EffectTechnique* m_pTextureLinearTechnique{ nullptr };
		ID3DX11EffectTechnique* m_pTextureAnisotropicTechnique{ nullptr };
		ID3D11InputLayout* m_pInputLayout{ nullptr };

		ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{ nullptr };
		ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };

		static Technique m_Technique;

	private:
		ID3DX11EffectTechnique* FindTechnique(const std::string_view& name) const;
		ID3DX11EffectVariable* FindVariable(const std::string_view& name) const;

		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	};
}
