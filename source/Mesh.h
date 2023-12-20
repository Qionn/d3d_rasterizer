#pragma once

#include "Camera.h"
#include "Effect.h"
#include "Matrix.h"
#include "Texture.h"

namespace dae
{
	struct Vertex
	{
		Vector3 position;
		Vector2 texCoord;
		ColorRGB color{ colors::White };
	};

	class Mesh final
	{
	public:
		Matrix worldMatrix;

	public:
		Mesh(ID3D11Device* pDevice, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		~Mesh();

		Mesh(const Mesh&)				= delete;
		Mesh& operator=(const Mesh&)	= delete;
		Mesh(Mesh&&)					= delete;
		Mesh& operator=(Mesh&&)			= delete;

		void Render(const Camera& camera, ID3D11DeviceContext* pDeviceContext) const;

		void SetDiffuseMap(const std::string_view& filepath);

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		uint32_t m_NumIndices;

		std::unique_ptr<Effect> m_pEffect;
		std::unique_ptr<Texture> m_pDiffuseMap;

		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;

		ID3D11Device* m_pDevice;
	};
}
