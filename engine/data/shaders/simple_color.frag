
uniform sampler2D texture;
uniform vec3 color;

varying vec2 g_vTexCoord;

void main(void)
{
	vec3 final = texture2D(texture, g_vTexCoord);
	final = pow(final, 1.5);
	final = final * color;
	gl_FragColor = vec4(final, 1.0f);
}