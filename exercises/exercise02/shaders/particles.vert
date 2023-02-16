#version 330 core

layout (location = 0) in vec2 ParticlePosition;
layout (location = 1) in vec4 ParticleColor;
layout (location = 2) in float ParticleSize;
layout (location = 3) in float Birth;
layout (location = 4) in float Duration;
// (todo) 02.X: Add more vertex attributes


// (todo) 02.5: Add Color output variable here
out vec4 Color;

// (todo) 02.X: Add uniforms
uniform float CurrentTime;

void main()
{
	Color = ParticleColor;
	float age = CurrentTime - Birth;
	gl_PointSize = ParticleSize;// * 1-(step(Duration, age));
	if (age > Duration) {
		gl_PointSize = 0;
	}
	gl_Position = vec4(ParticlePosition, 0.0, 1.0);
}
