#version 330 core

in vec4 Color;

out vec4 FragColor;

// (todo) 02.5: Add Color input variable here


void main()
{
	// (todo) 02.3: Compute alpha using the built-in variable gl_PointCoord
	float intensity = 1-length((gl_PointCoord*2)-1);
//	intensity = step(0.5f, intensity);
	FragColor = vec4(Color.rgb, intensity);//vec4(1, 1, 1, intensity);
}
