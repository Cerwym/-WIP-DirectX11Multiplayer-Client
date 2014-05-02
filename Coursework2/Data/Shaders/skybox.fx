cbuffer cbPerObject
{
	float4x4 WVP;
	float4x4 World;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;
TextureCube SkyMap;

struct VS_OUTPUT
{
	float4 Position: SV_POSITION;
	float2 TextureC: TEXCOORD;
	float3 Normals: NORMAL;
};

struct SKYMAP_VS_OUTPUT	//output structure for skymap vertex shader
{
	float4 Position : SV_POSITION;
	float3 TextureC : TEXCOORD;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
	VS_OUTPUT output;

	output.Position = mul(inPos, WVP);

	output.Normals = mul(normal, World);

	output.TextureC = inTexCoord;

	return output;
}

SKYMAP_VS_OUTPUT SKYMAP_VS(float3 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
	SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT)0;

	//Set Pos to xyww instead of xyzw, so that z will always be 1 (furthest from camera)
	output.Position = mul(float4(inPos, 1.0f), WVP).xyww;

	output.TextureC = inPos;

	return output;
}

float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_Target
{
	return SkyMap.Sample(ObjSamplerState, input.TextureC);
}
