uniform sampler2D sampler0;

void main(void)
{
	
	gl_TexCoord[0] = gl_TexCoord[0]  + (sin(gl_TexCoord[0]*50)*0.01); 
	vec4 color = texture2D(sampler0,  gl_TexCoord[0]);
	gl_FragColor = vec4(color);
}

