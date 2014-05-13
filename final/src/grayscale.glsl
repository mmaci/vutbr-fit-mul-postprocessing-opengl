uniform sampler2D sampler0;


void main(void)
{
	vec4 Color = texture2D(sampler0, gl_TexCoord[0]);
	float gray = pow((0.299* Color[0] + 0.587 * Color[1] + 0.114* Color[2]),  0.5) * Color[3];
	gl_FragColor = vec4(gray, gray, gray, gray);

}