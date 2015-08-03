/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

//我们添加一个新的纹理变量反映纹理渲染的场景。
Texture2D reflectionTexture;

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_Position;
	float2 tex : TEXCOORD;
	float4 reflectionPosition : TEXCOORD1;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float2 reflectTexCoord;
	float4 reflectionColor;
	float4 color;


	// Sample the texture pixel at this location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);
	//输入反射同质坐标位置需要转换为适当的纹理坐标。 这样做首先除以W协调。 这让我们与tu和电视坐标1 + 1范围, 修复它映射到一个0到 + 1除以2, 添加0.5范围。
	// Calculate the projected reflection texture coordinates.
	reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
	reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;


	// Sample the texture pixel from the reflection texture using the projected texture coordinates.
	reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);

	//最后我们结合反映纹理的纹理从地板上创建的影响反映多维数据集在地板上。 这里我们使用一个线性插值两个纹理之间的0.15倍。 你可以改变这一个不同的混合方程或改变金额的因素, 不同的或更强的效果。
	// Do a linear interpolation between the two textures for a blend effect.
	color = lerp(textureColor, reflectionColor, 0.15f);

	return color;
}