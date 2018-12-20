#version 460 core

layout (location=0) in vec3 pos;
layout (location=1) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int textureAvailable;

out vec2 fragTexCoord;
out flat int fragTextureAvailable;

void main(){
	vec4 position=vec4(pos,1.0);
	gl_Position=projection*view*model*position;
	
	fragTexCoord=texCoord;
	fragTextureAvailable=textureAvailable;
}