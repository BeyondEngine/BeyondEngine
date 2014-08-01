attribute POSP vec4 a_position;
attribute TEXP vec2 a_texCoord0;

#ifndef USE_UBO
	uniform MATP mat4 u_MVPMatrix;
#endif
varying TEXP vec2 texCoord0;
void main()
{
	gl_Position = u_MVPMatrix * a_position;
	texCoord0 = a_texCoord0;
}
