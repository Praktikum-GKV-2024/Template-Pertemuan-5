#version 330 core

out vec4 FragColor;

in vec2 UV;

uniform sampler2D textureSampler;
uniform float near_plane;
uniform float far_plane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    // return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
    return (2.0 * near_plane) / (far_plane + near_plane - depth * (far_plane - near_plane));	
}

void main()
{   
    // float depthValue = gl_FragCoord.z;
    // FragColor = vec4(vec3(LinearizeDepth(depthValue)), 1.0);
    gl_FragDepth = gl_FragCoord.z;
}

