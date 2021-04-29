#version 460

//Variables passed in from the Vertex Shader and used for the frag methods
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Vec;

//FragColor used for the overall out of the program, the texture data is passed in from the loaded textures
layout (location = 0) out vec4 FragColor;
layout(binding=0) uniform sampler2D Tex1;

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

vec3 blinnPhong(vec3 position, vec3 n)
{
    vec3 texColor = texture(Tex1, TexCoord).rgb;

    //Calculate the overall Ambient using the light ambient value and the combined texture value, col
    vec3 ambient = light.La * texColor;
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

void main()
{
    //SpotLight Shading Data passed in FragColor to produce the Spotlight and output Textures
     FragColor = vec4(blinnPhong(Position, normalize(Normal)),1);
}