#version 460 core

vec4 axis[]={vec4(10.0f,0.0f,0.0f,0.0f),vec4(0.0f,10.0f,0.0f,0.0f),vec4(0.0f,0.0f,10.0f,0.0f)};

uniform mat4 view;
uniform mat4 projection;
uniform vec4 begin;

out vec4 color;

void main(){
	
	int axisIdx=gl_VertexID/2;
	int pointIdx=gl_VertexID%2;

	if(pointIdx==0) gl_Position=projection*view*begin;
	else gl_Position=projection*view*(axis[axisIdx]+begin);

	color=axis[axisIdx];
}