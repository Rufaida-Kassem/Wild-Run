#version 330 core

uniform sampler2D tex;
in vec2 tex_coord;
// fogDensity is the density of the fog
uniform float fogDensity = 0.1;
// fogColor is the color of the fog
uniform vec3 fogColor = vec3(1.0, 0.0, 0.0);

out vec4 frag_color;

void main()
{


    // Sample the color from the input texture
    vec4 color = texture(tex, tex_coord);

    // Calculate the fog distance based on fragment depth
    float fogDistance = gl_FragCoord.z / gl_FragCoord.w;

    // Calculate the fog factor using the fog distance and density
    float fogFactor = exp(-fogDensity * fogDistance);

    // Apply fog effect to the color
    vec3 foggedColor = mix(color.rgb, fogColor, fogFactor);

    // Set the output fragment color
    frag_color = vec4(foggedColor, color.a);
}
