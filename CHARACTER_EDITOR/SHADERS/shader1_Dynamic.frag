#version 460 core

in vec2 fragTexCoord;
in flat int fragTextureAvailable;

uniform sampler2D gameTex;

out vec4 fragColor;

void main(){
	if(fragTextureAvailable==1) fragColor=texture(gameTex,fragTexCoord);
	else fragColor=vec4(0.67f,0.67f,0.67f,1.0f);
}