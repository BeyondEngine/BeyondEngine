/* file: SkinShader.vs
 */
attribute POSP vec3 a_position;
attribute TEXP vec2 a_texCoord0;
attribute BIDXP ivec4 a_boneIndices;
attribute WEIP vec4 a_weights;

uniform MATP mat4 u_BoneMatrices[100];
#ifndef USE_UBO
	uniform MATP mat4 u_MVPMatrix;
#endif

varying TEXP vec2 TexCoord0;

void main()
{
	POSP vec4 finalPos = vec4(a_position, 1);
	POSP vec4 finalPos2 = vec4(0,0,0,0);
	MATP mat4 matrix0 = u_BoneMatrices[a_boneIndices.x];
	finalPos2 = matrix0 * finalPos;
	finalPos2 *= a_weights.x;
	if(a_boneIndices.y >= 0)
	{
		MATP mat4 matrix1 = u_BoneMatrices[a_boneIndices.y];
		POSP vec4 finalPos3 = matrix1 * finalPos;
		finalPos3 *= a_weights.y;
		finalPos2 += finalPos3;
	}
	if(a_boneIndices.z >= 0)
	{
		MATP mat4 matrix2 = u_BoneMatrices[a_boneIndices.z];
		POSP vec4 finalPos3 = matrix2 * finalPos;
		finalPos3 *= a_weights.z;
		finalPos2 += finalPos3;
	}
	if(a_boneIndices.w >= 0)
	{
		MATP mat4 matrix3 = u_BoneMatrices[a_boneIndices.w];
		POSP vec4 finalPos3 = matrix3 * finalPos;
		finalPos3 *= a_weights.w;
		finalPos2 += finalPos3;
	}
	gl_Position = u_MVPMatrix * finalPos2;
	TexCoord0 = a_texCoord0;
}
