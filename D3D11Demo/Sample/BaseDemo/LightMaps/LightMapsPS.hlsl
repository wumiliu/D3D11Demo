/////////////
// GLOBALS //
/////////////
Texture2D shaderTextures[2];
SamplerState SampleType;


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
	float4 color;
	float4 lightColor;
	float4 finalColor;

	// Get the pixel color from the color texture.
	color = shaderTextures[0].Sample(SampleType, input.tex);
	lightColor = shaderTextures[1].Sample(SampleType, input.tex);
	// Blend the two pixels together.
	finalColor = color * lightColor;

	return finalColor;
}