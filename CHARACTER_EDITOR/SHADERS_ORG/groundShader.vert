#version 460 core

vec4 vert[]={vec4(100.0f,0.0f,-100.0f,1.0f),vec4(100.0f,0.0f,100.0f,1.0f),vec4(-100.0f,0.0f,100.0f,1.0f),vec4(-100.0f,0.0f,-100.0f,1.0f)};

uniform mat4 projection;
uniform mat4 view;

void main(){
	vec4 pos;
	
	if(gl_VertexID<3)
		pos=vert[gl_VertexID];
	else {
		if(gl_VertexID==3) pos=vert[3];
		else if(gl_VertexID==4) pos=vert[0];
		else if(gl_VertexID==5) pos=vert[2];
	}

	gl_Position=projection*view*pos;
}