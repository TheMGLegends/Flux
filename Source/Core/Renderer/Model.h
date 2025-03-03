#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <wrl.h>

struct aiScene;

namespace Flux
{
	struct Vertex
	{
	public:
		Vertex() : position(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)), color(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
			texCoord(DirectX::XMFLOAT2(0.0f, 0.0f)), normal(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)) {}
		~Vertex() = default;

	public:
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT3 normal;
	};

	class Model
	{
	public:
		Model();
		~Model();

		void Initialise(ID3D11Device& device, ID3D11DeviceContext& deviceContext, const aiScene* modelData, const std::string& _modelName);
		void Draw(ID3D11DeviceContext& deviceContext);

		inline const std::string& GetModelName() const { return modelName; }

	private:
		std::string modelName;

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};
}

