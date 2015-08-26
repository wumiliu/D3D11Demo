////////////////////////////////////////////////////////////////////////////////
// Filename: color.vs
////////////////////////////////////////////////////////////////////////////////

//#include "LightHelper.hlsl"
/////////////
// GLOBALS //
/////////////
cbuffer  MatrixBuffer: register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};



//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float4 position : SV_Position;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_Position;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType main(VertexInputType input)
{
	PixelInputType output;


	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;
	// Calculate the position of the vertex against the world, view, and projection matrices.
	
	//output.position -= MyColor1;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);



	// Store the input color for the pixel shader to use.
	output.tex = input.tex;
	output.normal = mul(float4(input.normal, 0.0f), worldMatrix).xyz;
	output.normal = normalize(output.normal);
	return output;
}