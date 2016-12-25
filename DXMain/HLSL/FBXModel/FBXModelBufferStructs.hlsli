
cbuffer cbSkinned : register(b11)
{
	// �� ĳ���ʹ� �ִ� ���� ������ 96
    matrix gmtxBoneTransforms[96] : packoffset(c0);
};

Texture2D g_tex2D_Quad : register(t10);
SamplerState g_ss_Quad : register(s10);

// ������ ���̴��� ���� �Է����� ���Ǵ� �������� �������Դϴ�.
struct VertexShaderInput
{
    float3 pos      : POSITION;
    float3 normal   : NORMAL;
    float2 texcoord : TEXCOORD0;

    uint4 bone      : BONE;
    float3 weight   : WEIGHT;
};

// �ȼ� ���̴��� ����� �ȼ��� �� �������Դϴ�.
struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float2 texcoord : TEXCOORD0;
    float2 texcoordShadow : TEXCOORD1;
};