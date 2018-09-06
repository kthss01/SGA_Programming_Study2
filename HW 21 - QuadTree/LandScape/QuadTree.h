#pragma once

class QuadTree
{
public:
	static const UINT DivideCount;

private:
	struct NodeType;

public:
	QuadTree(ExecuteValues* values, class Terrain* terrain);
	~QuadTree();

	void Update();
	void Render();

private:
	void RenderNode(NodeType* node);
	void DeleteNode(NodeType* node);

	void CalcMeshDimensions(UINT vertexCount, 
		float& centerX, float& centerZ, float& meshWidth);
	void CreateTreeNode(struct NodeType* node, float positionX, float positionZ, float width);

	UINT ContainTriangleCount(float positionX, float positionZ, float width);
	bool IsTriangleContained(UINT index, float positionX, float positionZ, float width);

	void RenderDebugLine(D3DXVECTOR3& center, float& width);

private:
	ExecuteValues* values;

	UINT triangleCount, drawCount;
	VertexTextureNormal* vertices;

	NodeType* parent;
	class Frustum* frustum;

	WorldBuffer* worldBuffer; // ����� ��帶�� �־�� ������ �׳� ��ü���� �����
	Material* material;

	RasterizerState* rasterizer[2];
	Perspective* perspective;

	float fWidth, fHeight;

	bool isFrameMode;
	bool isDebugMode;
private:
	class DebugDrawCube * debug;

private:
	// ���� ��� �ȿ� �ؽ�ó�� ���ߵ� �������� ��� �ȿ� �ִ°� �Ҳ�
	struct NodeType
	{
		float X, Z, Width;
		UINT TriangleCount;
		ID3D11Buffer* VertexBuffer, *IndexBuffer;

		NodeType* Childs[4];
	};

private:
	class LineBuffer : public ShaderBuffer
	{
	public:
		LineBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Type = 0;
			Data.Color = D3DXVECTOR3(1, 1, 1);

			Data.Spacing = 16;
			Data.Thickness = 0.01f;
		}

		struct Struct
		{
			int Type;
			D3DXVECTOR3 Color;

			int Spacing;
			float Thickness;

			float Padding[2];
		} Data;
	};

	LineBuffer* lineBuffer;
};