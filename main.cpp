// @COMPILECMD g++ $@ -o $* $(pkg-config --cflags --libs sdl2)
#include <sys/param.h>	// MAX()
#include <math.h>
#include <SDL.h>

SDL_Window*   window;
SDL_Renderer* renderer;
static bool is_running = true;


typedef struct {
	int y;
	int x;
} spatial_t;



void dda(const spatial_t from, const spatial_t to) {
	const int steps = MAX(abs(to.y - from.y), abs(to.x - from.x));

	struct {
		double y;
		double x;
	} d = {
		.y = (double)from.y,
		.x = (double)from.x,
	};

	for (int i = 0; i < steps+1; i++) {
		SDL_RenderDrawPoint(renderer, round(d.x), round(d.y));
		d.y += (double)(to.y - from.y) / (double)steps;
		d.x += (double)(to.x - from.x) / (double)steps;
	}
	return;
}

void midpoint(spatial_t from, const spatial_t to) {
	spatial_t d = {
		.y = abs(to.y - from.y),
		.x = abs(to.x - from.x),
	};
	spatial_t s = {
		.y = (from.y < to.y) ? 1 : -1,
		.x = (from.x < to.x) ? 1 : -1,
	};

	int direction = (d.x > d.y ? d.x : -d.y) / 2;

	while (true) {
		SDL_RenderDrawPoint(renderer, from.x, from.y);

		if (from.x == to.x
		&&  from.y == to.y) {
			break;
		}

		int buffer = direction;
		if (buffer > -d.x) {
			direction += -d.y;
			from.x    +=  s.x;
		}
		if (buffer <  d.y) {
			direction +=  d.x;
			from.y    +=  s.y;
		}
	}
}


void draw_circle_8w(const SDL_Point &p, const int &r){
	const int r2 = r * r;

	SDL_RenderDrawPoint(renderer, p.x    , p.y + r);
	SDL_RenderDrawPoint(renderer, p.x    , p.y - r);
	SDL_RenderDrawPoint(renderer, p.x + r, p.y    );
	SDL_RenderDrawPoint(renderer, p.x - r, p.y    );

	for (int x = 1, y = (int)(round(sqrt(r2 - x*x)));
		 x <= y;
		 ++x, y = (int)(round(sqrt(r2 - x*x)))) {
		SDL_RenderDrawPoint(renderer, p.x + x, p.y + y);
		SDL_RenderDrawPoint(renderer, p.x + x, p.y - y);
		SDL_RenderDrawPoint(renderer, p.x - x, p.y + y);
		SDL_RenderDrawPoint(renderer, p.x - x, p.y - y);
		SDL_RenderDrawPoint(renderer, p.x + y, p.y + x);
		SDL_RenderDrawPoint(renderer, p.x + y, p.y - x);
		SDL_RenderDrawPoint(renderer, p.x - y, p.y + x);
		SDL_RenderDrawPoint(renderer, p.x - y, p.y - x);
	}
}



signed main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	window   = SDL_CreateWindow("Circle", 0, 0, 800, 800, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	SDL_Event event;
	spatial_t mouse_position_buffer;
	bool is_follow_up_click = false;
	bool is_mouse_down      = false;

	while (is_running) {
		SDL_RenderPresent(renderer);
		if (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					is_running = false;
				} break;
				case SDL_MOUSEBUTTONDOWN: {
					switch (event.button.button) {
						case SDL_BUTTON_LEFT: {
							SDL_RenderDrawPoint(renderer, event.motion.x, event.motion.y);
							if (is_follow_up_click) {
								midpoint(
									(spatial_t){mouse_position_buffer.y, mouse_position_buffer.x},
									(spatial_t){event.motion.y, event.motion.x}
								);
							}
							is_follow_up_click = true;
						} break;
						case SDL_BUTTON_RIGHT: {
							draw_circle_8w((SDL_Point){event.motion.x, event.motion.y}, 10);
							is_follow_up_click = true;
						} break;
						case SDL_BUTTON_MIDDLE: {
							is_follow_up_click = false;
						} break;

					}
					// ---
					is_mouse_down = true;
					mouse_position_buffer = (spatial_t){
						.y = event.motion.y,
						.x = event.motion.x,
					};
				} break;
				case SDL_MOUSEBUTTONUP: {
					is_mouse_down = false;
				} break;
				case SDL_MOUSEMOTION: {
					if (is_mouse_down) {
						midpoint(
							(spatial_t){mouse_position_buffer.y, mouse_position_buffer.x},
							(spatial_t){event.motion.y, event.motion.x}
						);
						mouse_position_buffer = (spatial_t){
							.y = event.motion.y,
							.x = event.motion.x,
						};
						
					}
				} break;
			}
		}
        SDL_Delay(10);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
