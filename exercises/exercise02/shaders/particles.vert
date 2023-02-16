#version 330 core

layout (location = 0) in vec2 ParticlePosition;
layout (location = 1) in vec4 ParticleColor;
layout (location = 2) in float ParticleSize;
layout (location = 3) in float Birth;
layout (location = 4) in float ParticleDuration;
layout (location = 5) in vec2 Velocity;
// (todo) 02.X: Add more vertex attributes


// (todo) 02.5: Add Color output variable here
out vec4 Color;
out float Age;
out float Duration;

// (todo) 02.X: Add uniforms
uniform float CurrentTime;
uniform vec2 Gravity;

void main()
{
	Color = ParticleColor;
	Age = CurrentTime - Birth;
	Duration = ParticleDuration;
	gl_PointSize = mix(ParticleSize, 0, clamp(Age/Duration, 0, 1));// * 1-(step(ParticleDuration, age));

	vec2 pos = ParticlePosition + (Velocity * Age) + (0.5f * (Gravity * mix(1.5f, 3, clamp(gl_PointSize / 100, 0.01f, 1))) * Age*Age);
//	vec2 pos = ParticlePosition;//

	gl_Position = vec4(pos.xy, 0.0, 1.0);
}
