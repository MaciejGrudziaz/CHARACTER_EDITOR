#version 460 core

layout (location=0) in vec3 pos;
layout (location=1) in vec2 texCoord;
//jointIdxWeightPairX -> [jointIdx, verticeWeight]
layout (location=2) in vec2 jointIdxWeightPair0;
layout (location=3) in vec2 jointIdxWeightPair1;
layout (location=4) in vec2 jointIdxWeightPair2;
layout (location=5) in vec2 jointIdxWeightPair3;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout(binding=0,std430) buffer prevJointsBuffer{
	mat4 prevJoints[];
};

layout(binding=1,std430) buffer nextJointsBuffer{
	mat4 nextJoints[];
};

uniform float interpolation;

uniform int textureAvailable;

out vec2 fragTexCoord;
out flat int fragTextureAvailable;

void main() {
	vec4 posV4=vec4(pos,1.0f);
	mat4 prevMat=(jointIdxWeightPair0[1]*prevJoints[int(jointIdxWeightPair0[0])])+(jointIdxWeightPair1[1]*prevJoints[int(jointIdxWeightPair1[0])])+
	(jointIdxWeightPair2[1]*prevJoints[int(jointIdxWeightPair2[0])])+(jointIdxWeightPair3[1]*prevJoints[int(jointIdxWeightPair3[0])]);
	vec4 prevVertexVal=prevMat*posV4;

	mat4 nextMat=(jointIdxWeightPair0[1]*nextJoints[int(jointIdxWeightPair0[0])])+(jointIdxWeightPair1[1]*nextJoints[int(jointIdxWeightPair1[0])])+
	(jointIdxWeightPair2[1]*nextJoints[int(jointIdxWeightPair2[0])])+(jointIdxWeightPair3[1]*nextJoints[int(jointIdxWeightPair3[0])]);
	vec4 nextVertexVal=nextMat*posV4;

	vec4 vertexVal=prevVertexVal+interpolation*(nextVertexVal-prevVertexVal);

	gl_Position=projection*view*model*vertexVal;
	fragTexCoord=texCoord;

	fragTextureAvailable=textureAvailable;
}