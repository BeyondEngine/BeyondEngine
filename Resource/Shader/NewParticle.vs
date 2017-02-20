attribute vec3 a_position;
attribute vec4 a_color;
attribute vec2 a_texCoord0;

uniform TEXP vec4 _MainTex_ST;
varying COLP vec4 color;
varying TEXP vec2 texCoord0;

void main ()
{
    gl_Position = u_MVPMatrix * vec4(a_position, 1.0);
    color = a_color;
    texCoord0 = ((a_texCoord0.xy * _MainTex_ST.xy) + _MainTex_ST.zw);
}