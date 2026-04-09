#version 400 core
// Used to specify the number of vertices sent to the TPG (Tessellation Primitive Generator)
layout (vertices = 4) out; // TODO for students: modify for more control points!

void main() {
    gl_TessLevelOuter[0] = 1;
//	gl_TessLevelOuter[1] = 16;
//	gl_TessLevelOuter[1] = 32;
	gl_TessLevelOuter[1] = 1024;	// 1024 vertices is 4096 components

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}