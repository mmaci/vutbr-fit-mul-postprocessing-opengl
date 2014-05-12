uniform sampler2D sampler0;

void main(void)
{		
	float res = dot(texture2D(sampler0, gl_TexCoord[0].xy), vec4(0.2, 0.6, 0.1, 0.1));
	vec4 result = vec4(0.0, res, 0.0, 1.0);	
	gl_FragColor = result;		
}