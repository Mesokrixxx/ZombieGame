#ifdef __WIN32__
# define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <stdio.h>
#include <stdbool.h>

#include "util/util.h"

typedef struct s_zombiegame {
	SDL_Window		*window;
	SDL_GLContext	context;
	bool			running;
}	t_zombiegame;

int main() {
	t_zombiegame game;

	ASSERT(!SDL_Init(SDL_INIT_VIDEO),
		"Failed to init SDL: %s", SDL_GetError());

	game.window = 
		SDL_CreateWindow("Zombie Game",
			SDL_WINDOWPOS_CENTERED_DISPLAY(1), SDL_WINDOWPOS_CENTERED_DISPLAY(1),
			1080, 720,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	ASSERT(game.window,
		"Failed to create SDL Window: %s", SDL_GetError());

	game.context = SDL_GL_CreateContext(game.window);
	ASSERT(game.context,
		"Failed to create GL Context %s", SDL_GetError());

	ASSERT(glewInit() == GLEW_OK,
		"Failed to init GLEW");

	game.running = true;
	while (game.running)
	{
		SDL_Event ev;

		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
				case (SDL_QUIT):
					game.running = false;
					break;
			}
		}

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(game.window);
	}

	SDL_GL_DeleteContext(game.context);
	SDL_DestroyWindow(game.window);
	SDL_Quit();
}
