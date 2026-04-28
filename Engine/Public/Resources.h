#pragma once

#include "ResDynamicBuffer.h"
#include "ResCBuffer.h"
#include "ResCubeColBuffer.h"
#include "ResDynamicVIBuffer.h"
#include "ResQuadColBuffer.h"
#include "ResQuadFullscreenTexBuffer.h"
#include "ResQuadTexBuffer.h"

#include "ResFmodSound.h"

#include "ResJson.h"

#include "ResComputeShader.h"
#include "ResGeometryShader.h"
#include "ResGeoShaderStreamOut.h"
#include "ResPixelShader.h"
#include "ResTessDomainShader.h"
#include "ResTessHullShader.h"
#include "ResVertexShader.h"

#include "ResBlendState.h"
#include "ResDepthStencilState.h"
#include "ResRasterizerState.h"
#include "ResSamplerState.h"

#include "ResTexture2D.h"
#include "ResDynamicTexture2D.h"
#include "ResOffscreenTexture.h"
#include "ResTexture2DArray.h"
#include "ResTextureCubeMap.h"

#include "ResViewPort.h"


static const char* TAG_RES_GRP_PERMANENT_BUFFER = "PERMANENT_BUFFER";
static const char* TAG_RES_GRP_PERMANENT_SHADER = "PERMANENT_SHADER";
static const char* TAG_RES_GRP_PERMANENT_TEXTURE = "PERMANENT_TEXTURE";
static const char* TAG_RES_GRP_PERMANENT_STATE = "PERMANENT_STATE";
static const char* TAG_RES_GRP_PERMANENT_VP = "PERMANENT_VP";


// RS_: RasterazerState
// BS_: BlendState
// DS_: DepthStencilState
// SS_: SamplerState
static const char* TAG_RES_STATE_RS_SOLID_BACKCULL = "RS_SOLID_BACKCULL";
static const char* TAG_RES_STATE_RS_SOLID_FRONTCULL = "RS_SOLID_FRONTCULL";
static const char* TAG_RES_STATE_RS_SOLID_NOCULL = "RS_SOLID_NOCULL";
static const char* TAG_RES_STATE_RS_WIREFRAME_NOCULL = "RS_WIREFRAME_NOCULL";

static const char* TAG_RES_STATE_SS_LINEAR_WRAP = "SS_LinearWrap";