
#include "shaders.h"

unsigned long getFileLength(std::ifstream& file)
{
    if(!file.good()) return 0;
    
    unsigned long pos=file.tellg();
    file.seekg(0,std::ios::end);
    unsigned long len = file.tellg();
    file.seekg(std::ios::beg);
    
    return len;
}

void CheckFramebufferStatus()
{
    GLenum status;
    status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            printf("Unsupported framebuffer format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            printf("Framebuffer incomplete, missing attachment\n");
            break;
        /*case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
            printf("Framebuffer incomplete, duplicate attachment\n");
            break;*/
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            printf("Framebuffer incomplete, attached images must have same dimensions\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            printf("Framebuffer incomplete, attached images must have same format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            printf("Framebuffer incomplete, missing draw buffer\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            printf("Framebuffer incomplete, missing read buffer\n");
            break;
        default:
            assert(0);
    }
}

int loadShader(char* filename, GLchar** ShaderSource, unsigned long* len)
{
   std::ifstream file;
   file.open(filename, std::ios::in); // opens as ASCII!
   if(!file) return -1;
  // unsigned long temp = getFileLength(file);
   *len = getFileLength(file);
    
   if (*len==0) return -2;   // Error: Empty File 
    
   *ShaderSource = (GLchar*) new char[*len+1];
   if (*ShaderSource == NULL) return -3;   // can't reserve memory
   
    // len isn't always strlen cause some characters are stripped in ascii read...
    // it is important to 0-terminate the real length later, len is just max possible value... 
   (*ShaderSource)[*len-1] = 0; 

   unsigned int i=0;
   while (file.good())
   {
       (*ShaderSource)[i] = (GLchar) file.get();       // get character from file.
       if (!file.eof())
        i++;
   }
    
   (*ShaderSource)[i] = 0;  // 0-terminate it at the correct position
    
   file.close();
      
   return 0; // No Error
}


int unloadShader(GLubyte** ShaderSource)
{
   if (*ShaderSource != 0)
     delete[] *ShaderSource;
   *ShaderSource = 0;
   return 0;
}

GLuint CreateAndCompileShader(char* filename, GLuint shaderType)
{
	GLuint fragmentShader = glCreateShader(shaderType);
	GLchar * fragmentSource;
	unsigned long len;
	loadShader(filename, &fragmentSource, &len);
	const GLint flen= len;
	const GLcharARB * a = fragmentSource;
	glShaderSourceARB(fragmentShader, 1, &a, &flen);
	glCompileShaderARB(fragmentShader);

	GLint compiled;

	glGetProgramiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLint blen = 0;	
		GLsizei slen = 0;

		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH , &blen);       
		if (blen > 1)
		{
			GLchar* compiler_log = (GLchar*)malloc(blen);
			glGetInfoLogARB(fragmentShader, blen, &slen, compiler_log);
			std::cerr << "compiler_log:\n" << compiler_log;
			free (compiler_log);
		}
	}  
	return fragmentShader;


}
using namespace std;

bool HasGLSLSupport(void)
   {
	   bool useGLSL = true;
      
      if (GLEW_VERSION_2_0)
      {
         cout << "OpenGL 2.0 (or higher) is available!" << endl;
      }
      else if (GLEW_VERSION_1_5)
      {
         cout << "OpenGL 1.5 core functions are available" << endl;
      }
      else if (GLEW_VERSION_1_4)
      {
         cout << "OpenGL 1.4 core functions are available" << endl;
      }
      else if (GLEW_VERSION_1_3)
      {
         cout << "OpenGL 1.3 core functions are available" << endl;
      }
      else if (GLEW_VERSION_1_2)
      {
         cout << "OpenGL 1.2 core functions are available" << endl;
      }

      if (GL_TRUE != glewGetExtension("GL_ARB_fragment_shader"))
      {
          cout << "[WARNING] GL_ARB_fragment_shader extension is not available!\n";
          useGLSL = false;
      }

      if (GL_TRUE != glewGetExtension("GL_ARB_vertex_shader"))
      {
          cout << "[WARNING] GL_ARB_vertex_shader extension is not available!\n";
          useGLSL = false;
      }

      if (GL_TRUE != glewGetExtension("GL_ARB_shader_objects"))
      {
          cout << "[WARNING] GL_ARB_shader_objects extension is not available!\n";
          useGLSL = false;
      }

      if (useGLSL)
      {
          cout << "[OK] OpenGL Shading Language is available!\n\n";
      }
      else
      {
          cout << "[FAILED] OpenGL Shading Language is not available...\n\n";
      }   
     
      return useGLSL;
}
