// Blur.fx (최종: 가우시안 9탭)
Texture2D SrcTex : register(t0);
SamplerState Samp : register(s0);

cbuffer Post : register(b5)
{
    float2 TexelSize;
    float2 Direction;
    float Threshold;
    float Intensity;
    float2 Padding;
};

struct VS_IN
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD0;
};
struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};

VS_OUT VS(VS_IN i)
{
    VS_OUT o;
    o.Pos = float4(i.Pos.xy * 2.0f, 0.0f, 1.0f);
    o.UV = i.UV;
    return o;
}

float4 PS(VS_OUT i) : SV_Target
{
    float2 stepUV = TexelSize * Direction;

    float4 c = 0;
    c += SrcTex.Sample(Samp, i.UV + stepUV * -4.0f) * 0.016216f;
    c += SrcTex.Sample(Samp, i.UV + stepUV * -3.0f) * 0.054054f;
    c += SrcTex.Sample(Samp, i.UV + stepUV * -2.0f) * 0.121622f;
    c += SrcTex.Sample(Samp, i.UV + stepUV * -1.0f) * 0.194594f;
    c += SrcTex.Sample(Samp, i.UV) * 0.227027f;
    c += SrcTex.Sample(Samp, i.UV + stepUV * 1.0f) * 0.194594f;
    c += SrcTex.Sample(Samp, i.UV + stepUV * 2.0f) * 0.121622f;
    c += SrcTex.Sample(Samp, i.UV + stepUV * 3.0f) * 0.054054f;
    c += SrcTex.Sample(Samp, i.UV + stepUV * 4.0f) * 0.016216f;
    return c;
}
