#version 330 core

out vec4 frag_color;

// In this shader, we want to draw a checkboard where the size of each tile is (size x size).
// The color of the bottom-left most tile should be "colors[0]" and the 2 tiles adjacent to it
// should have the color "colors[1]".

//TODO: (Req 1) Finish this shader.

uniform int size = 32;
uniform vec3 colors[2];

void main(){
    //. x -> 0 or 1 
    //. y -> 0 or 1
    //. x ^ y -> 0 or 1
    //. the idea in the following line is to get the color of the tile by using the x and y coordinates of the fragment
    //. and the size of the tile by xoring the x and y coordinates of the fragment and then using the result as an index
    //. to the colors array, the reason for xoring is that if x and y are both even or both odd then the result will be 0
    //. and if one of them is even and the other is odd then the result will be 1, so we will get the color of the tile
    frag_color = vec4(colors[int(gl_FragCoord.x)/size % 2 ^ int(gl_FragCoord.y)/size % 2], 1.0);
}