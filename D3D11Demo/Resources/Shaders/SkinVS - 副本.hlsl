////////////////////////////////////////////////////////////////////////////////
// Filename: color.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer cbSkinned
{
	matrix gBoneTransforms[58];
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float3 position : SV_Position;
	float2 tex : TEXCOORD;
	float4 Weights    : WEIGHTS;
	uint4 BoneIndices : BONEINDICES;
};

struct PixelInputType
{
    float4 position : SV_Position;
    float2 tex : TEXCOORD;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = input.Weights.x;
	weights[1] = input.Weights.y;
	weights[2] = input.Weights.z;
	weights[3] = input.Weights.w;
	float3 posLTmp  = mul(float4(input.position, 1.0f), worldMatrix);
	posLTmp = input.position;
	// Change the position vector to be 4 units for proper matrix calculations.
	float3 posL = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		int bone = input.BoneIndices[i];
		matrix mm =   gBoneTransforms[bone];
		posL += weights[i] * mul(float4(posLTmp, 1.0f), mm).xyz;
	}

	output.position = mul(float4(posL, 1.0f), projectionMatrix);
	//output.position = mul(float4(input.position, 1.0f), projectionMatrix);
	// Store the input color for the pixel shader to use.
	//output.position = mul(input.position, projectionMatrix);
	output.tex.x = input.tex.x;
	output.tex.y = 1.0f - input.tex.y;
	return output;
}