// gridline.fs
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core

out vec4 color;

in vec3 fragmentPosition;
uniform vec3 cameraPosition;

void main(void)
{
	if(fract(fragmentPosition.x) < 0.005 || fract(fragmentPosition.x) > 0.995
		|| fract(fragmentPosition.z) < 0.005 || fract(fragmentPosition.z) > 0.995)
	{
		color = vec4(vec3(.6), smoothstep(-50, 0, -distance(cameraPosition, fragmentPosition)));
	}
	else
	{
		// color = vec4(vec3(0), 0);
		discard;
	}
}
