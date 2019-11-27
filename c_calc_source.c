#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string.h>

void handle_number();

SDL_Rect center_rect();

const int SCREEN_HEIGHT = 600;
const int SCREEN_WIDTH = 500;
const char button_text_vals[20][3] = {
	"ce", "c", "<", "/",
	"7", "8", "9", "*", 
	"4", "5", "6", "-", 
	"1", "2", "3", "+", 
	"±", "0", ".", "=",
};

SDL_Color text_color = { 0,0,0 };
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
		float top_bound = SCREEN_HEIGHT * 0.3;
		float bottom_bound = SCREEN_HEIGHT * 1.0;
		float padding = 5;

		TTF_Font* calc_font = TTF_OpenFont("./Font_Files/Roboto-Black.ttf", 32);
		if (calc_font == NULL) {
			printf("Font not found!\n");
		}
		
		for (int i = 0; i < button_cols; i++) {
			for (int j = 0; j < button_rows; j++) {
				int new_x = (int) (padding + i*SCREEN_WIDTH/button_cols);
				int new_y = (int) (bottom_bound - top_bound)/button_rows * j + padding + top_bound;
				int new_width = SCREEN_WIDTH/button_cols - 2*padding;
				int new_height = (bottom_bound - top_bound)/button_rows - 2*padding;

				SDL_Rect new_square = {new_x, new_y, new_width, new_height};
				number_rects[i+j*button_cols] = new_square;
				SDL_FillRect(calc_surface, &number_rects[i+j*button_cols], color_key_up);

				text_message_surfaces[i + j * button_cols] = TTF_RenderText_Blended(calc_font, button_text_vals[i + j * button_cols], text_color);
				if (!(text_message_surfaces[i + j * button_cols])) {
					printf("Message surface could not be created! Error: %s\n", TTF_GetError());
				}

				int cur_surface_width = text_message_surfaces[i + j * button_cols]->w;
				int cur_surface_height = text_message_surfaces[i + j * button_cols]->h;

				number_text_rects[i + j * button_cols] = center_rect(new_square, cur_surface_width, cur_surface_height);
				SDL_BlitSurface(text_message_surfaces[i + j * button_cols], NULL, calc_surface, &number_text_rects[i + j * button_cols]);
			}
		}

		SDL_UpdateWindowSurface(calc_window);

		while (!exit_flag) {
			while (SDL_PollEvent(&e) != 0) {
				if (e.type == SDL_QUIT) {
					exit_flag = SDL_TRUE;
				}
				if (e.type == SDL_MOUSEBUTTONDOWN) {
					int x = e.button.x, y = e.button.y;
					SDL_Point cur_mouse_pos = { x, y };

					for (int i = 0; i < 20; i++) {
						if (SDL_PointInRect(&cur_mouse_pos, &number_rects[i])) {
							SDL_FillRect(calc_surface, &number_rects[i], color_key_pressed);
							cur_pressed_button_num = i;
							SDL_BlitSurface(text_message_surfaces[cur_pressed_button_num], NULL, calc_surface, &number_text_rects[cur_pressed_button_num]);
						}
					}

					SDL_UpdateWindowSurface(calc_window);
				}
				if (e.type == SDL_MOUSEBUTTONUP) {
					int x = e.button.x, y = e.button.y;
					SDL_Point cur_mouse_pos = { x, y };
					
					if (cur_pressed_button_num > -1) {
						SDL_FillRect(calc_surface, &number_rects[cur_pressed_button_num], color_key_up);
						if (SDL_PointInRect(&cur_mouse_pos, &number_rects[cur_pressed_button_num])) {
							handle_number(cur_pressed_button_num, calc_window, calc_surface, calc_font);
						}
						SDL_BlitSurface(text_message_surfaces[cur_pressed_button_num], NULL, calc_surface, &number_text_rects[cur_pressed_button_num]);
					}
					SDL_UpdateWindowSurface(calc_window);
					cur_pressed_button_num = -1;
				}

			}
		}

		TTF_CloseFont(calc_font);
	}

	for (int i = 0; i < 20; i++){
		SDL_FreeSurface(text_message_surfaces[i]);
	}
	
	SDL_DestroyWindow(calc_window);

	TTF_Quit();

	SDL_Quit();

	return 0;
}

int char_to_int(char *char_array[]) {
	if (char_array[0] == '\0') {
		return 0;
	}
	else {
		int num_int;
		sscanf_s(char_array, "%d", &num_int);
		return num_int;
	}
}

double perform_op(double num1, double num2, int operator) {
	switch (operator) {
	case 3:
		return num2 / num1;
	case 7:
		return num2 * num1;
	case 11:
		return num2 - num1;
	case 15:
		return num2 + num1;
	default:
		return num1;
	}
}

double add_digit(double number, int new_digit) {
	if (number >= 0) {
		return number * 10 + new_digit;
	}
	else {
		return number * 10 - new_digit;
	}
}

void update_nums(int new_input);
void display_numbers(double prev_num, double cur_num, int cur_op, SDL_Window* calc_window, SDL_Surface* calc_surface, TTF_Font* calc_font);

double prev_num = 0;
double cur_num = 0;
int cur_op = 0;
void handle_number(int new_input, SDL_Window* calc_window, SDL_Surface* calc_surface, TTF_Font* calc_font) {
	switch (new_input) {
	case 17:
	case 12:
	case 13:
	case 14:
	case 8:
	case 9:
	case 10:
	case 4:
	case 5:
	case 6:
		cur_num = add_digit(cur_num, button_text_vals[new_input][0] - '0');
		break;
	
	case 16:
		cur_num = cur_num * (-1);
		break;

	case 1:
		prev_num = 0;
	case 0:
		cur_num = 0;
		break;

	case 3:
		cur_op = 3;
		break;
	case 7:
		cur_op = 7;
		break;
	case 11:
		cur_op = 11;
		break;
	case 15:
		cur_op = 15;
		break;
	case 19:
		prev_num = perform_op(cur_num, prev_num, cur_op);
		cur_num = 0;
		cur_op = 0;
		break;
	}
	printf("Previous number: %lf;   Number entered: %lf;   Cur op: %s\n", prev_num, cur_num, button_text_vals[cur_op]);
	display_numbers(prev_num, cur_num, cur_op, calc_window, calc_surface, calc_font);
	//printf("Button pressed: %d\n", new_input);
}

SDL_Rect center_rect(SDL_Rect outer_rect, int message_width, int message_height) {
	int new_x = (int)(outer_rect.x + outer_rect.w / 2 - message_width / 2);
	int new_y = (int)(outer_rect.y + outer_rect.h / 2 - message_height / 2);
	SDL_Rect centered_rect = {new_x, new_y, message_width, message_height};
	return centered_rect;
}

char* prev_num_string[100], cur_num_string[100];
SDL_Surface *prev_num_surface, *cur_num_surface, *op_surface;
const SDL_Rect clear_area_rect = { 0, 0, 500, 180 };
SDL_Rect prev_num_rect = { 10, 10, 10, 10 }, cur_num_rect = { 10, 50, 10, 10 }, op_rect = { 460, 10, 10, 10 };
void display_numbers(double prev_num, double cur_num, int cur_op, SDL_Window *calc_window, SDL_Surface *calc_surface, TTF_Font *calc_font) {

	SDL_FreeSurface(prev_num_surface);
	SDL_FreeSurface(cur_num_surface);

	SDL_Color bg_color = { 210, 210, 240 };
	Uint32 color_bg = SDL_MapRGB(calc_surface->format, 210, 210, 240);
	SDL_FillRect(calc_surface, &clear_area_rect, color_bg);
	snprintf(prev_num_string, sizeof(prev_num_string), "%f", prev_num);
	snprintf(cur_num_string, sizeof(cur_num_string), "%f", cur_num);
	prev_num_surface = TTF_RenderText_Shaded(calc_font, prev_num_string, text_color, bg_color);
	SDL_BlitSurface(prev_num_surface, NULL, calc_surface, &prev_num_rect);
	cur_num_surface = TTF_RenderText_Shaded(calc_font, cur_num_string, text_color, bg_color);
	SDL_BlitSurface(cur_num_surface, NULL, calc_surface, &cur_num_rect);
	if (cur_op != 0) {
		SDL_FreeSurface(op_surface);
		op_surface = TTF_RenderText_Shaded(calc_font, button_text_vals[cur_op], text_color, bg_color);
		SDL_BlitSurface(op_surface, NULL, calc_surface, &op_rect);
	}
	SDL_UpdateWindowSurface(calc_window);
}

/*
TODO:
clean up a bit! (especially display_numbers)
improve calvulator behavior!
improve color scheme?
add keyboard commands
add method docs

*/