#include "..\BindingBuffer.hlsli"
#include "FBXModelBufferStructs.hlsli"

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float4 pos = float4(input.pos, 1.0f);
    float3 normal = input.normal;
    
	// �迭�� �ʱ�ȭ�Ѵ�. �׷��� ������ SV_POSITION�� ������ ��� �޽����� ���´�.
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.weight.x;
    weights[1] = input.weight.y;
    weights[2] = input.weight.z;
    weights[3] = 1.0f - (weights[0] + weights[1] + weights[2]); //input.weights.w;
    
	// ���� ���� ����
    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; ++i)
    {
        posL +=         weights[i] * mul(pos, gmtxBoneTransforms[input.bone[i]]).xyz;
        normalL +=      weights[i] * mul(input.normal, (float3x3) gmtxBoneTransforms[input.bone[i]]);
    }
    
	// ������ ��ġ�� �������ǵ� �������� ��ȯ�մϴ�.
    pos = mul(float4(posL, 1.f), model);
	//pos = mul(pos, model);
	
	// ���� �������� ��ȯ�Ѵ�.
    output.positionW = pos.xyz;

    output.normalW = mul(normalL, (float3x3) model);
    
    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.position = pos;
    output.texcoord = input.texcoord;

    return output;
}