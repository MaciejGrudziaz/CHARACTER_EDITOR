#version 430 core

struct JointPos{
	float x;
	float y;
	float z;
	float idx;
};

layout(binding=0,std430) buffer hitboxBasicVertices{
	JointPos hitboxVertices[8];
};

layout(binding=1,std430) buffer prevJointsBuffer{
	mat4 prevJoints[];
};

layout(binding=2,std430) buffer nextJointsBuffer{
	mat4 nextJoints[];
};

layout (location=0) uniform int hitboxJointIdx=1;
layout (location=1) uniform vec3 localAxis[3];

layout (location=4) uniform mat4 model;
layout (location=5) uniform mat4 view;
layout (location=6) uniform mat4 projection;

layout (location=7) uniform float interpolation;

out vec4 fragColor;

void main(){
	vec4 center=vec4(0.0f);

	vec4 hitboxPt;
	for(int i=0;i<8;++i){
		hitboxPt=vec4(hitboxVertices[i].x,hitboxVertices[i].y,hitboxVertices[i].z,1.0f);
		center+=hitboxPt;
	}
	center/=8.0f;

	vec4 prevPt,nextPt,pt;
	vec4 axis;

	if(gl_VertexID%2==0){
		if(hitboxJointIdx>=0){
			prevPt=prevJoints[hitboxJointIdx]*center;
			nextPt=nextJoints[hitboxJointIdx]*center;
			pt=prevPt+interpolation*(nextPt-prevPt);
		}
		else pt=center;
		gl_Position=projection*view*model*pt;
	}
	else{
		if(gl_VertexID==1){
			axis=vec4(localAxis[0],0.0f);
			pt=center+axis;
			if(hitboxJointIdx>=0){
				prevPt=prevJoints[hitboxJointIdx]*pt;
				nextPt=nextJoints[hitboxJointIdx]*pt;
				pt=prevPt+interpolation*(nextPt-prevPt);
			}
			gl_Position=projection*view*model*pt;
		}
		if(gl_VertexID==3){
			axis=vec4(localAxis[1],0.0f);
			pt=center+axis;
			if(hitboxJointIdx>=0){
				prevPt=prevJoints[hitboxJointIdx]*pt;
				nextPt=nextJoints[hitboxJointIdx]*pt;
				pt=prevPt+interpolation*(nextPt-prevPt);
			}
			gl_Position=projection*view*model*pt;
		}
		if(gl_VertexID==5){
			axis=vec4(localAxis[2],0.0f);
			pt=center+axis;
			if(hitboxJointIdx>=0){
				prevPt=prevJoints[hitboxJointIdx]*pt;
				nextPt=nextJoints[hitboxJointIdx]*pt;
				pt=prevPt+interpolation*(nextPt-prevPt);
			}
			gl_Position=projection*view*model*pt;
		}
	}

	if(gl_VertexID/2==0) fragColor=vec4(1.0f,0.0f,0.0f,1.0f);
	if(gl_VertexID/2==1) fragColor=vec4(0.0f,1.0f,0.0f,1.0f);
	if(gl_VertexID/2==2) fragColor=vec4(0.0f,0.0f,1.0f,1.0f);
}
