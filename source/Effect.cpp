#include "pch.h"
#include "Effect.h"
#include "Texture.h"

#include <cassert>

namespace dae
{
	/* static */ Effect::Technique Effect::m_Technique{ Technique::TexturePoint };

	Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
		: m_pEffect{ LoadEffect(pDevice, assetFile) }
	{
		m_pTexturePointTechnique = FindTechnique("TexturePointTechnique");
		m_pTextureLinearTechnique = FindTechnique("TextureLinearTechnique");
		m_pTextureAnisotropicTechnique = FindTechnique("TextureAnisotropicTechnique");

		static constexpr uint32_t numElements{ 3 };
		D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

		vertexDesc[0].SemanticName		= "POSITION";
		vertexDesc[0].Format			= DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[0].AlignedByteOffset	= 0;
		vertexDesc[0].InputSlotClass	= D3D11_INPUT_PER_VERTEX_DATA;
		
		vertexDesc[1].SemanticName		= "TEXCOORD";
		vertexDesc[1].Format			= DXGI_FORMAT_R32G32_FLOAT;
		vertexDesc[1].AlignedByteOffset	= 12;
		vertexDesc[1].InputSlotClass	= D3D11_INPUT_PER_VERTEX_DATA;

		vertexDesc[2].SemanticName		= "COLOR";
		vertexDesc[2].Format			= DXGI_FORMAT_R32G32B32_FLOAT;
		vertexDesc[2].AlignedByteOffset	= 20;
		vertexDesc[2].InputSlotClass	= D3D11_INPUT_PER_VERTEX_DATA;

		D3DX11_PASS_DESC passDesc{};
		m_pTexturePointTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

		HRESULT result = pDevice->CreateInputLayout(
			vertexDesc,
			numElements,
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize,
			&m_pInputLayout
		);

		if (FAILED(result))
		{
			std::cout << "Failed to create input layout\n";
			assert(false);
		}

		m_pMatWorldViewProjVariable = FindVariable("gWorldViewProj")->AsMatrix();
		m_pDiffuseMapVariable = FindVariable("gDiffuseMap")->AsShaderResource();
	}

	Effect::~Effect()
	{
		if (m_pInputLayout) m_pInputLayout->Release();
		if (m_pEffect) m_pEffect->Release();
	}

	ID3DX11Effect* Effect::GetEffect() const
	{
		return m_pEffect;
	}

	ID3DX11EffectTechnique* Effect::GetTechnique() const
	{
		switch (m_Technique)
		{
			case Technique::TexturePoint:		return m_pTexturePointTechnique;
			case Technique::TextureLinear:		return m_pTextureLinearTechnique;
			case Technique::TextureAnisotropic:	return m_pTextureAnisotropicTechnique;
		}

		return m_pTexturePointTechnique;
	}

	ID3D11InputLayout* Effect::GetInputLayout() const
	{
		return m_pInputLayout;
	}

	void Effect::SetWorldViewProjMatrix(const Matrix& matrix)
	{
		m_pMatWorldViewProjVariable->SetMatrix(matrix.GetData());
	}

	void Effect::SetDiffuseMap(const Texture* pTexture)
	{
		if (pTexture != nullptr)
		{
			m_pDiffuseMapVariable->SetResource(pTexture->GetShaderResourceView());
		}
	}

	void Effect::CycleTechnique()
	{
		switch (m_Technique)
		{
			case Technique::TexturePoint:
				m_Technique = Technique::TextureLinear;
				break;

			case Technique::TextureLinear:
				m_Technique = Technique::TextureAnisotropic;
				break;

			case Technique::TextureAnisotropic:
				m_Technique = Technique::TexturePoint;
				break;
		}
	}

	ID3DX11EffectTechnique* Effect::FindTechnique(const std::string_view& name) const
	{
		ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByName(name.data());
		if (!pTechnique->IsValid())
		{
			std::cout << "Couldn't find \"" << name << "\" technique!\n";
			assert(false);
		}
		return pTechnique;
	}

	ID3DX11EffectVariable* Effect::FindVariable(const std::string_view& name) const
	{
		ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(name.data());
		if (!pVariable->IsValid())
		{
			std::cout << "Couldn't find \"" << name << "\n as shader variable!\n";
			assert(false);
		}
		return pVariable;
	}

	ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
	{
		HRESULT result;
		ID3D10Blob* pErrorBlob{ nullptr };
		ID3DX11Effect* pEffect{ nullptr };

		DWORD shaderFlags = 0;
	#if defined(_DEBUG) || defined(DEBUG)
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

		result = D3DX11CompileEffectFromFile(
			assetFile.c_str(),
			nullptr,
			nullptr,
			shaderFlags,
			0,
			pDevice,
			&pEffect,
			&pErrorBlob);

		if (FAILED(result))
		{
			if (pErrorBlob != nullptr)
			{
				const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

				std::wstringstream ss;
				for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
				{
					ss << pErrors[i];
				}

				OutputDebugStringW(ss.str().c_str());

				pErrorBlob->Release();
				pErrorBlob = nullptr;

				std::wcout << ss.str() << std::endl;
			}
			else
			{
				std::wstringstream ss;
				ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
				std::wcout << ss.str() << std::endl;
			}

			return nullptr;
		}

		return pEffect;
	}
}
