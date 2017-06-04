// GLWrapper.h
// Copyright (c) 2014 - 2017, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
// #include <GLUT/glut.h>

namespace GL
{
	void pushTextureBinding(int id);
	int popTextureBinding();
}
