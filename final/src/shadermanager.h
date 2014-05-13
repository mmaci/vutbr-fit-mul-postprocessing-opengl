#pragma once
#include <map>
#include <string>
#include <GL/glew.h>
#include "shaders.h"
class ShaderManager
{
public:
	void add(std::string key, std::string filename);
	GLuint find(std::string key);

private:
	std::map<std::string, GLuint> _c;
};

