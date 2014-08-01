/* file: particleTex.vs
 */
attribute POSP vec3 a_position;
attribute TEXP vec2 a_texCoord0;

#ifndef USE_UBO
	uniform MATP mat4 u_MVPMatrix;
	uniform MATP mat4 u_MVMatrix;
#endif

varying TEXP vec2 texCoord0;

void main()
{
	POSP vec4 pPosition = u_MVMatrix * vec4(a_position, 1.0);
	gl_Position = u_MVPMatrix * pPosition;
	texCoord0 = a_texCoord0;
}
