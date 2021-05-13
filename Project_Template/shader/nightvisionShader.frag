#version 460

//Variables passed in from the Vertex Shader and used for the frag methods
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform int Width;
uniform int Height;
uniform float Radius;
uniform sampler2D RenderTex;
uniform sampler2D NoiseTex;

subroutine vec4 RenderPassType();
subroutine uniform RenderPassType RenderPass;

struct LightInfo 
{
   vec4 Position; //Light position in eye coords
   vec3 La;       
    vec3 L;   // A,D,S intensity
};
uniform LightInfo Light;

struct MaterialInfo 
{
  vec3 Ka;      //Ambient reflectivity
  vec3 Kd;      //Diffuse reflectivity
  vec3 Ks;      //Specular reflectivity
  float Shininess;  //Specular Shininess factor
};
uniform MaterialInfo Material;

layout (location = 0) out vec4 FragColor;
layout(binding=0) uniform sampler2D Tex1;

float lumiance(vec3 color) 
{
    return dot(color.rgb, vec3(0.2126, 0.7152, 0.0722) );
}

vec3 phongModel(vec3 position, vec3 n)
{
    vec3 texColor = texture(Tex1, TexCoord).rgb;

    vec3 ambient = Light.La * texColor;
    vec3 s = normalize(Light.La - position);

    //Calculating the diffuse output
    float sDotN = max( dot(s,n), 0.0 );
    vec3 diffuse = Material.Kd * sDotN;

  //Calculating the specular output
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
    {
        vec3 v = normalize(-position.xyz);
        vec3 r = reflect(-s,n);
        spec = Material.Ks * pow( max( dot(r,v), 0.0 ), Material.Shininess );
    }

    return ambient + Light.L * (diffuse + spec);
}

subroutine (RenderPassType)
vec4 pass1()
{
    return vec4(phongModel(Position, Normal), 1);
}

subroutine (RenderPassType)
vec4 pass2()
{
    vec4 noise = texture(NoiseTex, TexCoord);
    vec4 color = texture(RenderTex, TexCoord);
    float green = lumiance(color.rgb);

    float dist1 = length(gl_FragCoord.xy - vec2(Width/4.0, Height/2.0));
    float dist2 = length(gl_FragCoord.xy - vec2(3.0 * Width/4.0, Height/2.0));
    if(dist1 > Radius && dist2 > Radius ) green = 0.0;

    return vec4(0.0, green * clamp(noise.a, 0.0, 1.0) , 0.0, 1.0);
}

void main()
{
    //This will call either pass1() or pass2()
    FragColor = RenderPass();
}