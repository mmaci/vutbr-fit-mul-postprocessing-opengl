#include <GL/glew.h>
#include <glm.hpp>
#include <glut.h>

#include <iostream>
#include <fstream>

//inspirovano http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/loading.php
unsigned long getFileLength(std::ifstream& file);
int loadShader(char* filename, GLchar** ShaderSource, unsigned long* len);
int unloadShader(GLubyte** ShaderSource);
GLuint CreateAndCompileShader(char* filename, GLuint shaderType);
bool HasGLSLSupport(void);

void CheckFramebufferStatus();