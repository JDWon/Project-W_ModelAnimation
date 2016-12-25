
cbuffer cbSkinned : register(b11)
{
	// 한 캐릭터당 최대 뼈대 개수는 96
    matrix gmtxBoneTransforms[96] : packoffset(c0);
};

Texture2D g_tex2D_Quad : register(t10);
SamplerState g_ss_Quad : register(s10);

// 꼭짓점 셰이더에 대한 입력으로 사용되는 꼭짓점별 데이터입니다.
struct VertexShaderInput
{
    float3 pos      : POSITION;
    float3 normal   : NORMAL;
    float2 texcoord : TEXCOORD0;

    uint4 bone      : BONE;
    float3 weight   : WEIGHT;
};

// 픽셀 셰이더를 통과한 픽셀당 색 데이터입니다.
struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float2 texcoord : TEXCOORD0;
    float2 texcoordShadow : TEXCOORD1;
};