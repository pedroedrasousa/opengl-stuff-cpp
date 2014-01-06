
uniform sampler2D g_texHeightMap;
uniform sampler2D g_texNormalMap;
uniform sampler2D g_texReflection;
uniform sampler2D g_texRefraction;

uniform vec3 g_fogColor;

varying vec3 g_viewVector;
varying vec2 g_vTexCoord1;
varying vec2 g_vTexCoord2;
varying vec4 g_vProjTexCoord;
varying float g_vFogValue;

void main(void)
{
	vec3 normViewVector = normalize(g_viewVector);

	vec3 bump1 = texture2D(g_texNormalMap, g_vTexCoord1).xyz;
	vec3 bump2 = texture2D(g_texNormalMap, g_vTexCoord2).xyz;

	vec3 bump = mix(bump1, bump2, 0.5);
	bump = normalize( (bump * 2.0) - 1.0 );
	

	vec2 vReflectTexCoords, vRefractTexCoords;
	vReflectTexCoords = vRefractTexCoords = vec2(g_vProjTexCoord.x / g_vProjTexCoord.w, g_vProjTexCoord.y / g_vProjTexCoord.w);

	vReflectTexCoords.x = vReflectTexCoords.x + bump.x * 0.03; 
	vReflectTexCoords.y = vReflectTexCoords.y + bump.y * 0.03; 
	
	vRefractTexCoords.x = vRefractTexCoords.x - bump.x * 0.005; 
	vRefractTexCoords.y = vRefractTexCoords.y - bump.y * 0.005; 

	vec3 reflection	= texture2D(g_texReflection, vReflectTexCoords).xyz;
	vec3 refraction	= texture2D(g_texRefraction, vRefractTexCoords).xyz;
	
	vec3 final = mix(refraction, reflection , 0.4);
	

	gl_FragColor = vec4(final, 1.0); 
}