/* file: TextShader.vs
 */
attribute POSP vec4 a_position;
attribute COLP vec4 a_color;
attribute COLP vec4 a_borderColor;
attribute TEXP vec2 a_texCoord0;
varying COLP vec4 color;
varying COLP vec4 borderColor;
varying TEXP vec2 texCoord0;

void main()
{
    gl_Position = u_MVPMatrix * a_position;
    color = a_color;
    borderColor = a_borderColor;
    texCoord0 = a_texCoord0;
}
