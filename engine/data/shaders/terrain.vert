
uniform vec3 g_vLightPos;
uniform vec3 g_vEyePos;

varying vec2 vTexCoord1;
varying vec2 vTexCoord2;

//varying vec3 lightVec;

void main(void)
{
	vTexCoord1 = vec2(gl_MultiTexCoord0);
	vTexCoord2 = vec2(gl_MultiTexCoord0) * vec2(100.0);

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}