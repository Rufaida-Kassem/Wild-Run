#version 330
// a postprocessing shader that adds a bloom effect to the scene
// bloom is the effect of light spreading out from bright areas in an image

// the radius of the bloom effect
uniform float bloomRadius = 1.0f;
// the threshold of the bloom effect (the minimum brightness of a pixel to be considered for the bloom effect)
uniform float bloomThreshold = 0.5f;
// the intensity of the bloom effect
uniform float bloomIntensity = 1.0f;
// The texture holding the scene pixels
uniform sampler2D tex;

in vec2 tex_coord;
out vec4 frag_color;

// we get the bloom color of a pixel by substracting the threshold from the pixel color
vec3 GetBloomPixel(vec2 uv, vec2 texPixelSize) {
    //    first we get the pixel of the texture by rounding the uv to the nearest pixel
    vec2 uv2 = floor(uv / texPixelSize) * texPixelSize;
    //    then we add a small offset to get the pixel next to it
    uv2 += texPixelSize * .001;
    //    we get the color of the pixel and we substract the threshold to get the bloom
    //    color and we clamp the result to 0 to avoid negative values
    //    we blur the bloom color by getting the color of the 4 pixels around it and we mix them
    vec3 tl = max(texture(tex, uv2).rgb - bloomThreshold, 0.0);
    vec3 tr = max(texture(tex, uv2 + vec2(texPixelSize.x, 0.0)).rgb - bloomThreshold, 0.0);
    vec3 bl = max(texture(tex, uv2 + vec2(0.0, texPixelSize.y)).rgb - bloomThreshold, 0.0);
    vec3 br = max(texture(tex, uv2 + vec2(texPixelSize.x, texPixelSize.y)).rgb - bloomThreshold, 0.0);
    // fract returns the fractional part of a number
    vec2 f = fract(uv / texPixelSize);

    //   we mix the colors of the 4 pixels around the bloom color
    vec3 tA = mix(tl, tr, f.x);
    vec3 tB = mix(bl, br, f.x);
    return mix(tA, tB, f.y);
}

// we get the bloom color of a pixel by getting the color of the 9 pixels around it and we mix them
vec3 GetBloom(vec2 uv, vec2 texPixelSize) {
    vec3 bloom = vec3(0.0);
    vec2 off = vec2(1) * texPixelSize * bloomRadius;
    bloom += GetBloomPixel(uv + off * vec2(-1, -1), texPixelSize * bloomRadius) * 0.292893;
    bloom += GetBloomPixel(uv + off * vec2(-1, 0), texPixelSize * bloomRadius) * 0.5;
    bloom += GetBloomPixel(uv + off * vec2(-1, 1), texPixelSize * bloomRadius) * 0.292893;
    bloom += GetBloomPixel(uv + off * vec2(0, -1), texPixelSize * bloomRadius) * 0.5;
    bloom += GetBloomPixel(uv + off * vec2(0, 0), texPixelSize * bloomRadius) * 1.0;
    bloom += GetBloomPixel(uv + off * vec2(0, 1), texPixelSize * bloomRadius) * 0.5;
    bloom += GetBloomPixel(uv + off * vec2(1, -1), texPixelSize * bloomRadius) * 0.292893;
    bloom += GetBloomPixel(uv + off * vec2(1, 0), texPixelSize * bloomRadius) * 0.5;
    bloom += GetBloomPixel(uv + off * vec2(1, 1), texPixelSize * bloomRadius) * 0.292893;
    bloom /= 4.171573f;
    return bloom;
}

void main() {
    //    we get  the dimention of the first level of the texture by textureSize(tex, 0)
    //    we get the size of a pixel by dividing 1 by the texture size
    vec2 TEXTURE_PIXEL_SIZE = 1.0 / textureSize(tex, 0);

    vec4 col = texture(tex, tex_coord);
    vec3 bloom = GetBloom(tex_coord, TEXTURE_PIXEL_SIZE);
    col.rgb += bloom * bloomIntensity;
    frag_color = col;
}