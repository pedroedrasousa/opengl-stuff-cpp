
uniform sampler2D g_texNormalMap;
uniform sampler2D g_texViewport;

uniform vec3 g_fogColor;

varying vec3 g_viewVector;
varying vec2 g_vTexCoord1;
varying vec2 g_vTexCoord2;
varying vec2 g_vTexCoord;

void main(void)
{
	vec3 bump1 = texture2D(g_texNormalMap, g_vTexCoord1).xyz;
	vec3 bump2 = texture2D(g_texNormalMap, g_vTexCoord2).xyz;

	vec3 bump = mix(bump1, bump2, 0.5);
	bump = normalize( (bump * 2.0) - 1.0 );
	

	vec2 vReflectTexCoords;

	vReflectTexCoords.x = g_vTexCoord.x + bump.x * 0.01; 
	vReflectTexCoords.y = g_vTexCoord.y + bump.y * 0.01; 
	

	vec3 reflection	= texture2D(g_texViewport, vReflectTexCoords).xyz;
	
	vec3 final = reflection;
	/*
	final.x -= 0.1;
	final.y -= 0.1;
	final.z += 0.1;
	*/
	final = mix(final, vec3(0.3, 0.3, 0.8), 0.4);

	gl_FragColor = vec4(final, 1.0); 
}