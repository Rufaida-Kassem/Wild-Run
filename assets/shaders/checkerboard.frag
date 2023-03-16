#version 330 core

out vec4 frag_color;

// In this shader, we want to draw a checkboard where the size of each tile is (size x size).
// The color of the bottom-left most tile should be "colors[0]" and the 2 tiles adjacent to it
// should have the color "colors[1]".

//TODO: (Req 1) Finish this shader.

uniform int size = 32;
uniform vec3 colors[2];

void main(){
    // x -> 0 or 1 
    // y -> 0 or 1
    // x ^ y -> 0 or 1
    // indicates if the position of the tile is odd or even in both x and y
    frag_color = vec4(colors[int(gl_FragCoord.x)/size % 2 ^ int(gl_FragCoord.y)/size % 2], 1.0);
}