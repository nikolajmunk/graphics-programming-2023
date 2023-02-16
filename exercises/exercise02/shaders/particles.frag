#version 330 core

in vec4 Color;
in float Age;
in float Duration;

out vec4 FragColor;

// (todo) 02.5: Add Color input variable here

// signed distance to a n-star polygon with external angle en
float sdStar(in vec2 p, in float r, in int n, in float m) // m=[2,n]
{
    // these 4 lines can be precomputed for a given shape
    float an = 3.141593/float(n);
    float en = 3.141593/m;
    vec2  acs = vec2(cos(an),sin(an));
    vec2  ecs = vec2(cos(en),sin(en)); // ecs=vec2(0,1) and simplify, for regular polygon,

    // reduce to first sector
    float bn = mod(atan(p.x,p.y),2.0*an) - an;
    p = length(p)*vec2(cos(bn),abs(sin(bn)));

    // line sdf
    p -= r*acs;
    p += ecs*clamp( -dot(p,ecs), 0.0, r*acs.y/ecs.y);
    return length(p)*sign(p.x);
}

void main()
{
	// (todo) 02.3: Compute alpha using the built-in variable gl_PointCoord
	vec2 posNormalized = (gl_PointCoord*2)-1;
	float dist = length(posNormalized);
	dist = sdStar(posNormalized, 1, 5, 3);
	float intensity;
	intensity = mix(0, 1, 1.0-smoothstep(0.0,0.025,abs(dist)) ); // clean outline
//	intensity = (1-dist) * step(0.1, mod(dist, 0.25)); // concentric rings
	//intensity = (dist < 0.0 ? 1 : 0) * step(0.1, (sin(dist)-0.5)*2); // concentric rings
	//intensity = (1-dist) * sin(dist*2); // concentric rings
//	intensity *= mix(1, 0, clamp(Age/Duration, 0, 1));
//	intensity = step(0.5f, intensity);
	FragColor = vec4(Color.rgb, intensity);//vec4(1, 1, 1, intensity);
}
