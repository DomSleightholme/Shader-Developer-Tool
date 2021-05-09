#version 460

//declare the primitives for the geometry shader
layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

out vec3 GNormal; //Normal used by Fragment Shader
out vec3 GPosition; //Position used for Fragment Shader

noperspective out vec3 GEdgeDistance; //the distance from the edge

in vec3 VNormal[];
in vec3 VPosition[];

uniform mat4 ViewportMatrix;

void main()
{
	//Transform each vertex into the viewport space from clip coorinates
	//divivde by the w coordinate as the clip coorinates are humogenous and need to be converted back to true Cartesian coordinates
	vec2 p0 = vec2(ViewportMatrix * (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
	vec2 p1 = vec2(ViewportMatrix * (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
	vec2 p2 = vec2(ViewportMatrix * (gl_in[2].gl_Position / gl_in[2].gl_Position.w));

	//calculate the edges of the triangle
	float a = length(p1 - p2);
	float b = length(p2 - p0);
	float c = length(p1 - p0);

	//calculate the angles
	float alpha = acos( (b*b + c*c - a*a) / (2.0*b*c) );
	float beta = acos( (a*a + c*c - b*b) / (2.0*a*c) );

	//calculate the distances
	float ha = abs( c * sin( beta ) );
	float hb = abs( c * sin( alpha ) );
	float hc = abs( b * sin( alpha ) );

	//pase the distance and position of the new vertex to the fragment
	//also pass the normal, position of the mesh
	GEdgeDistance = vec3( ha, 0, 0);
	GNormal = VNormal[0];
	GPosition = VPosition[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	GEdgeDistance = vec3( 0, hb, 0);
	GNormal = VNormal[1];
	GPosition = VPosition[1];
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	GEdgeDistance = vec3( 0, 0, hc);
	GNormal = VNormal[2];
	GPosition = VPosition[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}