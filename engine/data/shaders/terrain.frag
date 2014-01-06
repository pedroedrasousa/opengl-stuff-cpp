
uniform sampler2D grassMap;
uniform sampler2D normalMap;
uniform sampler2D sandMap;
uniform sampler2D sandAlphaMap;

varying vec2 vTexCoord1;
varying vec2 vTexCoord2;

uniform vec3 lightVec;

void main(void)
{
	vec3 grass		= texture2D(grassMap, vTexCoord2).xyz;
	vec3 normal		= texture2D(normalMap, vTexCoord1).xyz;
	vec3 sand		= texture2D(sandMap, vTexCoord2).xyz;
	float sandAlpha	= texture2D(sandAlphaMap, vTexCoord1).x;
	
	normal = normalize( ( normal * 2.0 ) - 1.0 );
	
	//vec3 normLightVector = normalize(g_vLightVector);
	
	vec3 n_dot_l = vec3( dot(normal, vec3(lightVec.x, -lightVec.z, lightVec.y)) );
	
	vec3 light = max( vec3(0.2, 0.2, 0.2), n_dot_l);
	

	
	light *= 1.5;
	
	//light = min( vec3(0.2, 0.2, 0.2), light);
		
	//light = max( vec3(0.8, 0.8, 0.8), light);
			
	vec3 final = mix(grass, sand, sandAlpha);
		
	gl_FragColor = vec4( final * light, 1.0 );
}