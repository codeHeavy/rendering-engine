

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 sampleDir	: TEXCOORD;
};


// Texture-related variables
TextureCube SkyTexture		: register(t0);
SamplerState BasicSampler	: register(s0);


// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	// Gamma correction
	float4 skyColor = SkyTexture.Sample(BasicSampler, input.sampleDir);
	skyColor.rgb = lerp(skyColor.rgb, pow(skyColor.rgb, 2.2), 1);
	float3 gammaCorrectValue = lerp(skyColor, pow(skyColor, 1.0f / 2.2f), 1);
	return float4(gammaCorrectValue,1);
}