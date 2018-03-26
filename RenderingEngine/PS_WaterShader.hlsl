
#define MAX_LIGHTS 2
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
	float3 worldPos		: POSITION;
	float3 tangent		: TANGENT;
};

struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};

struct PointLight
{
	float4 Color;
	float3 Position;
};

cbuffer externalData : register(b0)
{
	DirectionalLight dirLights[MAX_LIGHTS];
	PointLight pointLights[MAX_LIGHTS];
	int DirectionalLightCount;
	int PointLightCount;
	float3 cameraPosition;
	float translate;
}

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
SamplerState basicSampler : register(s0);
//
//float4 calculateDirectionalLight(float3 normal, DirectionalLight light)
//{
//	float3 dirToLight = normalize(-light.Direction);
//	float NdotL = dot(normal, dirToLight);
//	NdotL = saturate(NdotL);
//	return light.DiffuseColor * NdotL + light.AmbientColor;
//}
//
//float calculateSpecular(float3 normal, float3 worldPos, float3 dirToPointLight, float3 camPos)
//{
//	float3 dirToCamera = normalize(camPos - worldPos);
//	float3 refl = reflect(-dirToPointLight, normal);
//	float specExp = 64;
//	float spec = pow(saturate(dot(dirToCamera, refl)), specExp);
//	return spec;
//}
//
//float4 calculatePointLight(float3 normal, float3 worldPos, PointLight light)
//{
//	float3 dirToPointLight = normalize(light.Position - worldPos);
//	float pointNdotL = dot(normal, dirToPointLight);
//	pointNdotL = saturate(pointNdotL);
//	float spec = calculateSpecular(normal, worldPos, dirToPointLight, cameraPosition);
//	return spec + light.Color * pointNdotL;
//}
//
//float3 calculateNormalFromMap(float2 uv, float3 normal, float3 tangent)
//{
//	float3 normalFromTexture = normalTexture.Sample(basicSampler, uv).xyz;
//	float3 unpackedNormal = normalFromTexture * 2.0f - 1.0f;
//	float3 N = normal;
//	float3 T = normalize(tangent - N * dot(tangent, N));
//	float3 B = cross(N, T);
//	float3x3 TBN = float3x3(T, B, N);
//	return normalize(mul(unpackedNormal, TBN));
//	return unpackedNormal;
//}

Texture2D roughnessTexture : register(t2);
// Range-based attenuation function
float Attenuate(float3 lightPosition, float lightRange, float3 worldPos)
{
	float dist = distance(lightPosition, worldPos);

	// Ranged-based attenuation
	float att = saturate(1.0f - (dist * dist / (lightRange * lightRange)));

	// Soft falloff
	return att * att;
}
float calculateSpecular(float3 normal, float3 worldPos, float3 dirToLight, float3 camPos)
{
	float3 dirToCamera = normalize(camPos - worldPos);
	float3 halfwayVector = normalize(dirToLight + dirToCamera);
	//float3 refl = reflect(-dirToLight, normal);
	float shininess = 64;
	//float spec = pow(saturate(dot(dirToCamera, refl)), shininess);
	return shininess == 0 ? 0.0f : pow(max(dot(halfwayVector, normal), 0), shininess);
	//return spec;
}

float4 calculateDirectionalLight(float3 normal, float3 worldPos, DirectionalLight light, float roughness)
{
	float3 dirToLight = normalize(-light.Direction);
	float NdotL = dot(normal, dirToLight);
	NdotL = saturate(NdotL);
	float spec = calculateSpecular(normal, worldPos, dirToLight, cameraPosition) * roughness;
	return spec + light.DiffuseColor * NdotL + light.AmbientColor;
}

float4 calculatePointLight(float3 normal, float3 worldPos, PointLight light, float roughness)
{
	float3 dirToPointLight = normalize(light.Position - worldPos);
	float pointNdotL = dot(normal, dirToPointLight);
	pointNdotL = saturate(pointNdotL);
	float spec = calculateSpecular(normal, worldPos, dirToPointLight, cameraPosition) * roughness;
	return spec + light.Color * pointNdotL;
}

float3 calculateNormalFromMap(float2 uv, float3 normal, float3 tangent)
{
	float3 normalFromTexture = normalTexture.Sample(basicSampler, uv).xyz;
	float3 unpackedNormal = normalFromTexture * 2.0f - 1.0f;
	float3 N = normal;
	float3 T = normalize(tangent - N * dot(tangent, N));
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);
	return normalize(mul(unpackedNormal, TBN));
}
float4 main(VertexToPixel input) : SV_TARGET
{
	////........................................................................
	////Translate the normal map
	//input.uv.x += translate;

	//input.normal = normalize(input.normal);
	//float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);
	////return surfaceColor;
	//float3 finalNormal = calculateNormalFromMap(input.uv, input.normal, input.tangent);
	//finalNormal = normalize(finalNormal);
	//
	//float4 dirLight = calculateDirectionalLight(finalNormal, light) * surfaceColor;
	//float4 secDirLight = calculateDirectionalLight(finalNormal, secondaryLight) * surfaceColor;
	//float4 pLight = calculatePointLight(finalNormal, input.worldPos, pointLight)  * surfaceColor;
	//float3 pLightDir = normalize(pointLight.Position - input.worldPos);
	//float spec = calculateSpecular(finalNormal, input.worldPos, pLightDir, cameraPosition);

	//return dirLight;//+ secDirLight + spec.xxxx;
	////.........................................................................
	
	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);
	float3 finalNormal = calculateNormalFromMap(input.uv, input.normal, input.tangent);
	input.normal = normalize(input.normal);
	finalNormal = normalize(finalNormal);
	float4 totalColor = float4(0, 0, 0, 0);
	float roughness = roughnessTexture.Sample(basicSampler, input.uv).r;

	int i = 0;
	for (i = 0; i < DirectionalLightCount; ++i)
	{
		totalColor += calculateDirectionalLight(finalNormal, input.worldPos, dirLights[i], roughness) * surfaceColor;
	}

	for (i = 0; i < PointLightCount; ++i)
	{
		totalColor += calculatePointLight(finalNormal, input.worldPos, pointLights[i], roughness)  * surfaceColor;
	}

	/*float4 dirLight = calculateDirectionalLight(finalNormal, dirLights[0]) * surfaceColor;
	float4 secDirLight = calculateDirectionalLight(finalNormal, dirLights[1]) * surfaceColor;
	float4 pLight = calculatePointLight(finalNormal, input.worldPos, pointLights[0])  * surfaceColor;*/
	//return dirLight + secDirLight + pLight;
	return totalColor;
}