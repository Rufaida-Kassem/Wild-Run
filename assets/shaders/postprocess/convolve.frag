#version 330

// The texture holding the scene pixels
uniform sampler2D tex;

// Read "assets/shaders/fullscreen.vert" to know what "tex_coord" holds;
in vec2 tex_coord;

out vec4 frag_color;


void main() {

    ivec2 texture_size = textureSize(tex, 0);
    vec2 step_size = 1.0/texture_size;

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

    //    float kernel[9] = float[](
    //    -1, -1, -1,
    //    -1, 9, -1,
    //    -1, -1, -1
    //    );

    //        averaging kernal
    float kernel[9] = float[](
    1.0/9.0, 1.0/9.0, 1.0/9.0,
    1.0/9.0, 1.0/9.0, 1.0/9.0,
    1.0/9.0, 1.0/9.0, 1.0/9.0
    );

    vec4 sum = vec4(0.0);
    for (int i = 0; i < 9; i++) {
        sum += texture(tex, tex_coord + offsets[i]) * kernel[i];
    }

    sum.a = 1.0;
    frag_color = sum;

}