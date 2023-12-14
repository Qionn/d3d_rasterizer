#pragma once

#include "Matrix.h"

#include <string_view>

namespace dae
{
	class Effect final
	{
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

	private:
		ID3DX11Effect* m_pEffect{ nullptr };
		ID3DX11EffectTechnique* m_pTechnique{ nullptr };
		ID3D11InputLayout* m_pInputLayout{ nullptr };

		ID3DX11EffectMatrixVariable* n_pMatWorldViewProjVariable{ nullptr };

	private:
		static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	};
}
