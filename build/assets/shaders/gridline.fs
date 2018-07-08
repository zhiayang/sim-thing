// gridline.fs
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core

out vec4 color;

in vec3 fragmentPosition;
uniform vec3 cameraPosition;

void main(void)
{
	if(fract(fragmentPosition.x) < 0.01 || fract(fragmentPosition.x) > 0.99
		|| fract(fragmentPosition.z) < 0.01 || fract(fragmentPosition.z) > 0.99)
	{
		color = vec4(vec3(1), smoothstep(-50, 0, -distance(cameraPosition, fragmentPosition)));
	}
	else
	{
		color = vec4(0);
	}
}
