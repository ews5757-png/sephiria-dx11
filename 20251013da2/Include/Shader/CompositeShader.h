// CompositeShader.h
#pragma once
#include "GraphicShader.h"

class CCompositeShader : public CGraphicShader
{
public:
    CCompositeShader();
    ~CCompositeShader();

public:
    bool Init() override;
};
