#include <def.glsl>
#include <const.h>
layout(location = 0) in float pos;
void main() {
  float id = gl_VertexIndex * cmn.scale - 1;
  float y = pos * -I2_3;
  float ofst = gl_InstanceIndex;
  gl_PointSize = 1;
  gl_Position = vec4(id, y - I_3 + ofst, 0, 1);
}
