#include <GL/glew.h>
#include <glm.hpp>
#include <glut.h>
#include <cstdint>

#include <iostream> // Allow us to print to the console

#include "shaders.h"


GLuint fbo; // The frame buffer object
GLuint fboDepth; // The depth buffer for the frame buffer object
GLuint fboTexture; // The texture object to write our frame buffer object to

const uint16_t WINDOW_WIDTH = 500; // The width of our window
const uint16_t WINDOW_HEIGHT = 500; // The height of our window

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };
 
const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f }; 

float rotation = 0.0f; // The angle of rotation in degrees for our teapot

GLuint program, fragment;

void initFBDepthBuffer(void) {
	// generate buffer and bind it
	glGenRenderbuffersEXT(1, &fboDepth);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, fboDepth);
		
	// attach the buffer
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fboDepth);

	// unbind
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}

void initFBTexture(void) {
	// generate texture and bind it
	glGenTextures(1, &fboTexture);
	glBindTexture(GL_TEXTURE_2D, fboTexture); // Bind the texture fboTexture

	// setup texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	// create the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);	

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);
}

uint8_t initFB(void) {

	initFBDepthBuffer();
	initFBTexture();

	// generate framebuffer and bind it
	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo); 

	// attach the texture to the frame buffer
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fboTexture, 0);
	// attach the depth buffer to the frame buffer
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fboDepth);


	if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT) {
		std::cerr << "ERROR: failed to init frame buffer." << std::endl;
		return EXIT_FAILURE;
	}

	// unbind
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); 
}

void init(void) {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);		

	initFB();
}

void renderTeapotSceneToTexture(GLuint texture) {
	
	// perspective view
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, static_cast<float>(WINDOW_WIDTH)/WINDOW_HEIGHT, 1.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);

	// bind framebuffer
	// everything is now rendered not to screen but to a framebuffer, which is bound to a texture
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture, 0);

	// and now draw the teapot!
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth and colour buffers	
	glPushMatrix();
	{		
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);				
		glTranslatef(0.0f, 0.0f, -5.0f);
		glRotatef(rotation, 1.0f, 1.0f, 1.0f);

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
				
		glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
 
		glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess); 

		glColor3f(0.8, 0.2, 0.1);				
		glutSolidTeapot(1.5f);
	
	}
	glPopMatrix();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // Unbind our texture

	rotation += 0.25f;
	if (rotation > 360.0f)
		rotation = 0.0f;
}


void drawTexture(GLuint texture)
{
	// generate mipmap
	glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_2D, texture);
	glGenerateMipmapEXT(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// reset matrices
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// draw the texture as a fullscreen QUAD
	// without lighting!
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, texture);

	glBegin(GL_QUADS); 
	{ 
		glColor4f(1.0 ,1.0 ,1.0 ,1.0);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 0.0f); 
	} 
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

void display (void) {

	// render the teapot inside a texture
	renderTeapotSceneToTexture(fboTexture); // Render our teapot scene into our frame buffer
	
	// load postprocess shader
	glUseProgram(program);
	// glUniform1i(glGetUniformLocation(program, "mode"), 1);
	
	// draw the texture	
	drawTexture(fboTexture);

	glUseProgram(0);	
	glutSwapBuffers();
}

void reshape (int width, int height) {
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 100.0); 
	glMatrixMode(GL_MODELVIEW);
}


int main (int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize (WINDOW_WIDTH, WINDOW_HEIGHT);	
	glutCreateWindow ("Postprocessing");

	if (GLEW_OK != glewInit()) {
		std::cerr << "Couldn't initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// compile shader
	program = glCreateProgram();
	if (HasGLSLSupport()) {	
		fragment = CreateAndCompileShader("blur.glsl", GL_FRAGMENT_SHADER);//nacte dany soubor s kodem shaderu
		glAttachShader(program, fragment); // jednotlive shadery priradi k programu
	}
	glLinkProgram(program);

	init();

	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
}
