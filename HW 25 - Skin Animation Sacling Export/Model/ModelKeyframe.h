#pragma once

class ModelKeyframe
{
public:
	struct Transform;
	friend class ModelClip;

	ModelKeyframe();
	~ModelKeyframe();

	D3DXMATRIX GetInterpolatedMatrix(float time, bool bRepeat);

private:
	// time은 deltaTime 값 들어올꺼
	UINT GetKeyframeIndex(float time);
	void CalcKeyframeIndex(float time, bool bRepeat,
		OUT UINT& index1, OUT UINT& index2, OUT float& interpolatedTime);

	// 최종적으로 본에 들어갈 매트릭스
	D3DXMATRIX GetInterpolatedMatrix(UINT index1, UINT index2, float t);

private:
	wstring boneName;

	float duration;
	float frameRate;
	UINT frameCount;

	vector<Transform> transforms;
	
public:

	struct Transform
	{
		float Time;

		D3DXVECTOR3 S;
		D3DXQUATERNION R;
		D3DXVECTOR3 T;
	};
};