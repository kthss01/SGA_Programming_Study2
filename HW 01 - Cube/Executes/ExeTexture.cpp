#include "stdafx.h"
#include "ExeTexture.h"

ExeTexture::ExeTexture(ExecuteValues * values)
	:Execute(values)
	, vertexCount(4), indexCount(6)
	, number(0)
	, location(0, 0), rotation(0, 0, 0), scale(1, 1)
	, number2(0)
{
	shader = new Shader(Shaders + L"005_TextureUV.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;

	vertices = new VertexTexture[vertexCount];
	vertices[0].Position = D3DXVECTOR3(-0.5f, -0.5f, 0);
	vertices[1].Position = D3DXVECTOR3(-0.5f, 0.5f, 0);
	vertices[2].Position = D3DXVECTOR3(0.5f, -0.5f, 0);
	vertices[3].Position = D3DXVECTOR3(0.5f, 0.5f, 0);

	vertices[0].Uv = D3DXVECTOR2(0, 1.0f);
	vertices[1].Uv = D3DXVECTOR2(0, 0);
	vertices[2].Uv = D3DXVECTOR2(1.0f, 1.0f);
	vertices[3].Uv = D3DXVECTOR2(1.0f, 0);

	indices = new UINT[indexCount]{ 0, 1, 2, 2, 1, 3 };

	{
		// dx11 ������ desc�� ��
		D3D11_SAMPLER_DESC desc;
		// �� �Լ� ȣ��� �⺻������ ���õǼ� �Ѿ��
		States::GetSamplerDesc(&desc);

		//CD3D11_SAMPLER_DESC desc2; // �̰͵� �ڵ����� �����ؼ� ������ �Ѿ��
		//// �Ⱦ��� ���� Ʋ������ ������ ������ ���Ƽ� ���� ����Ű�

		// �⺻�� WRAP
		States::CreateSampler(&desc, &state[0]);

		//// CLAMP ����ó�� ��°�
		//desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		//desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		//States::CreateSampler(&desc, &state[1]);

		//// MIRROR (����� �ٸ��� ������ u,v ���� �°� ������)
		//desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		//desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		//States::CreateSampler(&desc, &state[2]);

		//// BORDER Ư�������� ä��°�
		//desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		//desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		//
		//D3DXCOLOR color = D3DXCOLOR(1, 0, 0, 1);
		//memcpy(&desc.BorderColor, &color, sizeof(D3DXCOLOR));
		//States::CreateSampler(&desc, &state[3]);

		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		States::CreateSampler(&desc, &state[1]);
	}

	// CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // ��� ��������� ���� ����
		desc.ByteWidth = sizeof(VertexTexture) * vertexCount; // ���� ���ۿ� �� �������� ũ��
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // �긦 ���ؼ� ���� �� lock ���
		data.pSysMem = vertices; // �� �������� �ּ�

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr)); // �����Ǹ� hr 0���� ū �� �Ѿ��
	}

	// CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // ��� ��������� ���� ����
		desc.ByteWidth = sizeof(UINT) * indexCount; // ���� ���ۿ� �� �������� ũ��
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // �긦 ���ؼ� ���� �� lock ���
		data.pSysMem = indices; // �� �������� �ּ�

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr)); // �����Ǹ� hr 0���� ū �� �Ѿ��
	}

	// Load SRV
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
			D3D::GetDevice(),
			(Textures + L"Box.png").c_str(),
			NULL, // �о�帱�� ������ NULL�̸� �⺻��
			NULL,
			&srv,
			NULL // ���⼭ hr ���� �޾Ƶ���
		);
		assert(SUCCEEDED(hr));
	}

	// Load SRV2
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
			D3D::GetDevice(),
			(Textures + L"Wall.png").c_str(),
			NULL, // �о�帱�� ������ NULL�̸� �⺻��
			NULL,
			&srv2,
			NULL // ���⼭ hr ���� �޾Ƶ���
		);
		assert(SUCCEEDED(hr));
	}

	{
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);
		// �⺻��
		States::CreateRasterizer(&desc, &rasterizer[0]);

		//desc.CullMode = D3D11_CULL_NONE;
		desc.FillMode = D3D11_FILL_WIREFRAME;
		States::CreateRasterizer(&desc, &rasterizer[1]);
	}
}

ExeTexture::~ExeTexture()
{
	// dx�κ��� ��������� release
	// ���� �������� ��������
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);
}

void ExeTexture::Update()
{
	//D3DXMATRIX R;
	//D3DXMatrixIdentity(&R);

	D3DXMATRIX Y, Z;
	D3DXMatrixIdentity(&Y);
	D3DXMatrixIdentity(&Z);

	D3DXMATRIX T;
	D3DXMatrixIdentity(&T);

	if (Keyboard::Get()->Press(VK_SHIFT)) 
	{
		if (Keyboard::Get()->Press('A'))
			rotation.y -= 30.0f * Time::Delta();
		else if(Keyboard::Get()->Press('D'))
			rotation.y += 30.0f * Time::Delta();

		if (Keyboard::Get()->Press('W'))
			rotation.z -= 30.0f * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			rotation.z += 30.0f * Time::Delta();
	}
	else
	{
		if (Keyboard::Get()->Press('A'))
			location.x -= 3.0f * Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			location.x += 3.0f * Time::Delta();

		if (Keyboard::Get()->Press('W'))
			location.y += 3.0f * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			location.y -= 3.0f * Time::Delta();
	}

	D3DXMatrixTranslation(&T, location.x, location.y, 0);

	D3DXMatrixRotationY(&Y, Math::ToRadian(rotation.y));
	D3DXMatrixRotationZ(&Z, Math::ToRadian(rotation.z));

	//R = Y * Z;

	//world = R;
	world = Y * Z * T;
}

void ExeTexture::PreRender()
{
}

void ExeTexture::Render()
{
	UINT stride = sizeof(VertexTexture); // �׸� ũ��
	UINT offset = 0;

	// vertex buffer ������ �� �� ����
	// IA �ٴ� ���� ���߿� �������ֽǲ�
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// �׸� ��� ����
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	colorBuffer->SetPSBuffer(0); // �������� ��ȣ
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	D3D::GetDC()->PSSetSamplers(0, 1, &state[number]);
	// �ȼ� ���̴��� �ؽ�ó �ִ°�
	D3D::GetDC()->PSSetShaderResources(
		0, // 0��
		1, // ���� 
		&srv);
	D3D::GetDC()->PSSetShaderResources(1, 1, &srv2);

	// �����Ͷ����� ����
	D3D::GetDC()->RSSetState(rasterizer[number2]);

	//D3DXMATRIX world, S, R, T;
	//D3DXMatrixIdentity(&world); // identity �������

	//// ��ġ �̵�
	////world._41 = 3; // x������ 3��ŭ �̵�
	////world._42 = 0;
	////world._43 = 0;

	//// ���� ��� �� �༮ ������ identity ���ص� ��
	//// D3DXMatrixTranslation(&world, 3, 0, 0);

	//// �Ѵ� �ϰ� �; �̷��� �ϸ� �������ٸ� �����
	//D3DXMatrixScaling(&S, 2, 3, 1);
	//D3DXMatrixRotationY(&R, Math::ToRadian(45));
	////D3DXMatrixTranslation(&T, 1.5f, 0, 0);

	//world = R;

	////world = S * T;
	////world = T * S;
	//// ���� ���� ���� -> SRT ������ �˰� ����

	worldBuffer->SetMatrix(world);
	worldBuffer->SetVSBuffer(1);

	// ������ �׸��� ��
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);


	//D3DXMATRIX world2;
	//D3DXMatrixIdentity(&world2); // identity �������

	//world2._41 = 1; // x������ 1��ŭ �̵�
	//world2._42 = 0;
	//world2._43 = 0;

	//worldBuffer->SetMatrix(world2);
	//worldBuffer->SetVSBuffer(1);

	//// ������ �׸��� ��
	//D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

void ExeTexture::PostRender()
{
	// â ���� �� ����
	ImGui::Begin("Color");
	{
		//ImGui::SliderInt("Number", &number, 0, 3);
		ImGui::SliderInt("Number", &number, 0, 1);
		ImGui::SliderInt("Number2", &number2, 0, 1);
		ImGui::SliderFloat3("Color", 
			(float *)&colorBuffer->Data.Color, 0, 1);
	}
	ImGui::End();
}

void ExeTexture::ResizeScreen()
{
}