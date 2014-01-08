cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer
{
	float3 camPosition;
	float padding; // This structure should be byte aligned to 16 (12+4 = 16)
}

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

PixelInputType LightVertexShader(VertexInputType i)
{
	PixelInputType o;
	float4 worldPosition;

	i.position.w = 1.0f;

	o.position = mul(i.position, worldMatrix);
	o.position = mul(o.position, viewMatrix);
	o.position = mul(o.position, projectionMatrix);

	o.tex = i.tex;

	// Calculate the normal vector against the world matrix over
	o.normal = mul(i.normal, (float3x3)worldMatrix);

	// Normalize the normal vector
	o.normal = normalize(o.normal);

	worldPosition = mul(i.position, worldMatrix);
	o.viewDirection = camPosition.xyz - worldPosition.xyz;
	o.viewDirection = normalize(o.viewDirection);

	return o;
}