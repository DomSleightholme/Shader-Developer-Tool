#version 460

struct LightInfo {
	vec4 Position; // Light Position in eye coords
	vec3 Intensity; // A,D,S Intensity
};
uniform LightInfo Light;

struct MaterialInfo{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shininess;
};
uniform MaterialInfo Material;

//Line Struct for rendering the wireframe
uniform struct LineInfo {
	float width;
	vec4 Color;
} Line;

//In variables coming from geometry shader
in vec3 GPosition;
in vec3 GNormal;
noperspective in vec3 GEdgeDistance;

//out colour
layout( location = 0 ) out vec4 FragColor;

vec3 phongModel(vec3 position, vec3 n)
{
	 //Calculate the overall Ambient using the light ambient value and the combined texture value, col
    vec3 ambient = Light.Intensity;
	vec3 s = normalize(vec3(Light.Position) - position);

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

    return ambient * Light.Intensity + (diffuse + spec);
}

void main()
{
	//calculate Color
	vec4 color = vec4( phongModel(GPosition, GNormal), 1.0 );

	//Find the smallest distance for the fragment
	float d = min( GEdgeDistance.x, GEdgeDistance.y);
	d = min( d, GEdgeDistance.z);

	float mixVal;
	if( d < Line.width - 1)
	{
		mixVal = 1.0f;
	}
	else if( d > Line.width + 1)
	{
		mixVal = 0.0f;
	}
	else
	{
		float x = d - (Line.width - 1);
		mixVal = exp2(-2.0 * (x*x));
	}

	FragColor = mix( color, Line.Color, mixVal );

}