#include "Model.h"

#include "Core/Debug/Debug.h"

#include <assimp/scene.h>

using namespace Flux;

Model::Model(ID3D11Device& device, ID3D11DeviceContext& deviceContext, const aiScene* modelData, const std::string& _modelName)
{
	if (!modelData)
	{
		Debug::LogError("Model::Initialise() - Invalid modelData");
		return;
	}

	modelName = _modelName;

	if (modelData->HasMeshes())
	{
		aiMesh* mesh = modelData->mMeshes[0];

		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			// INFO: Load Position Data
			if (mesh->HasPositions())
			{
				vertex.position.x = mesh->mVertices[i].x;
				vertex.position.y = mesh->mVertices[i].y;
				vertex.position.z = mesh->mVertices[i].z;
			}

			// INFO: Load Color Data
			if (mesh->HasVertexColors(0))
			{
				vertex.color.x = mesh->mColors[0][i].r;
				vertex.color.y = mesh->mColors[0][i].g;
				vertex.color.z = mesh->mColors[0][i].b;
				vertex.color.w = mesh->mColors[0][i].a;
			}

			// INFO: Load Texture Coordinate Data
			if (mesh->HasTextureCoords(0))
			{
				vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
				vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
			}

			// INFO: Load Normal Data
			if (mesh->HasNormals())
			{
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}

			vertices.push_back(vertex);
		}

		// INFO: Load Index Data
		if (mesh->HasFaces())
		{
			for (size_t i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];

				for (size_t j = 0; j < face.mNumIndices; j++)
				{
					indices.push_back(face.mIndices[j]);
				}
			}
		}
	}
	else
	{
		Debug::LogWarning("Model::Initialise() - Model has no meshes");
		return;
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
