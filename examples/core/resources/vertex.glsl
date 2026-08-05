#version 450                       
layout(location = 0) in vec3 vertexPosition;            
layout(location = 1) in vec2 vertexTexCoord;            
layout(location = 3) in vec4 vertexColor;               
layout(location = 0) out vec2 fragTexCoord;             
layout(location = 1) out vec4 fragColor;                
void main()                        
{                                  
    fragTexCoord = vertexTexCoord; 
    fragColor = vertexColor;       
    gl_Position = vec4(vertexPosition, 1.0); 
}                                  