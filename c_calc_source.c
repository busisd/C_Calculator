#include <stdio.h>
#include <SDL.h>

const int SCREEN_HEIGHT = 600;
const int SCREEN_WIDTH = 500;

int main(int argc, char* args[]) {	
	SDL_Event e;
	SDL_Window* calc_window = NULL;
	SDL_Surface* calc_surface = NULL;
	SDL_bool exit_flag = SDL_FALSE;
	SDL_bool initiate_program = SDL_TRUE;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not be initiated: %s", SDL_GetError());
		initiate_program = SDL_FALSE;
	}
	else {
		calc_window = SDL_CreateWindow("C Calculator", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (calc_window == NULL) {
			printf("Window could not be created: %s", SDL_GetError());
			initiate_program = SDL_FALSE;
		}
	}

	if (initiate_program) {
		calc_surface = SDL_GetWindowSurface(calc_window);

		Uint32 color_bg = SDL_MapRGB(calc_surface->format, 210, 210, 240);
		Uint32 color_key_up = SDL_MapRGB(calc_surface->format, 50, 80, 160);
		Uint32 color_key_pressed = SDL_MapRGB(calc_surface->format, 30, 50, 130);

		int cur_pressed_button_num = -1;

		SDL_FillRect(calc_surface, NULL, color_bg);

		SDL_Rect number_rects[9];
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				float top_bound = SCREEN_HEIGHT * 0.4;
				float bottom_bound = SCREEN_HEIGHT * 0.8;
				float padding = 5;

				int new_x = (int) (padding + i*SCREEN_WIDTH/3);
				int new_y = (int) (bottom_bound - top_bound)/3 * j + padding + top_bound;
				int new_width = SCREEN_WIDTH/3 - 2*padding;
				int new_height = (bottom_bound - top_bound)/3 - 2*padding;

				SDL_Rect new_square = {new_x, new_y, new_width, new_height};
				number_rects[i+j*3] = new_square;
				SDL_FillRect(calc_surface, &number_rects[i+j*3], color_key_up);
			}
		}

		SDL_UpdateWindowSurface(calc_window);

		SDL_Rect rect_test = { 100,100,30,30 };

		while (!exit_flag) {
			while (SDL_PollEvent(&e) != 0) {
				if (e.type == SDL_QUIT) {
					exit_flag = SDL_TRUE;
				}
				if (e.type == SDL_MOUSEBUTTONDOWN) {
					//SDL_FillRect(calc_surface, &rect_test, SDL_MapRGB(calc_surface->format, 30, 30, 60));

					int x, y;
					SDL_GetMouseState(&x, &y);
					SDL_Point cur_mouse_pos = { x, y };

					for (int i = 0; i < 9; i++) {
						//printf("%d", SDL_PointInRect(&cur_mouse_pos, &number_rects[i]) ? 1 : 0);
						if (SDL_PointInRect(&cur_mouse_pos, &number_rects[i])) {
							SDL_FillRect(calc_surface, &number_rects[i], color_key_pressed);
							cur_pressed_button_num = i;
						}
					}

					SDL_UpdateWindowSurface(calc_window);
				}
				if (e.type == SDL_MOUSEBUTTONUP) {
					int x, y;
					SDL_GetMouseState(&x, &y);
					SDL_Point cur_mouse_pos = { x, y };

					//for (int i = 0; i < 9; i++) {
					//	SDL_FillRect(calc_surface, &number_rects[i], color_key_up);
					//}

					if (cur_pressed_button_num > -1) {
						SDL_FillRect(calc_surface, &number_rects[cur_pressed_button_num], color_key_up);
						if (SDL_PointInRect(&cur_mouse_pos, &number_rects[cur_pressed_button_num])) {
							printf("Number entered: %d\n", cur_pressed_button_num);
						}
					}
					SDL_UpdateWindowSurface(calc_window);
					cur_pressed_button_num = -1;
				}

			}
		}
	}

	SDL_DestroyWindow(calc_window);

	SDL_Quit();

	return 0;
}
