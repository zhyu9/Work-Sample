#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

uniform mat4 modelTransformMatrix;
uniform mat4 MVP;

out vec3 theColor;

void main()
{
	vec4 v = vec4(position, 1.0);
	vec4 new_position = MVP * modelTransformMatrix * v;
	gl_Position = new_position;	
	theColor = vertexColor;
}