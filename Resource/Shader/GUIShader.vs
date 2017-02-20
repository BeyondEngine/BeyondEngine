/* file: GUIShader.vs
 */

attribute POSP vec3 a_position;
attribute COLP vec4 a_color;
attribute COLP vec4 a_melody;
attribute TEXP vec2 a_texCoord0;

varying TEXP vec2 texCoord0;
varying COLP vec4 color;
varying COLP vec4 melody;

void main()
{
    gl_Position = u_MVPMatrix * vec4(a_position, 1.0);
    texCoord0 = a_texCoord0;
    color = a_color;
    melody = a_melody;
}
