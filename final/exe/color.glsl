uniform sampler2D sampler0;

void main(void)
{		
	float green = dot(texture2D(sampler0, gl_TexCoord[0].xy), vec4(0.299, 0.587, 0.114, 0.1));	
	gl_FragColor = vec4(0.0, green, 0.0, 1.0);
}