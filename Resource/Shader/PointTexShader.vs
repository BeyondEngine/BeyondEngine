/* file: PointTexShader.vs
 */
attribute POSP vec3 a_position;
attribute TEXP vec2 a_texCoord0;

#ifndef USE_UBO
	uniform MATP mat4 u_MVPMatrix;
#endif

varying TEXP vec2 texCoord0;

void main()
{
	gl_Position = u_MVPMatrix * vec4(a_position, 1.0);
	texCoord0 = a_texCoord0;
}
