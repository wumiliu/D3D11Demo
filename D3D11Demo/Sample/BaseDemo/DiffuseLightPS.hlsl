////////////////////////////////////////////////////////////////////////////////
// Filename: color.ps
////////////////////////////////////////////////////////////////////////////////

Texture2D shaderTexture;
SamplerState SampleType: register(s0);
float4 diffuseColor;
cbuffer LightBuffer
{
	float3 lightDirection;
	float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_Position;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	//This is where the lighting equation that was discussed earlier is now implemented.The light intensity value is calculated as the dot product between the normal vector of triangle and the light direction vector.
	// Invert the light direction for calculations.
	lightDir = -lightDirection;
	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, lightDir));
	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	color = saturate(diffuseColor * lightIntensity);

	// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	color = color * textureColor;
	return color;
}

float4 mainPick(PixelInputType input) : SV_TARGET
{
	float4 textureColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	return textureColor;
}
