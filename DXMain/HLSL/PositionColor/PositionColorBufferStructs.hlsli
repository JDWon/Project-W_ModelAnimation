// ������ ���̴��� ���� �Է����� ���Ǵ� �������� �������Դϴ�.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};

// �ȼ� ���̴��� ����� �ȼ��� �� �������Դϴ�.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};