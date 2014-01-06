
uniform sampler2D texture;

varying vec2 g_vTexCoord;

void main(void)
{
	gl_FragColor = max(texture2D(texture, g_vTexCoord) - 1.0, 0.0);
}