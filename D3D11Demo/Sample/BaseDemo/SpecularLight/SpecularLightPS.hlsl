////////////////////////////////////////////////////////////////////////////////
// Filename: color.ps
////////////////////////////////////////////////////////////////////////////////

Texture2D shaderTexture;
SamplerState SampleType: register(s0);

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float specularPower;
	float4 specularColor;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_Position;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
	float3 viewDirection : TEXCOORD1;
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


	float3 reflection;
	float4 specular;


	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);
	//textureColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	color = ambientColor;
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	//This is where the lighting equation that was discussed earlier is now implemented.The light intensity value is calculated as the dot product between the normal vector of triangle and the light direction vector.
	// Invert the light direction for calculations.
	lightDir = -lightDirection;
	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, lightDir));
	if (lightIntensity > 0.0f)
	{
		color += saturate(diffuseColor * lightIntensity);
		float3 reflection = reflect(lightDirection, input.normal);
	//	reflection = normalize(2 * lightIntensity * input.normal - lightDir);
		reflection = normalize(reflection);
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
	}
	color = color * textureColor;
	color = saturate(color + specular);
	return color;
}

float4 mainPick(PixelInputType input) : SV_TARGET
{
	float4 textureColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	return textureColor;
}
