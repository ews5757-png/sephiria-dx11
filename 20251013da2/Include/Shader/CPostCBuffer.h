#pragma once
#include "ConstantBufferData.h"
#include "../GameInfo.h"

class CPostCBuffer : public CConstantBufferData
{
public:
    CPostCBuffer() {}
    CPostCBuffer(const CPostCBuffer& other)
        : CConstantBufferData(other), mInfo(other.mInfo)
    {
    }

    virtual ~CPostCBuffer() {}

private:
    FPostCBufferInfo mInfo = {};

public:
    virtual bool Init() override
    {
        SetConstantBuffer("Post");   // ShaderManager에서 만든 CBuffer
        return mBuffer != nullptr;
    }

    void SetTexelSize(int w, int h)
    {
        mInfo.TexelSize = FVector2D(1.f / (float)w, 1.f / (float)h);
    }

    void SetDirection(float x, float y)
    {
        mInfo.Direction = FVector2D(x, y);
    }

    void SetThreshold(float t) { mInfo.Threshold = t; }
    void SetIntensity(float i) { mInfo.Intensity = i; }

    virtual void UpdateBuffer() override
    {
        // 네 엔진에선 Update()가 곧 "복사 + 바인딩"
        mBuffer->Update(&mInfo);
    }

    virtual CConstantBufferData* Clone() override
    {
        return new CPostCBuffer(*this);
    }
};
