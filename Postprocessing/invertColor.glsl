//varying vec4 fragColor;
varying vec4 position; 
//varying vec4 glColor;
uniform float parametr;
uniform sampler2D sampler0;


void main(void)
{
	//tex2D(Tex0, In.TexCoord);
	//gl_FragColor[0] = 1.0 - pcolor[0];
	//gl_FragColor[1] = 1.0 - pcolor[1];
	//gl_FragColor[2] = 1.0 - pcolor[2];
	//gl_FragColor[3] = 1.0 - pcolor[3];
    //gl_FragColor = fragColor;
	//fragColour = colourV;
	//fragColor = pcolor;
	//gl_FragColor = vec4(0.0,1.0,0.0,0);
	//gl_FragColor = gl_Color;
	//gl_FragColor = vec4(0.0,parametr,0.0,0);
	//gl_FragColor= tex2D(Tex0, TexCoord[0]);;
	//float gray = dot(gl_Color.rgb, vec3(0.299, 0.587, 0.114));
// replicate grayscale to RGB components
	//gl_FragColor = vec4(gray, gray, gray, 1.0);
	//vec4 color = texture2D(sampler0, gl_TexCoord[0].xy);
	//vec4 color = vec4(gl_TexCoord[1].xyz, 1.0);
	//vec4 color = texture2D(Texture0, vec2(gl_TexCoord[0]));
	//gl_FragColor =  color;
	//gl_FragColor = vec4(0.7734);
	//gl_FragColor = gl_FragCoord;
	//gl_FragColor = texture2D(sampler0, gl_TexCoord[0]);
	vec4 color = texture2D(sampler0,  gl_TexCoord[0]);
	//gl_FragColor = vec4(1.0-color.xyz, color[3]);
	gl_FragColor = vec4(1.0-color);
}