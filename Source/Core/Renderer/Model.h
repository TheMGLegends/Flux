#pragma once

#define NOMINMAX

#include <DirectXMath.h>
#include <string>
#include <vector>
#include <wrl.h>

struct aiScene;
struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace Flux
{
	class Model
	{
	public:
		struct Vertex
		{
		public:
			Vertex() = default;
			~Vertex() = default;

		public:
			DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			DirectX::XMFLOAT2 texCoord = DirectX::XMFLOAT2(0.0f, 0.0f);
			DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		};

	public:
		Model(ID3D11Device& device, ID3D11DeviceContext& deviceContext, const aiScene* modelData, std::string_view _modelName);
		~Model();

		void Draw(ID3D11DeviceContext& deviceContext);

		const std::string& GetModelName() const;

	private:
		std::string modelName;

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};
}

