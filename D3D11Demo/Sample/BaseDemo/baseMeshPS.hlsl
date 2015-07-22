////////////////////////////////////////////////////////////////////////////////
// Filename: color.ps
////////////////////////////////////////////////////////////////////////////////

Texture2D shaderTexture;
SamplerState SampleType: register(s0);

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_Position;
	float2 tex : TEXCOORD;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
#if (EXAMPLE_DEFINE_1 == 0)
	float4 textureColor = { 1.0f, 1.0f, 1.0f, 1.0f };
#else
	float4 textureColor = { 0.0f, 0.0f, 1.0f, 1.0f };
	textureColor = shaderTexture.Sample(SampleType, input.tex);
#endif
	return textureColor;
}

float4 mainPick(PixelInputType input) : SV_TARGET
{
	float4 textureColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	return textureColor;
}

float4 mainRed(PixelInputType input) : SV_TARGET
{
	float4 textureColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	return textureColor;
}


float4 mainGreen(PixelInputType input) : SV_TARGET
{
	float4 textureColor = { 0.0f, 1.0f, 0.0f, 1.0f };
	return textureColor;
}


float4 mainBlue(PixelInputType input) : SV_TARGET
{
	float4 textureColor = { 0.0f, 0.0f, 1.0f, 1.0f };
	return textureColor;
}