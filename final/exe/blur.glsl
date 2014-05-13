uniform sampler2D sampler0;

void main(void)
{
  	float offset = 0.005;
	gl_FragColor = float4(0,0,0,0);
	for (int x = -1; x <= 1; x++)
		for (int y = -1; y <= 1; y++)
		{
			vec2 offset2 = vec2 (x,y) * offset;
			vec2 coord =   gl_TexCoord[0] + offset2;
			gl_FragColor += tex2D(sampler0, coord)/9; 
		}

}

