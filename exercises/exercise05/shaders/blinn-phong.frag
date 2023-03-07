#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform vec4 AmbientColor;
uniform float AmbientReflection;

vec3 GetAmbientReflection()
{
	return AmbientColor * AmbientReflection * texture(ColorTexture, TexCoord) * Color;
}

vec3 GetBlinnPhongReflection()
{
	return GetAmbientReflection();
}

void main()
{
	FragColor = vec4(GetBlinnPhongReflection(),1);
}
