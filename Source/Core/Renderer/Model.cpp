#include "Model.h"

#include <assimp/scene.h>
#include <d3d11.h>

#include "Core/Debug/Debug.h"

namespace Flux
{
	Model::Model(ID3D11Device& device, ID3D11DeviceContext& deviceContext, const aiScene* modelData, std::string_view _modelName)
	{
		if (!modelData)
		{
			Debug::LogError("Model::Initialise() - Invalid modelData");
			return;
		}

		modelName = _modelName;

		if (!modelData->HasMeshes())
		{
			Debug::LogError("Model::Initialise() - Model has no meshes");
			return;
		}

		const aiMesh* mesh = modelData->mMeshes[0];

		// INFO: Find which index has the vertex colors
		unsigned int vertexColourIndex = 0;
		for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; i++)
		{
			if (mesh->HasVertexColors(i))
			{
				vertexColourIndex = i;
				break;
			}
		}

		// INFO: Find which index has the texture coordinates
		unsigned int textureCoordinateIndex = 0;
		for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++)
		{
			if (mesh->HasTextureCoords(i))
			{
				textureCoordinateIndex = i;
				break;
			}
		}

		for (size_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++)
		{
			Vertex vertex;

			// INFO: Load Position Data
			if (mesh->HasPositions())
			{
				vertex.position.x = mesh->mVertices[vertexIndex].x;
				vertex.position.y = mesh->mVertices[vertexIndex].y;
				vertex.position.z = mesh->mVertices[vertexIndex].z;
			}

			// INFO: Load Color Data
			if (mesh->HasVertexColors(vertexColourIndex))
			{
				vertex.color.x = mesh->mColors[0][vertexIndex].r;
				vertex.color.y = mesh->mColors[0][vertexIndex].g;
				vertex.color.z = mesh->mColors[0][vertexIndex].b;
				vertex.color.w = mesh->mColors[0][vertexIndex].a;
			}

			// INFO: Load Texture Coordinate Data
			if (mesh->HasTextureCoords(textureCoordinateIndex))
			{
				vertex.texCoord.x = mesh->mTextureCoords[0][vertexIndex].x;
				vertex.texCoord.y = mesh->mTextureCoords[0][vertexIndex].y;
			}

			// INFO: Load Normal Data
			if (mesh->HasNormals())
			{
				vertex.normal.x = mesh->mNormals[vertexIndex].x;
				vertex.normal.y = mesh->mNormals[vertexIndex].y;
				vertex.normal.z = mesh->mNormals[vertexIndex].z;
			}

			vertices.push_back(vertex);
		}

		// INFO: Load Index Data
		if (!mesh->HasFaces())
		{
			Debug::LogError("Model::Initialise() - Model has no faces");
			return;
		}

		for (size_t faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
		{
			aiFace face = mesh->mFaces[faceIndex];

			for (size_t indicesIndex = 0; indicesIndex < face.mNumIndices; indicesIndex++)
			{
				indices.push_back(face.mIndices[indicesIndex]);
			}
		}

		D3D11_BUFFER_DESC vbd = {};
		vbd.Usage = D3D11_USAGE_DYNAMIC;
		vbd.ByteWidth = (UINT)(sizeof(Vertex) * vertices.size());
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (FAILED(device.CreateBuffer(&vbd, nullptr, &vertexBuffer)))
		{
			Debug::LogError("Model::Initialise() - Failed to create vertex buffer");
			return;
		}

		D3D11_MAPPED_SUBRESOURCE msr = {};
		deviceContext.Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		memcpy(msr.pData, vertices.data(), sizeof(Vertex) * vertices.size());
		deviceContext.Unmap(vertexBuffer.Get(), 0);

		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.ByteWidth = (UINT)(sizeof(unsigned int) * indices.size());
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices.data();

		if (FAILED(device.CreateBuffer(&ibd, &isd, &indexBuffer)))
		{
			Debug::LogError("Model::Initialise() - Failed to create index buffer");
			return;
		}
	}

	Model::~Model()
	{
		vertices.clear();
		indices.clear();
	}

	void Model::Draw(ID3D11DeviceContext& deviceContext)
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		deviceContext.IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext.IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		deviceContext.DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
	}

	const std::string& Model::GetModelName() const
	{
		return modelName;
	}
}
