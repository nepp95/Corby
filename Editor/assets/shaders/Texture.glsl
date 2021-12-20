#type vertex
#version 450 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in float a_texIndex;
layout(location = 4) in float a_tilingFactor;
layout(location = 5) in int a_entityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_viewProjection;
};

struct VertexOutput
{
	vec4 color;
	vec2 texCoord;
	float texIndex;
	float tilingFactor;
};

layout(location = 0) out VertexOutput Output;
layout(location = 4) out flat int v_entityID;

void main()
{
	Output.color = a_color;
	Output.texCoord = a_texCoord;
	Output.texIndex = a_texIndex;
	Output.tilingFactor = a_tilingFactor;
	v_entityID = a_entityID;
	gl_Position = u_viewProjection * vec4(a_position, 1.0f);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;
 
struct VertexOutput
{
	vec4 color;
	vec2 texCoord;
	float texIndex;
	float tilingFactor;
};

layout(location = 0) in VertexOutput Input;
layout(location = 4) in flat int v_entityID;

layout(binding = 0) uniform sampler2D u_textures[32];

void main()
{
	vec4 texColor = Input.color;

	switch(int(Input.texIndex))
	{
		case  0: texColor *= texture(u_textures[ 0], Input.texCoord * Input.tilingFactor); break;
		case  1: texColor *= texture(u_textures[ 1], Input.texCoord * Input.tilingFactor); break;
		case  2: texColor *= texture(u_textures[ 2], Input.texCoord * Input.tilingFactor); break;
		case  3: texColor *= texture(u_textures[ 3], Input.texCoord * Input.tilingFactor); break;
		case  4: texColor *= texture(u_textures[ 4], Input.texCoord * Input.tilingFactor); break;
		case  5: texColor *= texture(u_textures[ 5], Input.texCoord * Input.tilingFactor); break;
		case  6: texColor *= texture(u_textures[ 6], Input.texCoord * Input.tilingFactor); break;
		case  7: texColor *= texture(u_textures[ 7], Input.texCoord * Input.tilingFactor); break;
		case  8: texColor *= texture(u_textures[ 8], Input.texCoord * Input.tilingFactor); break;
		case  9: texColor *= texture(u_textures[ 9], Input.texCoord * Input.tilingFactor); break;
		case 10: texColor *= texture(u_textures[10], Input.texCoord * Input.tilingFactor); break;
		case 11: texColor *= texture(u_textures[11], Input.texCoord * Input.tilingFactor); break;
		case 12: texColor *= texture(u_textures[12], Input.texCoord * Input.tilingFactor); break;
		case 13: texColor *= texture(u_textures[13], Input.texCoord * Input.tilingFactor); break;
		case 14: texColor *= texture(u_textures[14], Input.texCoord * Input.tilingFactor); break;
		case 15: texColor *= texture(u_textures[15], Input.texCoord * Input.tilingFactor); break;
		case 16: texColor *= texture(u_textures[16], Input.texCoord * Input.tilingFactor); break;
		case 17: texColor *= texture(u_textures[17], Input.texCoord * Input.tilingFactor); break;
		case 18: texColor *= texture(u_textures[18], Input.texCoord * Input.tilingFactor); break;
		case 19: texColor *= texture(u_textures[19], Input.texCoord * Input.tilingFactor); break;
		case 20: texColor *= texture(u_textures[20], Input.texCoord * Input.tilingFactor); break;
		case 21: texColor *= texture(u_textures[21], Input.texCoord * Input.tilingFactor); break;
		case 22: texColor *= texture(u_textures[22], Input.texCoord * Input.tilingFactor); break;
		case 23: texColor *= texture(u_textures[23], Input.texCoord * Input.tilingFactor); break;
		case 24: texColor *= texture(u_textures[24], Input.texCoord * Input.tilingFactor); break;
		case 25: texColor *= texture(u_textures[25], Input.texCoord * Input.tilingFactor); break;
		case 26: texColor *= texture(u_textures[26], Input.texCoord * Input.tilingFactor); break;
		case 27: texColor *= texture(u_textures[27], Input.texCoord * Input.tilingFactor); break;
		case 28: texColor *= texture(u_textures[28], Input.texCoord * Input.tilingFactor); break;
		case 29: texColor *= texture(u_textures[29], Input.texCoord * Input.tilingFactor); break;
		case 30: texColor *= texture(u_textures[30], Input.texCoord * Input.tilingFactor); break;
		case 31: texColor *= texture(u_textures[31], Input.texCoord * Input.tilingFactor); break;
	}

	color = texColor;
	color2 = v_entityID;
}
