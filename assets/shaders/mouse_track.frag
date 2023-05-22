#version 330 core

out vec4 frag_color;

// its just a shader that draws a circle around the mouse pointer


// the mouse position is passed in as a uniform
// the uniform is a vec2, which is a 2D vector
uniform vec2 mouse_pos;



void main (){
    // the radius of the circle drawn around the mouse
    float radius =10;
    //distance is a built in function that returns the distance between two points
    // it will highlight the pixels that are within the radius of the mouse
    float dist = distance(gl_FragCoord.xy, mouse_pos);

    // if the distance is less than the radius, then the pixel is within the circle
    if (dist < radius){
        frag_color = vec4(1.0, 1.0, 0.0, 1.0);
    } else {
        frag_color = vec4(0, 0, 0, 0.0);
    }
}













