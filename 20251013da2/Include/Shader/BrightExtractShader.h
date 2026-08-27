#pragma once
#include "GraphicShader.h"

class CBrightExtractShader : public CGraphicShader
{
public:
    CBrightExtractShader() {}
    virtual ~CBrightExtractShader() {}

    virtual bool Init() override
    {
        if (!LoadVertexShader("VS", TEXT("BrightExtract.fx"))) return false;
        if (!LoadPixelShader("PS", TEXT("BrightExtract.fx"))) return false;
        AddInputLayoutDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);
        AddInputLayoutDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0);
        return CreateInputLayout();
    }
};
