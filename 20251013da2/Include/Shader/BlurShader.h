// BlurShader.h
#pragma once
#include "GraphicShader.h"

class CBlurShader : public CGraphicShader
{
public:
    CBlurShader();
    ~CBlurShader();

public:
    bool Init() override;
};
