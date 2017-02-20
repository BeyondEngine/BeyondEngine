/* file: PointTexShader.vs
 */
 
attribute POSP vec3 a_position;
attribute TEXP vec2 a_texCoord0;
varying TEXP vec2 texCoord0;
void main()
{
    gl_Position = u_MVPMatrix * vec4(a_position, 1.0);
    texCoord0 = a_texCoord0;
}
