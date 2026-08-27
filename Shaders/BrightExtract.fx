// BrightExtract.fx (최종: threshold 뽑기)
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
    float4 s = SrcTex.Sample(Samp, i.UV);

    float luma = dot(s.rgb, float3(0.2126, 0.7152, 0.0722));
    float m = step(Threshold, luma); // threshold 이상이면 1

    return float4(s.rgb * m, s.a);
}
