#include "pch.h"
#include "Mesh.h"

#include <cassert>

namespace dae
{
	Mesh::Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
		: m_Vertices{ vertices }
		, m_Indices{ indices }
		, m_NumIndices{ static_cast<uint32_t>(indices.size()) }
		, m_pEffect{ std::make_unique<Effect>(pDevice, L"Resources/PosCol3D.fx")}
	{
		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage			= D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth		= sizeof(Vertex) * static_cast<uint32_t>(m_Vertices.size());
		bufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags	= 0;
		bufferDesc.MiscFlags		= 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = m_Vertices.data();

		HRESULT result = pDevice->CreateBuffer(&bufferDesc, &initData, &m_pVertexBuffer);
		if (FAILED(result))
		{
			std::wcout << L"Failed to create vertex buffer\n";
			assert(false);
		}
		
		bufferDesc.Usage			= D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth		= sizeof(Vertex) * m_NumIndices;
		bufferDesc.BindFlags		= D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags	= 0;
		bufferDesc.MiscFlags		= 0;

		initData.pSysMem = m_Indices.data();

		result = pDevice->CreateBuffer(&bufferDesc, &initData, &m_pIndexBuffer);
		if (FAILED(result))
		{
			std::wcout << L"Failed to create index buffer\n";
			assert(false);
		}
	}

	Mesh::~Mesh()
	{
		if (m_pIndexBuffer) m_pIndexBuffer->Release();
		if (m_pVertexBuffer) m_pVertexBuffer->Release();
	}

	void Mesh::Render(const Camera& camera, ID3D11DeviceContext* pDeviceContext) const
	{
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->IASetInputLayout(m_pEffect->GetInputLayout());

		constexpr UINT stride = sizeof(Vertex);
		constexpr UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		m_pEffect->SetWorldViewProjMatrix(worldMatrix * camera.viewMatrix * camera.projectionMatrix);

		D3DX11_TECHNIQUE_DESC techniqueDesc{};
		m_pEffect->GetTechnique()->GetDesc(&techniqueDesc);
		for (UINT i = 0; i < techniqueDesc.Passes; ++i)
		{
			m_pEffect->GetTechnique()->GetPassByIndex(i)->Apply(0, pDeviceContext);
			pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
		}
	}
}
