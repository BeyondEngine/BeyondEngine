/* file: Header.vs
 */
#ifdef USE_UBO
	layout(std140) uniform ub_MVPMatrix
	{
		uniform MATP mat4 u_MVPMatrix;
		uniform MATP mat4 u_MMatrix;
		uniform MATP mat4 u_VMatrix;
		uniform MATP mat4 u_PMatrix;
	};
#endif

