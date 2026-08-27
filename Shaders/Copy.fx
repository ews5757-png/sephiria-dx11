// Copy.fx (그대로 출력)
Texture2D SrcTex : register(t0);
SamplerState Samp : register(s0);

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
    return SrcTex.Sample(Samp, i.UV);
}
