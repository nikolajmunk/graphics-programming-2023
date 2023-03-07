#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform vec4 AmbientColor;
uniform float AmbientReflection;
uniform float DiffuseReflection;
uniform vec3 LightColor;
uniform vec3 LightPosition;

vec4 GetColor()
{
	return texture(ColorTexture, TexCoord) * Color;
}

vec3 GetDiffuseReflection(vec3 lightDirection, vec3 worldNormal)
{
	return LightColor * DiffuseReflection * GetColor().rgb * dot(lightDirection, worldNormal);
}

vec3 GetAmbientReflection()
{
	return AmbientColor * AmbientReflection * GetColor();
}

vec3 GetBlinnPhongReflection(vec3 lightDirection, vec3 worldNormal)
{
	return GetAmbientReflection() + GetDiffuseReflection(lightDirection, worldNormal);
}

void main()
{
	vec3 dirToLight = normalize(LightPosition - WorldPosition);
	FragColor = vec4(GetBlinnPhongReflection(dirToLight, WorldNormal),1);
}
