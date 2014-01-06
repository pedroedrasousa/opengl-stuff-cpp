
uniform float offset;
uniform vec3 eyePos;
uniform float fadeStartDist;
uniform float fadeEndDist;
uniform float windStrength;


attribute float startingOffset;
attribute int isTopVertex;

varying vec2 vTexCoord;
varying vec2 vAlphaMultiplier;

void main(void)
{   
	vTexCoord = vec2(gl_MultiTexCoord0);

	float useOffset	= gl_Normal.x; 

	vec4 vertex = gl_Vertex;

	if(isTopVertex == 1)
	{
		vertex.x += cos(startingOffset + offset) * windStrength;
		vertex.z += cos(startingOffset + offset) * windStrength;
	}
	
	float distToEye = distance(vertex.xyz, eyePos);
	
	vAlphaMultiplier = min( 1.0 - (distToEye - fadeStartDist) / (fadeEndDist - fadeStartDist), 1.0 );
	
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}