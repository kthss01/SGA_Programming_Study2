#include "stdafx.h"
#include "ExeTriangle.h"

ExeTriangle::ExeTriangle(ExecuteValues * values)
	:Execute(values)
	, vertexCount(3), indexCount(3)
{
	shader = new Shader(Shaders + L"002_Vertex.hlsl");
	worldBuffer = new WorldBuffer();

	vertices = new Vertex[vertexCount];
	vertices[0].Position = D3DXVECTOR3(0, 0, 0);
	vertices[1].Position = D3DXVECTOR3(0, 1, 0);
	vertices[2].Position = D3DXVECTOR3(1, 0, 0);

	indices = new UINT[indexCount]{ 0,1,2 };

	// CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(Vertex) * vertexCount; // 정점 버퍼에 들어갈 데이터의 크기
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		data.pSysMem = vertices; // 쓸 데이터의 주소

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}

	// CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(UINT) * indexCount; // 정점 버퍼에 들어갈 데이터의 크기
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		data.pSysMem = indices; // 쓸 데이터의 주소

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}
}

ExeTriangle::~ExeTriangle()
{
	// dx로부터 만들어진건 release
	// 보통 역순으로 릴리즈함
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);
}

void ExeTriangle::Update()
{
}

void ExeTriangle::PreRender()
{
}

void ExeTriangle::Render()
{
	UINT stride = sizeof(Vertex); // 그릴 크기
	UINT offset = 0;

	// vertex buffer 여러개 들어갈 수 있음
	// IA 붙는 이유 나중에 설명해주실꺼
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 그릴 방식 설정
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);
	shader->Render();

	// 실제로 그리는 거
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

void ExeTriangle::PostRender()
{
}

void ExeTriangle::ResizeScreen()
{
}
