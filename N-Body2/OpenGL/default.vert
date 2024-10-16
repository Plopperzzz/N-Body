#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in float aRad;

out vec4 color;

uniform mat4 camMatrix;
uniform float radius;

void main()
{
	gl_Position = camMatrix * vec4(aPos, 0.0, 1.0);
	gl_PointSize = 7.5 * aRad / radius;

	color = aColor;
};
