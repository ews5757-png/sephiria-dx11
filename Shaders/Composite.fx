// Composite.fx (최종: Scene + Bloom*Intensity + HazeTone)
Texture2D SceneTex : register(t0);
Texture2D BloomTex : register(t1);
SamplerState Samp : register(s0);

cbuffer Post : register(b5)
{
    float2 TexelSize;
    float2 Direction;
    float Threshold;
    float Intensity;

    // 추가 파라미터
    float Haze; // 0~1 (먼지/안개 강도)
    float Desat; // 0~1 (탈색)
    float Contrast; // 0~2 (1=기본, 1보다 작으면 대비↓)
    float3 HazeColor;
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

float3 ApplyDustTone(float3 rgb)
{
    float luma = dot(rgb, float3(0.2126, 0.7152, 0.0722));
    rgb = lerp(rgb, luma.xxx, saturate(Desat));

    rgb = (rgb - 0.5f) * Contrast + 0.5f;

    // 회색 대신 지정 색으로 베일
    rgb = lerp(rgb, HazeColor, saturate(Haze));

    return saturate(rgb);
}

float4 PS_GreenTest(VS_OUT i) : SV_Target
{
    float4 scene = SceneTex.Sample(Samp, i.UV);
    float3 bloom = BloomTex.Sample(Samp, i.UV).rgb;

    float3 rgb = scene.rgb + bloom * Intensity;
    rgb = ApplyDustTone(rgb);

    return float4(rgb, scene.a);
}
