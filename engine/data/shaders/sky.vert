
varying float colorMix;


void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	colorMix = clamp( (gl_Vertex.y - 100.0) * .002, 0.0, 1.0);
		
	//g_vTexCoord = vec2(gl_MultiTexCoord0);
}