#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

// (todo) 03.X: Add the out variables here
out vec3 Normal;
out vec3 Position;

// (todo) 03.X: Add the uniforms here
uniform mat4 WorldMatrix = mat4(1.0f);
uniform mat4 ViewProjMatrix = mat4(1.0f);

void main()
{
	Position = vec3(ViewProjMatrix * WorldMatrix * vec4(VertexPosition, 1.0f));
	vec4 normalTransformed = WorldMatrix * vec4(VertexNormal, 0.0f);
	Normal = vec3(normalTransformed);
	gl_Position = ViewProjMatrix * WorldMatrix * vec4(VertexPosition, 1.0);
}
