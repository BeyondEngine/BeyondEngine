/* file: SkinShader.vs
 */
attribute POSP vec3 a_position;
attribute TEXP vec2 a_texCoord0;
attribute BIDXP vec4 a_boneIndices;
attribute WEIP vec4 a_weights;

uniform MATP mat4 u_BoneMatrices[32];
varying TEXP vec2 TexCoord0;

void main()
{
    POSP vec4 originPos = vec4(a_position, 1.0);
    POSP vec4 finalPos = u_BoneMatrices[int(a_boneIndices.x)] * originPos * a_weights.x;
    if(a_boneIndices.y >= 0.0)
    {
        finalPos += u_BoneMatrices[int(a_boneIndices.y)] * originPos * a_weights.y;
    }
    if(a_boneIndices.z >= 0.0)
    {
        finalPos += u_BoneMatrices[int(a_boneIndices.z)] * originPos * a_weights.z;
    }
    if(a_boneIndices.w >= 0.0)
    {
        finalPos += u_BoneMatrices[int(a_boneIndices.w)] * originPos * a_weights.w;        
    }
    gl_Position = u_MVPMatrix * finalPos;
    TexCoord0 = a_texCoord0;
}
