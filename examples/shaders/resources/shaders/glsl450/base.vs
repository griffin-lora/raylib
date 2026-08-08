#version 450

// Input vertex attributes
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec4 vertexColor;

// Input uniform values
layout(set = 0, binding = 0) uniform MVP {
    mat4 mvp;
};

// Output vertex attributes (to fragment shader)
layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragColor;

// NOTE: Add your custom variables here

void main()
{
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;

    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}