// gridline.vs
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexUV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 fragmentPosition;

void main()
{
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1);
    fragmentPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));
}
