#pragma once
#include "GraphicShader.h"

class CCopyShader : public CGraphicShader
{
public:
    CCopyShader();
    virtual ~CCopyShader();

public:
    virtual bool Init() override;
};