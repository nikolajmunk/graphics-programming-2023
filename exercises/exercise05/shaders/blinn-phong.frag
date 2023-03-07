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
uniform float SpecularReflection;
uniform float SpecularExponent;
uniform vec3 LightColor;
uniform vec3 LightPosition;
uniform vec3 CameraPosition;

vec4 GetColor()
{
	return texture(ColorTexture, TexCoord) * Color;
}

vec3 GetSpecularReflection(vec3 lightDirection, vec3 viewDirection, vec3 worldNormal)
{
	vec3 halfDir = normalize(lightDirection + viewDirection);
	return LightColor * SpecularReflection * pow(max(dot(worldNormal, halfDir),0), SpecularExponent);
}

vec3 GetDiffuseReflection(vec3 lightDirection, vec3 worldNormal)
{
	return LightColor * DiffuseReflection * GetColor().rgb * max(dot(lightDirection, worldNormal), 0);
}

vec3 GetAmbientReflection()
{
	return AmbientColor * AmbientReflection * GetColor();
}

vec3 GetBlinnPhongReflection(vec3 lightDirection, vec3 viewDirection, vec3 worldNormal)
{
	return GetAmbientReflection() + GetDiffuseReflection(lightDirection, worldNormal) + GetSpecularReflection(lightDirection, viewDirection, worldNormal);
}

void main()
{
	vec3 dirToLight = normalize(LightPosition - WorldPosition);
	vec3 dirToCam = normalize(CameraPosition - WorldPosition);
	FragColor = vec4(GetBlinnPhongReflection(dirToLight, dirToCam, WorldNormal),1);
}
