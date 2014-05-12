#include "shadermanager.h"

void ShaderManager::add(std::string key, std::string filename) {
	GLuint program, fragment;
	program = glCreateProgram();
	if (HasGLSLSupport()) {	
		fragment = CreateAndCompileShader(filename.c_str(), GL_FRAGMENT_SHADER);
		glAttachShader(program, fragment);
	}
	glLinkProgram(program);
	glDeleteShader(fragment);

	_c.insert(make_pair(key, program));
}

GLuint ShaderManager::find(std::string key) {
	std::map<std::string, GLuint>::const_iterator it = _c.find(key);

	if (it == _c.end())
		return 0;

	return it->second;
}
