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

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
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
	float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	float4 worldPosition;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;
	// Calculate the position of the vertex against the world, view, and projection matrices.


	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);



	// Store the input color for the pixel shader to use.
	output.tex = input.tex;
	output.normal = mul(float4(input.normal, 0.0f), worldMatrix).xyz;
	output.normal = normalize(output.normal);

	//The viewing direction is calculated here in the vertex shader. We calculate the world position of the vertex and subtract that from the camera position to determine where we are viewing the scene from. The final value is normalized and sent into the pixel shader.
	worldPosition = mul(input.position, worldMatrix);
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	output.viewDirection = normalize(output.viewDirection);

	return output;
}