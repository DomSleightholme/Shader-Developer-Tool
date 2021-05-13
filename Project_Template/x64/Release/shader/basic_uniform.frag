#version 460

//Variables passed in from the Vertex Shader and used for the frag methods
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Vec;

in vec3 LightDir;
in vec3 ViewDir;

//FragColor used for the overall out of the program, the texture data is passed in from the loaded textures
layout (location = 0) out vec4 FragColor;
layout(binding=0) uniform sampler2D Tex1;
layout(binding=1) uniform sampler2D NormalMapTex;

//Material variables used to calculate the shine and reflect on the Mesh
uniform struct MaterialInfo 
{
  vec3 Ka; 
  vec3 Kd; 
  vec3 Ks; 
  float Shininess; 
} Material;

//Light variables used to calculate the position of the light, the ambient, diffsue and specular of the light
uniform struct LightInfo 
{
  vec4 Position; 
  vec3 La;       
  vec3 L;       
} light;

//Spotlight variables used to calculate Spotlight position, direction and size
 uniform struct SpotLightInfo {
  vec3 Position; 
  vec3 L; 
  vec3 La; 
  vec3 Direction; 
  float Exponent; 
  float Cutoff; 
} Spot;

uniform int ShaderIndex;

vec3 blinnPhong(vec3 position, vec3 n)
{
    vec3 texColor = texture(Tex1, TexCoord).rgb;

    vec3 norm = texture(NormalMapTex, TexCoord).xyz; 
    norm.xy = 2.0 * norm.xy - 1.0;

    //Calculate the overall Ambient using the light ambient value and the combined texture value, col
    vec3 ambient = light.La * Material.Ka * texColor;
	vec3 s = normalize(light.L -position);

    //Calculating the diffuse output
    float sDotN = max( dot(s,n), 0.0 );
    vec3 diffuse = Material.Kd * sDotN;

    //Calculating the specular output
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
    {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        spec = Material.Ks * pow( max( dot(h,n), 0.0 ), Material.Shininess );
    }

    return ambient + light.L * (diffuse + spec);
}

vec3 blinnPhongSpot(vec3 position, vec3 n)
{
    vec3 texColor = texture(Tex1, TexCoord).rgb;

    //Calculate the overall Ambient using Spot Ambient value, Material Ambient value and the texture value
    vec3 ambient = Spot.La * Material.Ka * texColor;
    vec3 s = normalize(Spot.L -position);

    //Calculate the SpotLight angle using the direction and the dot method
    float cosAng = dot(-s, normalize(Spot.Direction));
    float angle = acos(cosAng);

    //Initliase varibles used in the following calculations
    float spotScale = 0.0f;
    vec3 spec = vec3(0.0f);  
    vec3 diffuse = vec3(0.0f);

    if(angle < Spot.Cutoff)
    {
        spotScale = pow(cosAng, Spot.Exponent);
        float sDotN = max( dot(s,n), 0.0 );
        diffuse = Spot.L * Material.Kd * sDotN;    

        //Calculating the specular output
        if( sDotN > 0.0 )
        {
            vec3 v = normalize(-position.xyz);
            vec3 h = normalize(v + s);
            spec = Material.Ks * pow( max( dot(h,n), 0.0 ), Material.Shininess );
        }
    }

    return ambient + spotScale * Spot.L * (diffuse + spec);
}

void main()
{
    //SpotLight Shading Data passed in FragColor to produce the Spotlight and output Textures
    if(ShaderIndex == 0)  FragColor = vec4(blinnPhong(Position, normalize(Normal)), 1);
    if(ShaderIndex == 1)  FragColor = vec4(blinnPhongSpot(Position, normalize(Normal)), 1);
}