//--------------------------------------------------------------------------------------
// Global
//--------------------------------------------------------------------------------------
cbuffer ConstantBufferMatrix
{
	matrix World;
	matrix View;
	matrix Projection;
}

//--------------------------------------------------------------------------------------
// Struct
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Color : COLOR;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;
	Output.Pos = mul(Input.Pos, World);
	Output.Pos = mul(Output.Pos, View);
	Output.Pos = mul(Output.Pos, Projection);
	Output.Color = Input.Color;

	return Output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT Input) : SV_Target
{
	return Input.Color;
}

technique11 Technique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
