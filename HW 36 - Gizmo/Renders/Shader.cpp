#include "stdafx.h"
#include "Shader.h"



Shader::Shader(wstring shaderFile, string vsName, string psName)
	: shaderFile(shaderFile), vsName(vsName), psName(psName)
	, reflection(NULL), inputLayout(NULL)
	, vertexBlob(NULL), vertexShader(NULL)
	, pixelBlob(NULL), pixelShader(NULL)
	, geometryBlob(NULL), geometryShader(NULL)
{
	CreateVertexShader();
	CreatePixelShader();
	CreateInputLayout();
}

Shader::~Shader()
{
	SAFE_RELEASE(reflection);

	SAFE_RELEASE(pixelShader);
	SAFE_RELEASE(pixelBlob);

	SAFE_RELEASE(vertexShader);
	SAFE_RELEASE(vertexBlob);

	SAFE_RELEASE(inputLayout);

	SAFE_RELEASE(geometryBlob);
	SAFE_RELEASE(geometryShader);
}

void Shader::Render()
{
	D3D::GetDC()->IASetInputLayout(inputLayout);
	D3D::GetDC()->VSSetShader(vertexShader, NULL, 0);
	D3D::GetDC()->PSSetShader(pixelShader, NULL, 0);

	if (geometryShader != NULL)
		D3D::GetDC()->GSSetShader(geometryShader, NULL, 0);
}

void Shader::CreateVertexShader()
{
	ID3D10Blob* error;
	HRESULT hr = D3DX11CompileFromFile
	(
		shaderFile.c_str(), NULL, NULL, vsName.c_str(), "vs_5_0"
		, D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &vertexBlob, &error, NULL
	);
	CheckShaderError(hr, error);

	hr = D3D::GetDevice()->CreateVertexShader
	(
		vertexBlob->GetBufferPointer()
		, vertexBlob->GetBufferSize()
		, NULL
		, &vertexShader
	);
	assert(SUCCEEDED(hr));
}

void Shader::CreatePixelShader()
{
	ID3D10Blob* error;
	HRESULT hr = D3DX11CompileFromFile
	(
		shaderFile.c_str(), NULL, NULL, psName.c_str(), "ps_5_0"
		, D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pixelBlob, &error, NULL
	);
	CheckShaderError(hr, error);

	hr = D3D::GetDevice()->CreatePixelShader
	(
		pixelBlob->GetBufferPointer()
		, pixelBlob->GetBufferSize()
		, NULL
		, &pixelShader
	);
	assert(SUCCEEDED(hr));
}

void Shader::CheckShaderError(HRESULT hr, ID3DBlob * error)
{
	if (FAILED(hr))
	{
		if (error != NULL)
		{
			string str = (const char *)error->GetBufferPointer();
			MessageBoxA(NULL, str.c_str(), "Shader Error", MB_OK);
		}
		assert(false);
	}
}

void Shader::CreateInputLayout()
{
	HRESULT hr;
	hr = D3DReflect
	(
		vertexBlob->GetBufferPointer()
		, vertexBlob->GetBufferSize()
		, IID_ID3D11ShaderReflection
		, (void**)&reflection
	);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i = 0; i< shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		reflection->GetInputParameterDesc(i, &paramDesc);

		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		// D3D11_APPEND_ALIGNED_ELEMENT �̳����� ���� �ڵ����� �����
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		// ���̴������� position�� float4�� ���ͼ� �̰Ÿ� float3�� �ִ°�
		string name = paramDesc.SemanticName;
		if (name == "POSITION")
			elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

		if (name == "INSTANCE")
		{
			elementDesc.InputSlot = 1;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			elementDesc.InstanceDataStepRate = 1;
		}

		inputLayoutDesc.push_back(elementDesc);
	}

	hr = D3D::GetDevice()->CreateInputLayout
	(
		&inputLayoutDesc[0]
		, inputLayoutDesc.size()
		, vertexBlob->GetBufferPointer()
		, vertexBlob->GetBufferSize()
		, &inputLayout
	);
	assert(SUCCEEDED(hr));
}

void Shader::CreateGS(string func)
{
	gsName = func;

	ID3D10Blob* error;
	HRESULT hr = D3DX11CompileFromFile
	(
		shaderFile.c_str(), NULL, NULL, gsName.c_str(), "gs_5_0"
		, D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &geometryBlob, &error, NULL
	);
	CheckShaderError(hr, error);

	hr = D3D::GetDevice()->CreateGeometryShader
	(
		geometryBlob->GetBufferPointer()
		, geometryBlob->GetBufferSize()
		, NULL
		, &geometryShader
	);
	assert(SUCCEEDED(hr));
}