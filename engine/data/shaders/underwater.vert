
uniform mat4 g_mModelViewProjection;
uniform float g_waterTranslation;
uniform vec3 g_vEyePos;

varying vec3 g_viewVector;
varying vec2 g_vTexCoord1;
varying vec2 g_vTexCoord2;

varying vec2 g_vTexCoord;


void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	g_vTexCoord1 = g_vTexCoord2 = vec2(gl_MultiTexCoord0);
	
	g_vTexCoord = vec2(gl_MultiTexCoord0);
	
	//g_vTexCoord1.x += g_waterTranslation * 0.5;
	//g_vTexCoord1.y += g_waterTranslation * 1.0;
	
	//g_vTexCoord2.x -= g_waterTranslation * 1.0;
	//g_vTexCoord2.y -= g_waterTranslation * 0.5;
	
	//g_vTexCoord1 *= vec2(0.3);
	//g_vTexCoord2 *= vec2(0.3);
}