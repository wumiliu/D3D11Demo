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


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float4 tex : TEXCOORD;
};

struct PixelInputType
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 tex : TEXCOORD;
};

struct GeoOut
{
	float4 position : SV_Position;
	float4 color : COLOR;
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

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the input color for the pixel shader to use.
	output.color = input.color;
	output.tex = input.tex;
	return output;
}

// The draw GS just expands points into lines.
[maxvertexcount(100)]
void DrawGS(line  PixelInputType gin[2],
	inout LineStream<GeoOut> output)
{
	float width = (gin[1].position.x - gin[0].position.x);
	float height = (gin[1].position.y - gin[0].position.y);

	GeoOut V0;
	int ntmp = 0;

	for (int i = 0; i < 100; ++i)
	{
		V0.position = (gin[0].position);
		V0.position.x += i* width / 100.0f;
		V0.position.y += i* height / 100.0f;
		if (i % 3 == 0)
		{
			ntmp++;
		}
		if (ntmp % 2 == 0)
		{
			V0.color = float4(0, 0, 0, 0);
		}
		else
		{
			V0.color = gin[0].color;
		}

		output.Append(V0);
	}
}