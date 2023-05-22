#version 330

// The texture holding the scene pixels
uniform sampler2D tex;
in vec2 tex_coord;
out vec4 frag_color;


void main() {
    // we first get the size of the texture so we know the step size in each direction
    ivec2 texture_size = textureSize(tex, 0);
    vec2 step_size = 1.0/texture_size;

    //   offsets for the 3x3 kernel 
    // we move in each direction by the step size
    vec2 offsets[9] = vec2[](
    vec2(-step_size.x, -step_size.y),
    vec2(0.0f, -step_size.y),
    vec2(step_size.x, -step_size.y),
    vec2(-step_size.x, 0.0f),
    vec2(0.0f, 0.0f),
    vec2(step_size.x, 0.0f),
    vec2(-step_size.x, +step_size.y),
    vec2(0.0f, +step_size.y),
    vec2(step_size.x, step_size.y)
    );

    //    edge detection kernel
       float kernel[9] = float[](
       -1, -1, -1,
       -1, 9, -1,
       -1, -1, -1
       );

    // averaging kernal for blurring
    // float kernel[9] = float[](
    // 1.0/9.0, 1.0/9.0, 1.0/9.0,
    // 1.0/9.0, 1.0/9.0, 1.0/9.0,
    // 1.0/9.0, 1.0/9.0, 1.0/9.0
    // );

    vec4 sum = vec4(0.0);
    // sum with weights in the kernel taking a step in each direction determined by the offsets
    for (int i = 0; i < 9; i++) {
        sum += texture(tex, tex_coord + offsets[i]) * kernel[i];
    }
    // set alpha to 1.0 and the color to the sum of the kernel  
    sum.a = 1.0;
    frag_color = sum;

}