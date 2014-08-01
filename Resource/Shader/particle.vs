/* file: particle.vs
 */
attribute POSP vec3 a_position;
attribute COLP vec3 a_color;

#ifndef USE_UBO
	uniform MATP mat4 u_MVPMatrix;
	uniform MATP mat4 u_MVMatrix;
#endif

varying COLP vec3 colortmp;

void main()
{
	POSP vec4 pPosition = u_MVMatrix * vec4(a_position, 1.0);
	gl_Position = u_MVPMatrix * pPosition;
	colortmp = a_color;
}
