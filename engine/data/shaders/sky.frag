
//uniform sampler2D texture;
uniform vec3 color1;
uniform vec3 color2;

varying float colorMix;

void main(void)
{
	vec3 color = mix(color1, color2, colorMix);
	
	gl_FragColor = vec4(color, 1.0f);
}