#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

// (todo) 03.X: Add the out variables here


// (todo) 03.X: Add the uniforms here
uniform mat4 WorldMatrix = mat4(1.0f);
uniform mat4 ViewProjMatrix = mat4(1.0f);

void main()
{
	gl_Position = ViewProjMatrix * WorldMatrix * vec4(VertexPosition, 1.0);
}
