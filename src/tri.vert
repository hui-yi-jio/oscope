#include <def.glsl>
layout(location = 0) out vec4 col;
void main(){
	uint id = gl_VertexIndex;
	gl_PointSize = 1;
  gl_Position = vec4(1,1,0,1);
}
