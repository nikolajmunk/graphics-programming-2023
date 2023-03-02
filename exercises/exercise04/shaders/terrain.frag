#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform sampler2D DirtTexture;
uniform sampler2D StoneTexture;
uniform sampler2D SnowTexture;
uniform vec2 ColorTextureScale;

uniform vec2 DirtRange;
uniform vec2 GrassRange;
uniform vec2 StoneRange;

float rangeToNormalize(vec2 range, float height)
{
	return clamp((height - range.x) / (range.y - range.x), 0, 1);
}

void main()
{

	vec4 grassTextureSample = texture(ColorTexture, TexCoord * ColorTextureScale);
	vec4 dirtTextureSample = texture(DirtTexture, TexCoord * ColorTextureScale);
	vec4 stoneTextureSample = texture(StoneTexture, TexCoord * ColorTextureScale);
	vec4 snowTextureSample = texture(SnowTexture, TexCoord * ColorTextureScale);
	vec4 col = mix(dirtTextureSample, grassTextureSample, rangeToNormalize(DirtRange, WorldPosition.y));
	col = mix(col, stoneTextureSample, rangeToNormalize(GrassRange, WorldPosition.y));
	col = mix(col, snowTextureSample, rangeToNormalize(StoneRange, WorldPosition.y));
	FragColor = Color * col;
//	FragColor = Color * vec4(WorldPosition.y);

}
