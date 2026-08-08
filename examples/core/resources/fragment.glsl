#version 450                       
layout(location = 0) in vec2 fragTexCoord;              
layout(location = 1) in vec4 fragColor;                 
layout(location = 0) out vec4 finalColor;   

layout(set = 2, binding = 0) uniform Buffer {
    vec4 colorOffset;
};

void main()                        
{                                  
    finalColor = colorOffset + fragColor;        
}                                  