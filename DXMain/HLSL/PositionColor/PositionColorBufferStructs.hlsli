// 꼭짓점 셰이더에 대한 입력으로 사용되는 꼭짓점별 데이터입니다.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};

// 픽셀 셰이더를 통과한 픽셀당 색 데이터입니다.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};