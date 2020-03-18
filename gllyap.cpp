#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080

#define OPENGL_MAJOR_VERSION 2
#define OPENGL_MINOR_VERSION 0

class Gllyap {
public:
	Gllyap() {};
	int init();
	void loop();
private:
	int winw, winh;
	SDL_Window *win;
	SDL_GLContext glcontext;
	GLuint tex[8];
	GLuint sp_lyap;
	int init_gl();
	void set_viewport();
	void draw();
	GLuint compile_shader(const char *shfname, GLenum shtype);
	GLuint build_sprogram(GLuint vshader, GLuint fshader);
};

GLuint Gllyap::compile_shader(const char *shfname, GLenum shtype) {
	// load the shader:
	std::ifstream t(shfname);
	std::stringstream txtbuf;
	txtbuf << t.rdbuf();
	std::string temp_str = txtbuf.str();
	const GLchar *text_str = temp_str.c_str();
	// compile the shader:
	GLuint shader = glCreateShader(shtype);
	glShaderSource(shader, 1, &text_str, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status != GL_TRUE) {
		char errbuf[10000];
		glGetShaderInfoLog(shader, 512, NULL, errbuf);
		std::cerr << "Error compiling " << shfname << errbuf <<
			std::endl;
		return 0;
	}
	return shader;
}

GLuint Gllyap::build_sprogram(GLuint vshader, GLuint fshader) {
	GLuint sprogram = glCreateProgram();
	// link
	glAttachShader(sprogram, vshader);
	glAttachShader(sprogram, fshader);
	glLinkProgram(sprogram);
	//glUseProgram(linen_shader);
	return sprogram;
}

int Gllyap::init_gl() {
	// enable smooth shading
	glShadeModel(GL_SMOOTH);
	// set the background red
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	// depth buffer setup
	glClearDepth(1.0f);
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
	// the type of depth test to do
	glDepthFunc(GL_LEQUAL);
	// really nice perspective calculations
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glGenTextures(8, tex);
	// load shaders:
	GLuint vshader = compile_shader(
		"box.glsl", GL_VERTEX_SHADER);
	GLuint lyap_shader = compile_shader(
		"lyap.glsl", GL_FRAGMENT_SHADER);
	sp_lyap = build_sprogram(vshader, lyap_shader);

	return 0;
}

int Gllyap::init() {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "There was an error initializing SDL2: " <<
			SDL_GetError() << std::endl;
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
						SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_MAJOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_MINOR_VERSION);

	winw = WIN_WIDTH;
	winh = WIN_HEIGHT;
	win = SDL_CreateWindow("GL Lyapunov",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		winw, winh, SDL_WINDOW_OPENGL);
	if(win == NULL) {
		std::cerr << "There was an error creating the window: " <<
			SDL_GetError() << std::endl;
		return 1;
	}

	glcontext = SDL_GL_CreateContext(win);
	if(glcontext == NULL) {
		std::cerr << "There was an error creating OpenGL context: " <<
			SDL_GetError() << std::endl;
		return 1;
	}

	glewExperimental = GL_TRUE;
	glewInit();
	SDL_GL_MakeCurrent(win, glcontext);
	init_gl();
	set_viewport();
	return 0;
}

void Gllyap::loop() {
	SDL_Event e;
	bool quit = false;
	bool dirty = false;
	draw();
	while(1) {
		// process events:
		while(SDL_PollEvent(&e) != 0) {
			if(e.type == SDL_QUIT) {
				quit = true;
			}
			if(e.type == SDL_WINDOWEVENT &&
			(e.window.event ==SDL_WINDOWEVENT_RESIZED ||
			e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
				winw = e.window.data1;
				winh = e.window.data2;
				set_viewport();
			}
			dirty = true;
		}
		if(quit) break;
		if(dirty) {
			draw();
			dirty = false;
		}
		SDL_Delay(100);
	}
	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
}

// reset viewport after a window resize:
void Gllyap::set_viewport() {
	winh = (winh < 1)? 1: winh;
//	GLfloat ratio = GLfloat(winw) / GLfloat(winh);
	glViewport(0, 0, (GLsizei)winw, (GLsizei)winh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// set an orthogonal projection:
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	// make sure we are changing the model view matrix and not projection:
	glMatrixMode(GL_MODELVIEW);
	// reset the view:
	glLoadIdentity();
}

void Gllyap::draw() {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	static const GLfloat vertices[][3] = {
		{ 0.0,  0.0,  0.0},
		{ 1.0,  0.0,  0.0},
		{ 1.0,  1.0,  0.0},
		{ 0.0,  1.0,  0.0}
	};
	static const GLfloat texCoords[] = {
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0
	};

	glLoadIdentity();
	//glTranslatef(0.0, 0.0, -3.0);

	glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	glBindTexture(GL_TEXTURE_2D, tex[1]);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
	// pass window size to the fragment shader:
	GLint bbox_param = glGetUniformLocation(sp_lyap, "bbox");
	glUniform2f(bbox_param, winw, winh);
	glUseProgram(sp_lyap);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	SDL_GL_SwapWindow(win);
	return;
}


int main(int argc, char *argv[]) {
	Gllyap lyap;
	int err = lyap.init();
	if(err) {
		return err;
	}
	lyap.loop();
	return 0;
}

