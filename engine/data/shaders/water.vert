
uniform mat4 g_mModelViewProjection;
uniform float g_waterTranslation;
uniform vec3 g_vEyePos;

varying vec3 g_viewVector;
varying vec2 g_vTexCoord1;
varying vec2 g_vTexCoord2;
varying vec4 g_vProjTexCoord;
varying float g_vFogValue;
//varying vec3 g_eyeVec;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	g_vTexCoord1 = g_vTexCoord2 = vec2(gl_MultiTexCoord0);
	
	g_vProjTexCoord = g_mModelViewProjection * gl_Vertex;
	
	g_vTexCoord1.x += g_waterTranslation * 1.0;
	
	g_vTexCoord2.x -= g_waterTranslation * 1.0;
	g_vTexCoord2.y += 0.5;
	
		g_vTexCoord1 *= vec2(30.0);
		g_vTexCoord2 *= vec2(30.0);

	// Calcular o view vector em tangent space
	vec3 tmpViewVector = g_vEyePos.xyz - gl_Vertex.xyz;
	g_viewVector.x = dot(tmpViewVector, vec3(1.0, 0.0, 0.0));
	g_viewVector.y = dot(tmpViewVector, vec3(0.0, 0.0, -1.0));
	g_viewVector.z = dot(tmpViewVector, vec3(0.0, 1.0, 0.0));
}