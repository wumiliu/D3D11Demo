////////////////////////////////////////////////////////////////////////////////
// Filename: color.ps
////////////////////////////////////////////////////////////////////////////////

TextureCube g_cubeMap;
SamplerState SampleType: register(s0);

Texture2D shaderTexture;
//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_Position;
	float3 posL: POSITION;
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
	input.posL = normalize(input.posL);
	textureColor = g_cubeMap.Sample(SampleType, input.posL);
#endif
	return textureColor;
}

float4 mainTex(PixelInputType input) : SV_TARGET
{
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	float4 textureColor;
	textureColor = shaderTexture.Sample(SampleType, input.tex);
	return textureColor;
}