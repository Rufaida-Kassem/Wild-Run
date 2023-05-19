#version 330 core

out vec4 frag_color;


//to draw a circle around the mouse pointer

uniform vec2 mouse_pos;



void main (){
    float radius =10;
    float dist = distance(gl_FragCoord.xy, mouse_pos);
    if (dist < radius){
        frag_color = vec4(1.0, 1.0, 0.0, 1.0);
    } else {
        frag_color = vec4(0, 0, 0, 0.0);
        //        frag_color = vec4(gl_FragCoord.x, gl_FragCoord.y, 0, 1.0);
    }
}













