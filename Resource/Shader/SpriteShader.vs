/* file: PointTexShader.vs
 */
 
attribute POSP vec3 a_position;
attribute HIGHP vec4 a_colorScale;
attribute TEXP vec2 a_texCoord0;
varying TEXP vec2 texCoord0;
varying HIGHP vec4 colorScale;
void main()
{
    gl_Position = u_MVPMatrix * vec4(a_position, 1.0);
    texCoord0 = a_texCoord0;
    colorScale = a_colorScale;
}
