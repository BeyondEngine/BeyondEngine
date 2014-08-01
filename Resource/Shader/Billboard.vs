/* file: billbord.vs
 */
attribute POSP vec3 a_position;
attribute COLP vec4 a_color;
attribute TEXP vec2 a_texCoord0;

#ifndef USE_UBO
    uniform MATP mat4 u_MVPMatrix;
    uniform MATP mat4 u_transforMatrix;
#endif

varying COLP vec4 colortmp;
varying TEXP vec2 texCoord0;

void main()
{
    POSP vec4 pPosition = u_transforMatrix * vec4(a_position, 1.0);
    gl_Position = u_MVPMatrix * pPosition;
    colortmp = a_color;
    texCoord0 = a_texCoord0;
}
