
varying vec2 g_vTexCoord;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	g_vTexCoord = vec2(gl_MultiTexCoord0);
}