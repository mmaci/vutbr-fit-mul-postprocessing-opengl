uniform sampler2D sampler0;


void main(void)
{
	vec4 color = texture2D(sampler0,  gl_TexCoord[0]);
	gl_FragColor = vec4(1.0-color);
}