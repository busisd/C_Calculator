#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>

const int SCREEN_HEIGHT = 600;
const int SCREEN_WIDTH = 500;
const char button_text_vals[20][3] = {
	"ce", "c", "<", "/",
	"7", "8", "9", "*", 
	"4", "5", "6", "-", 
	"1", "2", "3", "+", 
	"±", "0", ".", "=",
};

int main(int argc, char* args[]) {	
	SDL_Event e;
	SDL_Window* calc_window = NULL;
	SDL_Surface* calc_surface = NULL;
	SDL_bool exit_flag = SDL_FALSE;
	SDL_bool initiate_program = SDL_TRUE;
	SDL_Surface* text_message_surfaces[20];

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not be initiated: %s", SDL_GetError());
		initiate_program = SDL_FALSE;
	}
	else if (TTF_Init() < 0) {
		printf("TTF could not be initiated: %s", TTF_GetError());
		initiate_program = SDL_FALSE;
	}
	else {
		calc_window = SDL_CreateWindow("C Calculator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

		const int button_cols = 4;
		const int button_rows = 5;
		SDL_Rect number_rects[20];
		SDL_Rect number_text_rects[20];
		float top_bound = SCREEN_HEIGHT * 0.4;
		float bottom_bound = SCREEN_HEIGHT * 1.0;
		float padding = 5;

		TTF_Font* calc_font = TTF_OpenFont("./Font_Files/Roboto-Black.ttf", 24);
		if (calc_font == NULL) {
			printf("Font not found!\n");
		}
		SDL_Color text_color = { 0,0,0 };
		
		for (int i = 0; i < button_cols; i++) {
			for (int j = 0; j < button_rows; j++) {
				int new_x = (int) (padding + i*SCREEN_WIDTH/button_cols);
				int new_y = (int) (bottom_bound - top_bound)/button_rows * j + padding + top_bound;
				int new_width = SCREEN_WIDTH/button_cols - 2*padding;
				int new_height = (bottom_bound - top_bound)/button_rows - 2*padding;

				SDL_Rect new_square = {new_x, new_y, new_width, new_height};
				number_rects[i+j*button_cols] = new_square;
				SDL_FillRect(calc_surface, &number_rects[i+j*button_cols], color_key_up);

				SDL_Rect new_text_square = { new_x, new_y, new_width, new_height };
				number_text_rects[i + j * button_cols] = new_text_square;
				if (!(text_message_surfaces[i + j * button_cols] = TTF_RenderText_Solid(calc_font, button_text_vals[i + j * button_cols], text_color))) {
					printf("Message surface could not be created! Error: %s\n", TTF_GetError());
				}
				SDL_BlitSurface(text_message_surfaces[i + j * button_cols], NULL, calc_surface, &number_text_rects[i + j * button_cols]);
			}
		}

		TTF_CloseFont(calc_font);
		SDL_UpdateWindowSurface(calc_window);

		while (!exit_flag) {
			while (SDL_PollEvent(&e) != 0) {
				if (e.type == SDL_QUIT) {
					exit_flag = SDL_TRUE;
				}
				if (e.type == SDL_MOUSEBUTTONDOWN) {
					//SDL_FillRect(calc_surface, &rect_test, SDL_MapRGB(calc_surface->format, 30, 30, 60));

					int x = e.button.x, y = e.button.y;
					SDL_Point cur_mouse_pos = { x, y };

					for (int i = 0; i < 20; i++) {
						//printf("%d", SDL_PointInRect(&cur_mouse_pos, &number_rects[i]) ? 1 : 0);
						if (SDL_PointInRect(&cur_mouse_pos, &number_rects[i])) {
							SDL_FillRect(calc_surface, &number_rects[i], color_key_pressed);
							cur_pressed_button_num = i;
							SDL_BlitSurface(text_message_surfaces[cur_pressed_button_num], NULL, calc_surface, &number_text_rects[cur_pressed_button_num]);
						}
					}

					SDL_UpdateWindowSurface(calc_window);
				}
				if (e.type == SDL_MOUSEBUTTONUP) {
					//int x, y;
					//SDL_GetMouseState(&x, &y);
					int x = e.button.x, y = e.button.y;
					SDL_Point cur_mouse_pos = { x, y };
					
					//for (int i = 0; i < 20; i++) {
					//	SDL_FillRect(calc_surface, &number_rects[i], color_key_up);
					//}

					if (cur_pressed_button_num > -1) {
						SDL_FillRect(calc_surface, &number_rects[cur_pressed_button_num], color_key_up);
						if (SDL_PointInRect(&cur_mouse_pos, &number_rects[cur_pressed_button_num])) {
							printf("Number entered: %d\n", cur_pressed_button_num);
						}
						SDL_BlitSurface(text_message_surfaces[cur_pressed_button_num], NULL, calc_surface, &number_text_rects[cur_pressed_button_num]);
					}
					SDL_UpdateWindowSurface(calc_window);
					cur_pressed_button_num = -1;
				}

			}
		}
	}

	for (int i = 0; i < 20; i++){
		SDL_FreeSurface(text_message_surfaces[i]);
	}
	
	SDL_DestroyWindow(calc_window);

	TTF_Quit();

	SDL_Quit();

	return 0;
}
