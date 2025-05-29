#ifdef __WIN32__
	#define SDL_MAIN_HANDLED
#endif

#define UTIL_IMPL
#define SOKOL_IMPL
#define SOKOL_GLCORE33

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <stdio.h>
#include <stdbool.h>

#include "util/util.h"
#include "font.h"

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720
#define WINDOW_RES_X 320
#define WINDOW_RES_Y 180

typedef struct s_zombiegame {
	SDL_Window		*sdlwindow;
	SDL_GLContext	context;
	input_t			input;
	sprite_atlas_t	font_atlas;
	m4				proj;
	m4				view;
	window_t		window;
	bool			running;
}	t_zombiegame;

int main() {
	t_zombiegame game;

	ASSERT(!SDL_Init(SDL_INIT_VIDEO),
		"Failed to init SDL: %s", SDL_GetError());

	SDL_version	compiled;
	SDL_version	linked;
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	printf("Compiled SDL Version: %d.%d.%d\n"
		"Linked SDL Version: %d.%d.%d\n",
		compiled.major, compiled.minor, compiled.patch,
		linked.major, linked.minor, linked.patch);

	game.window.window_size = v2i_of(WINDOW_WIDTH, WINDOW_HEIGHT);
	game.sdlwindow = 
		SDL_CreateWindow("Zombie Game",
			SDL_WINDOWPOS_CENTERED_DISPLAY(1), SDL_WINDOWPOS_CENTERED_DISPLAY(1),
			game.window.window_size.x, game.window.window_size.y,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	ASSERT(game.sdlwindow,
		"Failed to create SDL Window: %s", SDL_GetError());

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	game.context = SDL_GL_CreateContext(game.sdlwindow);
	ASSERT(game.context,
		"Failed to create GL Context %s", SDL_GetError());
	SDL_GL_MakeCurrent(game.sdlwindow, game.context);

	ASSERT(glewInit() == GLEW_OK,
		"failed to load glew");

	printf("OpengGL Version: {%s}\nGLSL Version: {%s}\n",
		glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));

	// See use
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);

	// Debug message
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(gfx_glcallback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);

	// Loading composents
	window_init(
		&game.window, 
		game.window.window_size, 
		v2i_of(WINDOW_RES_X, WINDOW_RES_Y), 
		WINDOW_BUF_COLOR);
	input_init(&game.input);
	sprite_init();
	sprite_atlas_init(&game.font_atlas, "res/sprites/font.png", v2i_of(8));

	sprite_batch_t font_batch;
	sprite_batch_init(&font_batch, &game.font_atlas);

	game.proj = m4_ortho(0, WINDOW_RES_X, WINDOW_RES_Y, 0, 1, -1);
	game.view = m4_identity();

	game.running = true;
	while (game.running)
	{
		SDL_Event ev;

		static u64 last, dt;
		u64	now = time_ns();
		dt = now - last;
		last = now;

		window_update(&game.window, col_of());
		input_update(&game.input, now);

		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
				case (SDL_QUIT):
					game.running = false;
					break;
				default:
					input_process(&game.input, &ev);
					break;
			}
		}
		
		font_str(
			&font_batch, 
			"Prout\nProut",
			v2_of(50 + cos(time_s()) * 20, 60 + sin(time_s()) * 10),
			col_of(255), 
			0, 0);
		font_draw(&font_batch, NULL, &game.view, &game.proj);

		window_commit(&game.window, col_of(255));
		
		SDL_GL_SwapWindow(game.sdlwindow);
	}
	sprite_batch_destroy(&font_batch);

	sprite_atlas_destroy(&game.font_atlas);
	sprite_end();
	input_destroy(&game.input);
	window_destroy(&game.window);

	SDL_GL_DeleteContext(game.context);
	SDL_DestroyWindow(game.sdlwindow);
	SDL_Quit();

	debug_print_alloc_stats();
}
