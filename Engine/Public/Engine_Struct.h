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

	typedef struct tagVoxelManagerStateUpdateDesc
	{
		VOXEL_MANAGER_STATE eState;
		void* pArg;
	}VOXEL_MANAGER_STATE_UPDATE_DESC;

	
	typedef struct tagWorkerTask
	{
		_string sTaskName;
		_Func func;
	} WORKER_TASK;

	typedef struct tagMaterial
	{
		_float4 ambient{};
		_float4 diffuse{};
		_float4 specular{};
		_float4 reflect{};
	} MATERIAL;

	typedef struct tagDirectionalLight
	{
		_float4 ambient{};
		_float4 diffuse{};
		_float4 specular{};
		_float3 direction{};
		_float _pad{};
	} DIRECTIONAL_LIGHT;

	typedef struct tagPointLight
	{
		_float4 ambient{};
		_float4 diffuse{};
		_float4 specular{};
		_float3 pos{};
		_float range{};
		_float3 att{};//감쇠
		_float _pad{};
	} POINT_LIGHT;

	typedef struct tagSpotLight
	{
		_float4 ambient{};
		_float4 diffuse{};
		_float4 specular{};
		_float3 pos{};
		_float range{};
		_float3 direction{};
		_float spot{};
		_float3 att{};//감쇠
		_float _pad{};
	} SPOT_LIGHT;



	typedef struct tagVertexCol
	{
		_float3 pos;
		_float4 color;
	} VTX_COL;

	typedef struct tagVertexTex
	{
		_float3 pos;
		_float2 texCoord;
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
		_float2 texCoord;
	} VTX_NORMAL_TEX;

	typedef struct tagVertexVoxel
	{
		_float3 pos;
		uint32_t packedData;
	} VTX_VOXEL;


	typedef struct tagConstantBufferPerFrame
	{
		DIRECTIONAL_LIGHT dirLight{};
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

	typedef struct tagConstantBufferPerMaterial
	{
		MATERIAL mat{};
	} CB_PER_MATERIAL;

	typedef struct tagVoxQuad
	{
		_float3 v1;
		_float3 v2;
		_float3 v3;
		_float3 v4;
		FACE_DIR eDir;
	} VOX_QUAD;
}