#include "stdafx.h"
#include "Program.h"

#include "./Viewer/Freedom.h"

#include "./Executes/ExportMesh.h"

#include "./Executes/ExCharacterTool.h"
#include "./Executes/ExRetargeting.h"
#include "./Executes/ExHomework.h"

Program::Program()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values = new ExecuteValues();
	values->ViewProjection = new ViewProjectionBuffer();
	values->GlobalLight = new LightBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);

	values->MainCamera = new Freedom();
	values->MainCamera->RotationDegree(20.0f, -92.0f);
	values->MainCamera->Position(78, 50.0f, 30.0f);
	//values->MainCamera->Position(82.18f, 27.00f, -29.00f);
	// 12.5f, 90.0f, -300.0f
	//values->MainCamera->Position(0.0f, 120.0f, -250.0f);
	//values->MainCamera->Position(12.5f, 90.0f, -300.0f);
	//values->MainCamera->Position(110.0f, 90.0f, -300.0f);

	values->GlobalLight->Data.Direction = D3DXVECTOR3(-1, 0, -1);
	
	//executes.push_back(new ExportMesh(values));

	//executes.push_back(new ExCharacterTool(values));
	//executes.push_back(new ExRetargeting(values));
	executes.push_back(new ExHomework(values));

	showDebug = true;
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	SAFE_DELETE(values->GlobalLight);
	SAFE_DELETE(values->ViewProjection);
	SAFE_DELETE(values->Perspective);
	SAFE_DELETE(values->Viewport);
	SAFE_DELETE(values);
}

void Program::Update()
{
	values->MainCamera->Update();

	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{
	D3DXMATRIX view, projection;
	values->MainCamera->Matrix(&view);
	values->Perspective->GetMatrix(&projection);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);

	for (Execute* exe : executes)
		exe->PreRender();
}

void Program::Render()
{
	D3DXMATRIX view, projection;
	values->MainCamera->Matrix(&view);
	values->Perspective->GetMatrix(&projection);

	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);

	values->GlobalLight->SetPSBuffer(0);
	values->Viewport->RSSetViewport();

	for (Execute* exe : executes)
		exe->Render();
}

void Program::PostRender()
{
	ImGui::Text("FPS : %f", Time::Get()->FPS());

	if (ImGui::Checkbox("Show Debug", &showDebug)) {
		if (showDebug)
			ImGui::SetWindowSize(ImVec2(250, 300));
		else
			ImGui::SetWindowSize(ImVec2(250, 100));
	}

	if (showDebug) {
		// ������ ����
		ImGui::Separator();
		ImGui::SliderFloat3("Direction",
			(float *)&values->GlobalLight->Data.Direction, -1, 1);
		ImGui::Separator();

		D3DXVECTOR3 vec;
		values->MainCamera->Position(&vec);
		ImGui::LabelText("CamearPos", "%.2f, %.2f, %.2f",
			vec.x, vec.y, vec.z);

		D3DXVECTOR2 rot;
		values->MainCamera->RotationDegree(&rot);
		ImGui::LabelText("CameraRot", "%.2f, %.2f", rot.x, rot.y);

		for (Execute* exe : executes)
			exe->PostRender();
	}
}

void Program::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	values->Perspective->Set(desc.Width, desc.Height);
	values->Viewport->Set(desc.Width, desc.Height);

	for (Execute* exe : executes)
		exe->ResizeScreen();
}