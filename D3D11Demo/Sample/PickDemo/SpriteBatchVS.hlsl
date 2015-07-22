matrix  gWorldViewProj;
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
	
};

struct GeoOut
{
	float4 PosH  : SV_Position;
	float4 color : COLOR;
};


PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	input.position.w = 1.0f;
	output.position = mul(input.position, gWorldViewProj);
	//output.position = input.position;// mul(input.position, gWorldViewProj);
	output.color = input.color;
	return output;
}
PixelInputType mainEx(VertexInputType input)
{
	PixelInputType output;
	input.position.w = 1.0f;
	output.position = mul(input.position, gWorldViewProj);
	//output.position = input.position;// mul(input.position, gWorldViewProj);
	output.color = input.color;
	return output;
}
// The draw GS just expands points into lines.
[maxvertexcount(100)]
void DrawGS(line  PixelInputType gin[2],
	inout LineStream<GeoOut> output)
{
	PixelInputType ginT[2];
	ginT[0].position = gin[0].position;
	ginT[1].position = gin[1].position;
	float width = (gin[1].position.x - gin[0].position.x);
	float height = (gin[1].position.y - gin[0].position.y);

	GeoOut V0;
	int ntmp = 0;

	for (int i = 0; i < 100; ++i)
	{
		V0.PosH = (gin[0].position);
		V0.PosH.x += i* width / 100.0f;
		V0.PosH.y += i* height / 100.0f;
		V0.PosH = mul(V0.PosH, gWorldViewProj);
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
