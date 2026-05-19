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


	typedef struct tagRenderContext
	{
		RENDERPASS pass;
		_vector eye{};
		_matrix matView{};
		_matrix matProj{};
		_matrix matViewProj{};
	} RENDER_CTX;


	
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
		_float3 pos{};
		uint32_t packedData{};
	} VTX_VOXEL;

	typedef struct tagVertexEntity
	{
		_float3 pos{};   // 12 bytes
		_float3 normal{};     // 12 bytes  (조명 계산용)
		_float2 texCoord{}; //  8 bytes
		//_float4 boneWeight{}; // 16 bytes  (스키닝용, 마크는 1본만 쓰니까 index만 있어도 됨)
		uint32_t boneIndex{};  //  4 bytes
		uint32_t texIndex{};  //  4 bytes
	} VTX_ENTITY;

	typedef struct tagVertexItem
	{
		_float3 pos{};   // 12 bytes
		_float3 normal{};     // 12 bytes  (조명 계산용)
		_float2 texCoord{}; //  8 bytes
		uint32_t texIndex{};  //  4 bytes
	} VTX_ITEM;


	typedef struct tagVertexPointParticle
	{
		_float3 pos{};   // 12 bytes
		_float2 texCoord{}; //  8 bytes
		_float2 uvSize{}; // textureSize
		_float2 size{}; //  world size
		_float rotation{};
		_float4 color{};
		uint32_t texIndex{};  //  4 bytes
		uint32_t frameIndex{};
		uint32_t flag{};
		uint32_t _pad;
	} VTX_POINT_PARTICLE;


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

	typedef struct tagConstantBufferPerBone
	{
		_float4x4 matBone[64];
	} CB_PER_BONE;

	typedef struct tagConstantBufferPerQuadItemAnim
	{
		_float2 uvOffset{ 0,0 };
		_float2 uvScale{1,1};
	} CB_PER_QUADITEM_ANIM;

	typedef struct tagConstantBufferPerBlockOutline
	{
		_float3 vBlockPos{};
		_float fThickness{};
		_float4 vColor{};
		_float3 vExtents{};
		_float _pad{};
	} CB_PER_BLOCKOUTLINE;

	typedef struct tagVoxQuad
	{
		_float3 v1;
		_float3 v2;
		_float3 v3;
		_float3 v4;
		FACE_DIR eDir;
	} VOX_QUAD;


	typedef struct tagEntityCube
	{
		_float3 origin{};
		_float3 size{};
		_float3 rotation{};
		_float2 uv{};
		uint32_t texIndex{};
		_float inflate;
		_float3 uvSize{};
		_float3 pivot{};
		_string annotation{};
	} ENTITY_CUBE;

	typedef struct tagEntityBone
	{
		std::string name{};
		std::string parent{};
		_float3 pivot{};
		std::vector<ENTITY_CUBE> cubes{};
		_bool mirror{ false };
		std::vector<std::pair<std::string, _float3>> locators{};
		bool neverRender{};
	} ENTITY_BONE;

	typedef struct tagEntityModel
	{
		uint32_t texWidth{};
		uint32_t texHeight{};
		std::vector<ENTITY_BONE> bones{};
	} ENTITY_GEOMETRY;

	

	//struct EntityCube
	//{
	//	_float3 origin{};
	//	_float3 size{};
	//	_float3 rotation{};
	//	_float2 uv{};
	//	uint32_t texIndex{};
	//	_float inflate;
	//	_float3 uvSize{};
	//	_float3 pivot{};
	//};

	//struct EntityBone
	//{
	//	std::string name{};
	//	std::string parent{};
	//	_float3 pivot{};
	//	std::vector<EntityCube> cubes{};
	//	_bool mirror{ false };
	//	std::vector<std::pair<std::string, _float3>> locators{};
	//};





}