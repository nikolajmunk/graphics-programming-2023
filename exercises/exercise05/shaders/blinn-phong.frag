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
uniform vec4 LightColor; // 4th component is intensity
uniform vec3 LightPosition;
uniform vec3 CameraPosition;
uniform vec4 SunColor; // 4th component is intensity
uniform vec3 SunDirection;

vec4 GetColor()
{
	return texture(ColorTexture, TexCoord) * Color;
}

vec3 GetSpecularReflection(vec3 lightColor, vec3 lightDirection, vec3 viewDirection, vec3 worldNormal)
{
	vec3 halfDir = normalize(lightDirection + viewDirection);
	return lightColor * SpecularReflection * pow(max(dot(worldNormal, halfDir),0), SpecularExponent);
}

vec3 GetDiffuseReflection(vec4 color, vec3 lightColor, vec3 lightDirection, vec3 worldNormal)
{
	return lightColor * DiffuseReflection * color.rgb * max(dot(lightDirection, worldNormal), 0);
}

vec3 GetAmbientReflection(vec4 color)
{
	return AmbientColor * AmbientReflection * color;
}

vec3 GetBlinnPhongReflection(vec4 color, vec3 lightColor, vec3 lightDirection, vec3 viewDirection, vec3 worldNormal)
{
	return GetAmbientReflection(color) + GetDiffuseReflection(color, lightColor, lightDirection, worldNormal) + GetSpecularReflection(lightColor, lightDirection, viewDirection, worldNormal);
}

void main()
{
	vec3 dirToLight = LightPosition - WorldPosition;
	vec3 dirToCam = normalize(CameraPosition - WorldPosition);
	float intensity = LightColor.a / (length(dirToLight) * 0.1f);
	vec3 lightColor = LightColor.rgb * intensity;
	vec3 sunColor = SunColor.rgb * SunColor.a;
	vec4 albedoColor = GetColor();
	vec3 col = GetBlinnPhongReflection(albedoColor, lightColor, normalize(dirToLight), dirToCam, WorldNormal);
	col += GetBlinnPhongReflection(albedoColor, sunColor, normalize(SunDirection), dirToCam, WorldNormal);
	FragColor = vec4(col, 1);
}
