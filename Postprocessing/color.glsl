#version 120

uniform sampler2D color;

void main(void)
{	
	float res = dot(texture2D(color, gl_TexCoord[0].xy), vec4(0.2, 0.6, 0.1, 0.1));
	result = vec4(0.0, res, 0.0, 1.0);	
	gl_FragColor = result;		
}