cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelInputType TextureVertexShader(VertexInputType i)
{
	PixelInputType o;

	// Change the position vector to be 4 units fro proper matrix calculations.
	i.position.w = 1.0f;

	o.position = mul(i.position, worldMatrix);
	o.position = mul(o.position, viewMatrix);
	o.position = mul(o.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	o.tex = i.tex;

	return o;
}
