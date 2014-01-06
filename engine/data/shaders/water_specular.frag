
uniform sampler2D g_texHeightMap;
uniform sampler2D g_texNormalMap;
uniform vec3 g_fogColor;


varying vec3 g_lightVector;
varying vec3 g_viewVector;
varying vec2 g_vTexCoord1;
varying vec2 g_vTexCoord2;
varying float g_vFogValue;

void main(void)
{
	vec3 normLightVector = normalize(g_lightVector);
	vec3 normViewVector = normalize(g_viewVector);

	vec2 texCoord1 = g_vTexCoord1;
	vec2 texCoord2 = g_vTexCoord2;
	vec3 bump1 = texture2D(g_texNormalMap, texCoord1).xyz;
	vec3 bump2 = texture2D(g_texNormalMap, texCoord2).xyz;

	//vec3 bump1 = texture2D(g_texNormalMap, g_vTexCoord1).xyz;
	//vec3 bump2 = texture2D(g_texNormalMap, g_vTexCoord2).xyz;
	vec3 bump = mix(bump1, bump2, 0.5);
	
	bump = normalize( ( bump * 2.0 ) - 1.0 );

	vec3 n_dot_l = vec3( dot(bump, normLightVector) );

	vec3 vLightReflect = reflect(-normViewVector, bump);
	vec3 r_dot_l = vec3( dot(vLightReflect, normLightVector) );

	vec3 difuse = min( vec3(0.25), n_dot_l );
	vec3 specular = pow( max(vec3(0.0), r_dot_l), vec3(30.0) );
	

	specular = min(specular, 1.0);
	difuse = min(difuse, 0.1);
	specular *= vec4(2.5);
		
	gl_FragColor = vec4( specular /*+ difuse*/, 1.0); 
}