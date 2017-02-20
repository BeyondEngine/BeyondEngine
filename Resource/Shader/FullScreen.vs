attribute POSP vec2 a_position;

varying TEXP vec2 texCoord0;

void main()
{
    gl_Position = vec4(a_position.xy, 0.0, 1.0);
    texCoord0 = (a_position.xy + 1.0) * 0.5;
}
