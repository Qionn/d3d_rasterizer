#pragma once

#include "Effect.h"

namespace dae
{
	struct Vertex
	{
		Vector3 position;
		ColorRGB color;
	};

	class Mesh final
	{
	public:
		Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~Mesh();

		Mesh(const Mesh&)				= delete;
		Mesh& operator=(const Mesh&)	= delete;
		Mesh(Mesh&&)					= delete;
		Mesh& operator=(Mesh&&)			= delete;

		void Render(ID3D11DeviceContext* pDeviceContext) const;

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		uint32_t m_NumIndices;

		std::unique_ptr<Effect> m_pEffect;

		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
	};
}
