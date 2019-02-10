#version 460 core

layout (location=0) in vec3 pos;
layout (location=1) in float idx;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 color1;
uniform vec3 color2;
uniform int setJointIdx;

out vec4 fragColor;

layout(binding=0,std430) buffer prevJointsBuffer{
	mat4 prevJoints[];
};

layout(binding=1,std430) buffer nextJointsBuffer{
	mat4 nextJoints[];
};

uniform float interpolation;

void main(){
	vec4 currPos;

	int jointIdx=int(idx);

	if(jointIdx>=0){
		vec4 prevPos=prevJoints[jointIdx]*vec4(pos,1.0f);
		vec4 nextPos=nextJoints[jointIdx]*vec4(pos,1.0f);
		currPos=prevPos+interpolation*(nextPos-prevPos);
	}
	else currPos=vec4(pos,1.0f);

	vec4 position=projection*view*model*currPos;
	gl_Position=position;
	
	if(jointIdx!=setJointIdx)
		fragColor=vec4(color1,1.0f);
	else fragColor=vec4(color2,1.0f);
}