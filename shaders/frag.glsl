#version 330
#extension GL_ARB_gpu_shader_fp64: enable

in vec4 position;

//uniforms
uniform vec2 screen_size;
uniform float iTime;
uniform float zoom;
uniform vec2 offset;
uniform float iRatio;

const float PI = 3.14159265358979;

int max_iters = 256;


//array of colors using to color the fractal
vec3[32] colors = {
    vec3(0.675,0.863,0.918),
    vec3(0.424,0.663,0.894),
    vec3(0.192,0.451,0.831),
    vec3(0.137,0.384,0.796),
    vec3(0.086,0.314,0.753),
    vec3(0.047,0.247,0.706),
    vec3(0.016,0.188,0.651),
    vec3(0.004,0.137,0.592),
    vec3(0.0,0.102,0.545),
    vec3(0.004,0.075,0.502),
    vec3(0.016,0.051,0.459),
    vec3(0.031,0.031,0.408),
    vec3(0.051,0.012,0.361),
    vec3(0.082,0.012,0.31),
    vec3(0.114,0.012,0.267),
    vec3(0.157,0.012,0.227),
    vec3(0.204,0.02,0.188),
    vec3(0.267,0.047,0.153),
    vec3(0.345,0.086,0.122),
    vec3(0.431,0.133,0.086),
    vec3(0.518,0.184,0.063),
    vec3(0.604,0.243,0.035),
    vec3(0.686,0.298,0.016),
    vec3(0.753,0.357,0.004),
    vec3(0.808,0.404,0.0),
    vec3(0.851,0.459,0.008),
    vec3(0.882,0.518,0.02),
    vec3(0.91,0.573,0.043),
    vec3(0.933,0.631,0.071),
    vec3(0.949,0.686,0.098),
    vec3(0.969,0.733,0.122),
    vec3(0.976,0.776,0.141),
};




float atan2(in float y, in float x)
{
    bool s = (abs(x) > abs(y));
    return mix(PI/2.0 - atan(x,y), atan(y,x), s);
}

//scuare complex number z
vec4 dcSqr( vec4 a )
{
    return vec4( a.x*a.x - a.y*a.y, 
				 2.0*a.x*a.y,
				 2.0*(a.x*a.z - a.y*a.w),
				 2.0*(a.x*a.w + a.y*a.z) );
}

//compare complex number and a float
bool dcComp(vec4 a, float b){
    if (a.x > b) return false;
    if (a.y > b) return false;
    if (a.z > b) return false;
    if (a.w > b) return false;
    return true;
}

bool dcComp(vec4 a, uint b){
    if (a.x > b) return false;
    if (a.y > b) return false;
    if (a.z > b) return false;
    if (a.w > b) return false;
    return true;
}

bool dcComp(vec4 a, vec4 b){
    if (!dcComp(a, b.x)) return false;
    if (!dcComp(a, b.y)) return false;
    if (!dcComp(a, b.z)) return false;
    if (!dcComp(a, b.w)) return false;
    return true;
}

void main()
{    
    vec2 p = vec2(position.x*2, position.y);

    vec4 c = vec4(p * zoom + offset, 0.0000001, 0.0 );

    vec4 z = vec4(0);
    int n = 0;
    
    //loop that checks how fast number goes towards the infinity
    while (dcComp(abs(z), 6.0) && n < max_iters){
        z = dcSqr(z) + c;
        n += 1;
    }

    
    //coloring the pixel
    vec3 color;
    if (n > max_iters-1)
        color = vec3(0);
    else
        //color = vec3(n/776.14, n/152.38, n/232.3); //other coloring variant that works faster (at least for me)
        color = colors[n%32];
    gl_FragColor = vec4(color, 1);

}