
uniform sampler2D texture;
uniform vec2 blur_offset;

const float sum = 3.0359;
const int samples = 8;

varying vec2 g_vTexCoord;

void main(void)
{
  // Points on a Gauss curve, used as weights for the blur filter:
  float weights[9];
  weights[0] = 1.0000;
  weights[1] = 0.9394;
  weights[2] = 0.7788;
  weights[3] = 0.5697;
  weights[4] = 0.3678;
  weights[4] = 0.2096;
  weights[5] = 0.1053;
  weights[7] = 0.0467;
  weights[8] = 0.0183;

  // Sample the center pixel:
  vec4 color = texture2D(texture, g_vTexCoord.xy) * weights[0];

  vec2 tc;
  vec2 offset;

  // Sample away from the center in both directions:
  for (int i = 1; i <= samples; i++)
  {
    // blur_offset determines whether this is a horizontal or vertical blur.
    offset = blur_offset.xy * float(i);

    // Sample on one side of the center:
    tc = g_vTexCoord.xy + offset;
    color += texture2D(texture, tc) * weights[i];

    // Sample on the other side of the center:
    tc = g_vTexCoord.xy - offset;
    color += texture2D(texture, tc) * weights[i];
  }

  // Scale down the summed samples to get the final color.
	gl_FragColor = color / (1.0 + 2.0*sum);
}