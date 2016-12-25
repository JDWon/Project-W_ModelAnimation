#include "..\BindingBuffer.hlsli"
#include "FBXModelBufferStructs.hlsli"

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float4 pos = float4(input.pos, 1.0f);
    float3 normal = input.normal;
    
	// 배열을 초기화한다. 그렇지 않으면 SV_POSITION에 관련한 경고 메시지가 나온다.
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.weight.x;
    weights[1] = input.weight.y;
    weights[2] = input.weight.z;
    weights[3] = 1.0f - (weights[0] + weights[1] + weights[2]); //input.weights.w;
    
	// 정점 블랜딩 수행
    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; ++i)
    {
        posL +=         weights[i] * mul(pos, gmtxBoneTransforms[input.bone[i]]).xyz;
        normalL +=      weights[i] * mul(input.normal, (float3x3) gmtxBoneTransforms[input.bone[i]]);
    }
    
	// 꼭짓점 위치를 프로젝션된 공간으로 변환합니다.
    pos = mul(float4(posL, 1.f), model);
	//pos = mul(pos, model);
	
	// 월드 공간으로 변환한다.
    output.positionW = pos.xyz;

    output.normalW = mul(normalL, (float3x3) model);
    
    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.position = pos;
    output.texcoord = input.texcoord;

    return output;
}