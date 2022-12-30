#version 450 // version 4.5

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

layout(set = 0, binding = 0) uniform UboViewProjection {
    mat4 projection;
    mat4 view;
} uboViewProjection;

// NOT IN USE LEFT FOR REFERENCE
// layout(binding = 1) uniform UboModel {
//     mat4 model;
// } uboModel;

layout(push_constant) uniform PushModel {
    mat4 model;
} pushModel;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec2 fragUV;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragTangent;

void main() {
    // gl_VertexIndex keeps track like a static var
    gl_Position = uboViewProjection.projection * uboViewProjection.view * pushModel.model * vec4(pos, 1.0);
    fragPos = pos;
    fragUV = uv;
    fragNormal = normal;
    fragTangent = tangent;
}