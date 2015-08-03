/////////////
// GLOBALS //
/////////////
cbuffer  MatrixBuffer: register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//我们添加一个新的常数缓冲区来控制反射矩阵。

cbuffer ReflectionBuffer
{
	matrix reflectionMatrix;
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
	float2 tex : TEXCOORD;
	float4 reflectionPosition : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	matrix reflectProjectWorld;

	input.position.w = 1.0f;
	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
	//第一个改变顶点着色器是我们创建一个矩阵的输入位置值转换到投影反射的位置。 这个矩阵反映的组合矩阵、投影矩阵,矩阵的世界。
	reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
	reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

	//现在将输入的位置转换为投影反射的位置。 将使用这些转换位置坐标的像素着色器获得地图投影反射结构。

		// Calculate the input position against the reflectProjectWorld matrix.
	output.reflectionPosition = mul(input.position, reflectProjectWorld);
	return output;
}