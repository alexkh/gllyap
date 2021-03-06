//Original code by Ingo Quilez 2013 https://www.shadertoy.com/view/Mds3R8

vec2 iResolution = vec2(1920, 1080);

vec3 calc( in vec2 p )
{
/*	float w1 = 0.97 + 0.04*sin( 0.0 + 1.3*iGlobalTime*0.2 );
	float w2 = 0.97 + 0.04*sin( 1.0 + 1.7*iGlobalTime*0.2 );
	float w3 = 0.97 + 0.04*sin( 4.0 + 1.1*iGlobalTime*0.2 );
	float w4 = 0.97 + 0.04*sin( 2.0 + 1.5*iGlobalTime*0.2 );
	float w5 = 0.97 + 0.04*sin( 5.0 + 1.9*iGlobalTime*0.2 );
*/	
	float w1, w2, w3, w4, w5;
	w1 = w2 = w3 = w4 = w5 = 1.0;
	w3 = w5 = 1.0;
	
	float x = 0.5;
	float h = 0.0;
	for( int i=0; i<200; i++ )
	{
		x = w1*p.x*x*(1.0-x); h += log2(abs(w1*p.x*(1.0-2.0*x)));
		x = w2*p.x*x*(1.0-x); h += log2(abs(w2*p.x*(1.0-2.0*x)));
		x = w3*p.y*x*(1.0-x); h += log2(abs(w3*p.y*(1.0-2.0*x)));
		x = w4*p.x*x*(1.0-x); h += log2(abs(w4*p.x*(1.0-2.0*x)));
		x = w5*p.y*x*(1.0-x); h += log2(abs(w5*p.y*(1.0-2.0*x)));
	}
    h /= 200.0*5.0;
	
	
	vec3 col = vec3(0.0);
	if( h<0.0 )
	{
		h = abs(h);
		col = vec3( 0.5+0.5*sin(0.0+2.5*h),
	
			    0.5+0.5*sin(0.4+2.5*h),
				    0.5+0.5*sin(0.7+2.5*h) );
		col *= vec3(1.1)*pow(h,0.25);
	}
	

	return col;
}

void main() {
#if 1
	vec3 col = calc( 3.0 + 1.0*gl_FragCoord.xy/iResolution.xy );
#else
	vec3 col = calc( 3.0 + 1.0*(gl_FragCoord.xy+vec2(0.0,0.0)) / iResolution.xy ) +
	           calc( 3.0 + 1.0*(gl_FragCoord.xy+vec2(0.0,0.5)) / iResolution.xy ) +
	           calc( 3.0 + 1.0*(gl_FragCoord.xy+vec2(0.5,0.0)) / iResolution.xy ) +
	           calc( 3.0 + 1.0*(gl_FragCoord.xy+vec2(0.5,0.5)) / iResolution.xy );
    col /= 4.0;
#endif
	
	gl_FragColor = vec4( col, 1.0 );
}
