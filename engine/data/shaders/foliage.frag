
uniform sampler2D texture;

varying vec2 vTexCoord;
varying float vAlphaMultiplier;

void main(void)
{
	vec4 color = texture2D(texture, vTexCoord);
	
	//color[3] = color[3] * vAlphaMultiplier;
	
	gl_FragColor =  color;
}