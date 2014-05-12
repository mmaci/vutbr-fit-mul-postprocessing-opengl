uniform sampler2D sampler0;
uniform float time;

float rand(vec2 n, float time)
{
  return 0.5 + 0.5 * 
     fract(sin(dot(n.xy, vec2(12.9898*time, 78.233*time)))* 43758.5453);
}
void main(void)
{

  float y = rand(gl_TexCoord[0].xy, time);
  if (y > 0.975)
  {
	float x = rand(gl_TexCoord[0].xz, time);
	gl_FragColor = vec4(x, x, x, 1.0);
  }else
  {
	gl_FragColor = texture2D(sampler0,  gl_TexCoord[0]);
  }

  
}