
uniform vec3 g_vLightPosObjSpc;
uniform vec3 g_vEyePosObjSpc;
uniform float g_waterTranslation;

varying vec3 g_lightVector;
varying vec3 g_viewVector;

varying vec2 g_vTexCoord1;
varying vec2 g_vTexCoord2;
varying float g_vFogValue;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	// Calcular o vector da luz em tangent space
	vec3 tmpLightVector = g_vLightPosObjSpc - gl_Vertex.xyz; 
	g_lightVector.x = dot(tmpLightVector, vec3(1.0, 0.0, 0.0));
	g_lightVector.y = dot(tmpLightVector, vec3(0.0, 0.0, -1.0));
	g_lightVector.z = dot(tmpLightVector, vec3(0.0, 1.0, 0.0));

	// Calcular o view vector em tangent space
	vec3 tmpViewVector = g_vEyePosObjSpc.xyz - gl_Vertex.xyz;
	g_viewVector.x = dot(tmpViewVector, vec3(1.0, 0.0, 0.0));
	g_viewVector.y = dot(tmpViewVector, vec3(0.0, 0.0, -1.0));
	g_viewVector.z = dot(tmpViewVector, vec3(0.0, 1.0, 0.0));
	
	g_vTexCoord1 = g_vTexCoord2 = vec2(gl_MultiTexCoord0);
		
	g_vTexCoord1.x += g_waterTranslation * 1.0;
	
	g_vTexCoord2.x -= g_waterTranslation * 1.0;
	g_vTexCoord2.y += 0.5;
	
		g_vTexCoord1 *= vec2(30.0);
		g_vTexCoord2 *= vec2(30.0);
}