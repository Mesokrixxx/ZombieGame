#ifdef __WIN32__
	#define SDL_MAIN_HANDLED
#endif

#define UTIL_IMPL

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

#define DEFAULT_TPS 60
#define DEFAULT_FPS 240

#define DEFAULT_ENTITY_COUNTCAP 256 // 2 ^ 8

typedef struct s_zombiegame {
	SDL_Window		*sdlwindow;
	SDL_GLContext	context;
	t_input			input;
	t_sprite_atlas	font_atlas;
	t_sprite_batch	font_batch;
	m4				proj;
	m4				view;
	t_window		window;

	struct {
		i32	TPS, cTPS, FPS, cFPS;
		f32	Tdt, Fdt;
		
		f32	last_s, last_f, now, dt, accumulator;
		i32	tcount;
	}				s_time;

	t_ecs			ecs;
	bool			running;
}	t_zombiegame;

void	setFPS(t_zombiegame *game, i32 FPS);
void	setTPS(t_zombiegame *game, i32 TPS);
void	init(t_zombiegame *game);
void	update(t_zombiegame *game);
void	tick(t_zombiegame *game);
void	render(t_zombiegame *game);
void	destroy(t_zombiegame *game);

int main() {
	t_zombiegame game = {0};
	
	init(&game);

	game.running = true;
	while (game.running)
	{
		game.s_time.now = time_s();
		game.s_time.dt = game.s_time.now - game.s_time.last_f;
		game.s_time.last_f = game.s_time.now;
		
		update(&game);
		game.s_time.accumulator += game.s_time.dt;
		while (game.s_time.accumulator >= game.s_time.Tdt) {
			tick(&game);
			game.s_time.tcount++;
			game.s_time.accumulator -= game.s_time.Tdt;
		}
		render(&game);

		if (game.s_time.now - game.s_time.last_s >= 1) {
			game.s_time.cFPS = time_fps(game.s_time.dt);
			game.s_time.cTPS = game.s_time.tcount;
			
			game.s_time.tcount = 0;
			game.s_time.last_s = game.s_time.now;
		}
		time_cap(game.s_time.Fdt, game.s_time.now);
	}

	destroy(&game);
}

void	setFPS(t_zombiegame *game, i32 FPS)
{
	if (game->s_time.FPS == FPS)
		return ;

	game->s_time.FPS = FPS;
	game->s_time.Fdt = 
		game->s_time.FPS > 0 ? (1.0 / game->s_time.FPS) : 0;
}

void	setTPS(t_zombiegame *game, i32 TPS)
{
	if (game->s_time.TPS == TPS)
		return ;

	game->s_time.TPS = TPS;
	game->s_time.Tdt = 
		1.0 / (game->s_time.TPS > 0 ? game->s_time.TPS : 1);
}

void	init(t_zombiegame *game)
{
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

	game->window.window_size = v2i_of(WINDOW_WIDTH, WINDOW_HEIGHT);
	game->sdlwindow = 
		SDL_CreateWindow("Zombie Game",
			SDL_WINDOWPOS_CENTERED_DISPLAY(1), SDL_WINDOWPOS_CENTERED_DISPLAY(1),
			game->window.window_size.x, game->window.window_size.y,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	ASSERT(game->sdlwindow,
		"Failed to create SDL Window: %s", SDL_GetError());

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	game->context = SDL_GL_CreateContext(game->sdlwindow);
	ASSERT(game->context,
		"Failed to create GL Context %s", SDL_GetError());
	SDL_GL_MakeCurrent(game->sdlwindow, game->context);

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

	game->proj = m4_ortho(0, WINDOW_RES_X, WINDOW_RES_Y, 0, 1, -1);
	game->view = m4_identity();
	
	window_init(
		&game->window, 
		game->window.window_size, 
		v2i_of(WINDOW_RES_X, WINDOW_RES_Y), 
		WINDOW_BUF_COLOR);
	input_init(&game->input);
	sprite_init();
	sprite_atlas_init(&game->font_atlas, "res/sprites/font.png", v2i_of(8));
	sprite_batch_init(&game->font_batch, &game->font_atlas);
	
	ecs_init(&game->ecs, DEFAULT_ENTITY_COUNTCAP);

	setFPS(game, DEFAULT_FPS);
	setTPS(game, DEFAULT_TPS);

	i32	player = ecs_new(&game->ecs);
	ecs_add(&game->ecs, player, ECS_CMP_POSITION, &(t_ECSCMP_Position){ .pos = v3_of(2) });
}

void	update(t_zombiegame *game)
{
	window_update(&game->window, col_of());
	input_update(&game->input, game->s_time.now);

	ecs_event(&game->ecs, ECS_EVT_UPDATE);

	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
			case (SDL_QUIT):
				game->running = false;
				break;
			default:
				input_process(&game->input, &ev);
				break;
		}
	}
}

void	tick(t_zombiegame *game)
{
	ecs_event(&game->ecs, ECS_EVT_TICK);
}

void	render(t_zombiegame *game)
{
	char	inf_game[32];

	ecs_event(&game->ecs, ECS_EVT_RENDER);

	sprintf(inf_game, "FPS: %d\nTPS: %d", game->s_time.cFPS, game->s_time.cTPS);
	font_str(
		&game->font_batch,
		inf_game,
		v2_of(2, 2),
		col_of(255),
		1.0,
		FONT_NO_FLAGS);
	font_draw(&game->font_batch, NULL, &game->view, &game->proj);

	window_commit(&game->window, col_of(255));
	SDL_GL_SwapWindow(game->sdlwindow);
}

void	destroy(t_zombiegame *game)
{
	ecs_event(&game->ecs, ECS_EVT_DESTROY);
	ecs_destroy(&game->ecs);

	sprite_batch_destroy(&game->font_batch);
	sprite_atlas_destroy(&game->font_atlas);
	sprite_end();

	input_destroy(&game->input);
	window_destroy(&game->window);

	SDL_GL_DeleteContext(game->context);
	SDL_DestroyWindow(game->sdlwindow);
	SDL_Quit();

	debug_print_alloc_stats();
}
