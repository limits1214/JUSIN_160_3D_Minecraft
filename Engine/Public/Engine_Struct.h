#pragma once

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND hWnd;
		HINSTANCE hInstance;
		WINMODE eWinMode;
		uint32_t iWinSizeX, iWinSizeY;
		uint32_t		iNumLevels;
	} ENGINE_DESC;

	
	typedef struct tagWorkerTask
	{
		_string sTaskName;
		_Func func;
	} WORKER_TASK;


	typedef struct tagVertexCol
	{
		_float3 pos;
		_float4 color;
	} VTX_COL;

	typedef struct tagVertexTex
	{
		_float3 pos;
		_float2 coord;
	} VTX_TEX;

	typedef struct tagVertexNormal
	{
		_float3 pos;
		_float3 normal;
	} VTX_NORMAL;

	typedef struct tagVertexNormalTex
	{
		_float3 pos;
		_float3 normal;
		_float2 coord;
	} VTX_NORMAL_TEX;


	typedef struct tagConstantBufferPerFrame
	{
		_float4x4  matView{};            // 뷰 행렬
		_float4x4  matProj{};            // 투영 행렬 (Perspective 또는 Ortho)
		_float4x4  matViewProj{};        // 곱해진 행렬 (VS에서 연산 절약)
		_float4x4  matInvView{};			// 뷰 역행렬 (빌보드 계산이나 월드 좌표 복원용)
		_float3 vCamPos{};
		_float _padding{};
	} CB_PER_FRAME;

	typedef struct tagConstantBufferPerObject
	{
		_float4x4 matWorld{};
		_float4x4 matWVP{};
	} CB_PER_OBJECT;
}