attribute POSP vec2 a_position;

#ifndef USE_UBO
	uniform MATP mat4 u_MVPMatrix;
#endif

varying TEXP vec2 texCoord0;

void main()
{
    gl_Position = vec4(a_position.xy, 0.0F, 1.0F);
	texCoord0 = (a_position.xy + 1.0F) * 0.5F;
}
