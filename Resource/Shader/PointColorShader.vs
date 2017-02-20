/* file: PointColorShader.vs
 */
attribute POSP vec4 a_position;
attribute COLP vec4 a_color;

varying COLP vec4 color;

void main()
{
    gl_Position = u_MVPMatrix * a_position;
    color = a_color;
}
