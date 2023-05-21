#version 330

// first we define the different types of lights
// directional lights are lights that are infinitely far away
// and have no position, only a direction
// point lights are lights that have a position only. this type of lights radiate light in all directions 
// spot lights are lights that have a position and a direction. 
// they also have a cone of light that is defined by two angles. 
// the first angle is the angle of the cone at the light's position
// the second angle is the angle of the cone at the end of the cone
// the light's intensity is linearly interpolated between the two angles
// the light's intensity is 0 outside of the cone
#define DIRECTIONAL 0
#define POINT       1
#define SPOT        2

// we define a struct that represents a light
struct Light {
    int type; // the type of the light is defined by the type field and can be one of the three types defined above
    vec3 position; // the position of the light is defined by the position field and is needed only for point and spot lights
    vec3 direction; // the direction of the light is defined by the direction field and is needed only for directional and spot lights
    vec3 color; // the color of the light is defined by the color field and is needed for all types of lights
    vec3 attenuation; // this field is needed only for both point and spot lights
    vec2 cone_angles; // the cone angles of the light are defined for spot lights only
};

#define MAX_LIGHTS 32 // we define the maximum number of lights that can be passed to the shader as a uniform

uniform Light lights[MAX_LIGHTS]; // we define an array of lights that will be passed to the shader as a uniform
uniform int light_count; // we define the number of lights that will be passed to the shader as a uniform. this number must be less than or equal to MAX_LIGHTS

// we define a struct that represents a sky
// the sky is defined by three colors: the top color, the horizon color and the bottom color
// the top color is the color of the sky at the top of the skybox
// the horizon color is the color of the sky at the horizon
// the bottom color is the color of the sky at the bottom of the skybox
struct Sky {
    vec3 top, horizon, bottom;
};

// we define a sky that will be passed to the shader as a uniform
uniform Sky sky;

// we define a function that computes the color of the sky given a normal vector that represents the direction of the sky
// if the normal vector is pointing up, the ambient light will come from the tob
// if the normal vector is pointing down, the ambient light will come from the bottom
// the normal vector must be normalized before being passed to the function otherwise the result will be incorrect
// the function returns the color of the sky at the given direction
// @param normal: the direction of the sky
// @return: the color of the sky at the given direction
// hint --> this function is used to compute the ambient light of the object in the fragment shader instead of using a constant color for the ambient light to be more realistic
vec3 compute_sky_light(vec3 normal){
    vec3 extreme = normal.y > 0 ? sky.top : sky.bottom; // we compute the color of the sky at the top or bottom of the skybox depending on the direction of the normal
    // function mix(a, b, t) returns the linear interpolation between a and b at t
    // the product of normal.y and normal.y is used to make the interpolation non-linear as the normal vector is not linearly distributed
    // this makes the color of the sky at the horizon more visible
    return mix(sky.horizon, extreme, normal.y * normal.y); // we compute the color of the sky by interpolating between the color of the sky at the horizon and the color of the sky at the top or bottom of the skybox depending on the direction of the normal
    // hint --> we can use abs(normal.y) instead of normal.y * normal.y to make the interpolation linear but it will be very sharp
}

// we create a struct that represents the material of the object
// the material is defined by five textures: the albedo texture, the specular texture, the roughness texture, the ambient occlusion texture and the emissive texture
struct Material {
    sampler2D albedo; // the albedo texture is the texture that defines the color of the object
    sampler2D specular; // the specular texture is the texture that defines the color of the specular highlights of the object
    sampler2D roughness; // the roughness texture is the texture that defines the roughness of the object
    sampler2D ambient_occlusion; // the ambient occlusion texture is the texture that defines the ambient occlusion of the object (how much light is occluded by the object)
    sampler2D emissive; // the emissive texture is the texture that defines the emissive color of the object (the color of the object when it emits light)
};

// we define a material that will be passed to the shader as a uniform
uniform Material material;

// the following are the varyings that are passed from the vertex shader to the fragment shader
in Varyings {
    vec4 color; // the color of the vertex
    vec2 tex_coord; // the texture coordinates of the vertex
    vec3 normal; // the normal of the vertex in world space coordinates 
    vec3 view; // the direction of the view in world space coordinates 
    vec3 world; // the position of the vertex in world space coordinates
} fs_in;

out vec4 frag_color; // the color of the fragment will be outputted to the fragment shader to be displayed on the screen

// the following function computes the lambertian reflectance of the object
// the lambertian reflectance is the amount of light that is reflected by the object in all directions (diffuse light)
// the function takes as parameters the normal of the object and the direction of the light
// the function returns the amount of light that is reflected by the object
// @param normal: the normal of the object
// @param world_to_light_direction: the direction of the light
// @return: the amount of light that is reflected by the object (diffuse light)
float lambert(vec3 normal, vec3 world_to_light_direction) {
    // we compute the lambertian reflectance by computing the dot product between the normal 
    // and the direction of the light and we take the maximum between the result and 0 
    // to make sure that the result is positive cause the dot product can be negative 
    // and if it is negative then it shouldn't appear in the final result
    return max(0.0, dot(normal, world_to_light_direction)); 
}

// the following function computes the phong reflectance of the object
// the phong reflectance is the amount of light that is reflected by the object in the direction of the view (specular light)
// the function takes as parameters the direction of the reflected light, the direction of the view and the shininess of the object
// the function returns the amount of light that is reflected by the object
// @param reflected: the direction of the reflected light
// @param view: the direction of the view
// @param shininess: the shininess of the object
// @return: the amount of light that is reflected by the object (specular light)
float phong(vec3 reflected, vec3 view, float shininess) {
    return pow(max(0.0, dot(reflected, view)), shininess);
}

void main() {
    vec3 normal = normalize(fs_in.normal); // we normalize the normal of the fragment to make sure that it is a unit vector
    vec3 view = normalize(fs_in.view); // we normalize the direction of the view to make sure that it is a unit vector
    
    vec3 ambient_light = compute_sky_light(normal); // we compute the color of the sky at the direction of the normal to get the ambient light

    vec3 diffuse = texture(material.albedo, fs_in.tex_coord).rgb; // we get the color of the object from the albedo texture
    vec3 specular = texture(material.specular, fs_in.tex_coord).rgb; // we get the color of the specular highlights of the object from the specular texture
    float roughness = texture(material.roughness, fs_in.tex_coord).r; // we get the roughness of the object from the roughness texture
    vec3 ambient = diffuse * texture(material.ambient_occlusion, fs_in.tex_coord).r; // we get the ambient occlusion of the object from the ambient occlusion texture
    vec3 emissive = texture(material.emissive, fs_in.tex_coord).rgb; // we get the emissive color of the object from the emissive texture

    // we compute the shininess of the object from the roughness of the object
    // the shininess is computed using the following formula:
    // shininess = 2 / roughness^4 - 2
    // the shininess is used to compute the specular light of the object
    // we clamp the roughness between 0.001 and 0.999 to avoid division by 0
    float shininess = 2.0 / pow(clamp(roughness, 0.001, 0.999), 4.0) - 2.0;
    
    vec3 world_to_light_dir; // we define a variable that will hold the direction of the light in world space coordinates
    vec3 color = emissive + ambient_light * ambient; // we compute the color of the fragment by adding the emissive color of the object and the ambient light of the object

    // we loop over all the lights that are passed to the shader
    // we compute the color of the fragment for each light
    // we add the color of the fragment for each light to the color of the fragment
    // the color of the fragment for each light is computed by adding the diffuse light and the specular light of the object
    // the diffuse light is computed using the lambertian reflectance of the object
    // the specular light is computed using the phong reflectance of the object
    // the diffuse light and the specular light are attenuated by the attenuation of the light
    for(int light_idx = 0; light_idx < min(MAX_LIGHTS, light_count); light_idx++){
        Light light = lights[light_idx]; // we get the light at the current index
        float attenuation = 1.0; // we define a variable that will hold the attenuation of the light
        if(light.type == DIRECTIONAL){ // if the light is a directional light
            world_to_light_dir = -light.direction; // we set the direction of the light to the opposite of the direction of the light cause all lights are out from the object so that the range of the angle between the normal and the direction of the light is between 0 and 180 degrees
        } else { // if the light is a point or spot light
            world_to_light_dir = light.position - fs_in.world; // we compute the direction of the light by subtracting the position of the light from the position of the fragment
            float d = length(world_to_light_dir); // we compute the distance between the light and the fragment to compute the attenuation of the light
            world_to_light_dir /= d; // we normalize the direction of the light to make sure that it is a unit vector

            attenuation = 1.0 / dot(light.attenuation, vec3(d*d, d, 1.0)); // we compute the attenuation of the light using the attenuation of the light and the distance between the light and the fragment

            // if the light is a spot light, we compute the attenuation of the light using the cone angles of the light
            if(light.type == SPOT){
                float angle = acos(dot(light.direction, -world_to_light_dir));
                attenuation *= smoothstep(light.cone_angles.y, light.cone_angles.x, angle); // we compute the attenuation of the light using the cone angles of the light. The function smoothstep(a, b, t) returns the interpolation between a and b at t. The interpolation is smooth at the edges. The interpolation is linear between a and b if t is between a and b. The interpolation is constant if t is less than a or greater than b.
            }
        }

        // we compute the diffuse light and the specular light of the object
        vec3 computed_diffuse = light.color * diffuse * lambert(normal, world_to_light_dir);

        // we compute the specular light of the object
        vec3 reflected = reflect(-world_to_light_dir, normal); // we compute the direction of the reflected light by reflecting the direction of the light around the normal of the object to be able to get the specular light of the object
        vec3 computed_specular = light.color * specular * phong(reflected, view, shininess); // we compute the specular light of the object using the phong reflectance of the object

        // we add the diffuse light and the specular light of the object to the color of the fragment
        color += (computed_diffuse + computed_specular) * attenuation;

    }
    // we set the color of the fragment to the color of the fragment computed above and we set the alpha of the fragment to 1 (fully opaque) 
    frag_color = vec4(color, 1.0);
}