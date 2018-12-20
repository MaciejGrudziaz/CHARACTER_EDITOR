#version 460 core

struct JointPos{
	float x;
	float y;
	float z;
	float idx;
};

layout(binding=0,std430) buffer hitboxTransformedVertices{
	JointPos hitboxVertices[];
};

uniform int hitboxJointIdx;
uniform vec3 localAxis[3];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 fragColor;

void main(){
	vec4 center=vec4(0.0f);

//	int currHitboxVerticesPos=0;
//	int currJointIdx=int(hitboxVertices[0].idx);
//	while (currJointIdx!=hitboxJointIdx){
//		currHitboxVerticesPos+=8;
//		currJointIdx=int(hitboxVertices[currJointIdx].idx);
//	}
//	for (int i=currHitboxVerticesPos;i<currHitboxVerticesPos+8;++i){
//		vec4 hitboxPt=vec4(hitboxVertices[i].x,hitboxVertices[i].y,hitboxVertices[i].z,1.0f);
//		center+=hitboxPt;
//	}
//	center/=8.0f;

	vec4 hitboxPt;
	for(int i=0;i<8;++i){
		hitboxPt=vec4(hitboxVertices[i].x,hitboxVertices[i].y,hitboxVertices[i].z,1.0f);
		center+=hitboxPt;
	}
	center/=8.0f;

	vec4 axisPt;
	if(gl_VertexID%2==0)
		gl_Position=projection*view*center;
	else{
		if(gl_VertexID==1){
			axisPt=center+0.5f*vec4(localAxis[0],0.0f);
			gl_Position=projection*view*model*axisPt;
		}
		if(gl_VertexID==3){
			axisPt=center+0.5f*vec4(localAxis[1],0.0f);
			gl_Position=projection*view*model*axisPt;
		}
		if(gl_VertexID==5){
			axisPt=center+0.5f*vec4(localAxis[2],0.0f);
			gl_Position=projection*view*model*axisPt;
		}
	}

	if(gl_VertexID/2==0) fragColor=vec4(1.0f,0.0f,0.0f,1.0f);
	if(gl_VertexID/2==1) fragColor=vec4(0.0f,1.0f,0.0f,1.0f);
	if(gl_VertexID/2==2) fragColor=vec4(0.0f,0.0f,1.0f,1.0f);
}