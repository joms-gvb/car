/*
 * Copyright (c) 2026 joms-gvb
 * SPDX-License-Identifier: MIT
 *
 * Distributed under the MIT License. See the LICENSE file for details.
 *
 *
 * Audio file: motorseamless07.wav
 * Source: Z1ph101d - OpenGameArt
 * License: CC0 (public domain)
 * URL: https://opengameart.org/content/some-sounds-0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <SDL2/SDL.h>

volatile sig_atomic_t keep_running = 1;

void sig_handle(int sig)
{
	if(sig == SIGINT)
		keep_running = 0;
}

struct color {
	int cc;
	int sr;
} color = { 0, 0 };

static struct {
	int valid;
	unsigned char byte;
} pushback = { 0, 0 };

void audio_init()
{
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Could not initialize SDL2: %s\n", SDL_GetError());
		exit(1);
	}
}

void log_playtime(const char *sound_file, Uint32 start_time, Uint32 elapsed_time)
{
	FILE *log_file = fopen("playback.log", "a");
	if (log_file == NULL) {
		fprintf(stderr, "Could not open log file for writing.\n");
		return;
	}

	time_t now = time(NULL);
	char *timestamp = ctime(&now);
	if (timestamp != NULL)
		timestamp[strcspn(timestamp, "\n")] = 0;

	fprintf(log_file, "[ %s ] Played '%s': Start Time: %u ms, Elapsed Time: %u ms\n", 
			timestamp, sound_file, start_time, elapsed_time);
	fclose(log_file);
}

void *play_sound(void *sound_file)
{
	SDL_AudioSpec wav_spec;
	Uint32 wav_len;
	Uint8 *wav_buffer;
	SDL_AudioDeviceID device;

	if (SDL_LoadWAV(sound_file, &wav_spec, &wav_buffer, &wav_len) == NULL) {
		fprintf(stderr, "Could not open WAV file: %s\n", SDL_GetError());
		exit(1);
	}

	device = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);

	if (device == 0) {
		fprintf(stderr, "Could not open audio device: %s\n", SDL_GetError());
		SDL_FreeWAV(wav_buffer);
		exit(1);
	}

	while (keep_running) {
		SDL_ClearQueuedAudio(device);
		SDL_QueueAudio(device, wav_buffer, wav_len);
		SDL_PauseAudioDevice(device, 0);
		
		Uint32 start_time = SDL_GetTicks();
		while (keep_running && SDL_GetQueuedAudioSize(device) > 0) {
			log_playtime(sound_file, start_time, SDL_GetTicks() - start_time);
			SDL_Delay(50);
		}

		SDL_ClearQueuedAudio(device);
	}
	

	SDL_CloseAudioDevice(device);
	SDL_FreeWAV(wav_buffer);
	return NULL;
}


int rrange(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

int can_see_on_black(int cc)
{
	switch (cc) {
	case 16:
		return cc + 1;
	case 232:
		return cc + 3;
	}
	return cc;
}

void sr_color(struct color *cc)
{
	if (cc->sr == 1) {
		cc->sr = 0;
		printf("\x1b[00m");
	} else if (cc->sr == 0) {
		printf("\x1b[38;5;%dm", cc->cc);
		cc->sr = 1;
	}
}

void cls()
{
	printf("\x1b[H\x1b[J");
}

void cpos(int pos)
{
	for (int i = 0; i < pos; i++)
		putchar(0x20);
}

void print_car(int pos, int wheel)
{
	cpos(pos);

	printf("   ___\n");
	cpos(pos);
	printf(" _|_R_|__\n");
	cpos(pos);
	printf(" |_______|\n");
	cpos(pos);

	if (wheel == 0)
		printf(" o-o   o-o\n");
	else
		printf(" O-O   O-O\n");

}

void print_person(int pos, int wheel_state)
{
	cpos(pos);
	printf("   O   \n");
	cpos(pos);
	printf("  /|\\  \n");
	cpos(pos);
	switch (wheel_state) {
	case 0:
		printf("  | |  \n");
		break;
	case 1:
		printf("   / \\ \n");
		break;
	case 2:
		printf("  | |  \n");
		break;
	case 3:
		printf("  \\ /  \n");
		break;
	case 4:
		printf("  | |  \n");
		break;
	}
}



int get_tty_wid()
{
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	return w.ws_col;
}

void print_bg(int wid)
{
	for (int i = 0; i < wid; i++)
		printf("%c", (i % 2 == 0) ? '-' : ' ');
	putchar(0x0A);
}

int kbhit(void)
{
	unsigned char ch;
	int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	if (oldf == -1)
		return 0;
	if (fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK) == -1)
		return 0;

	size_t r = read(STDIN_FILENO, &ch, 1);
	if ((int)r > 0) {
		pushback.byte = ch;
		pushback.valid = 1;
		fcntl(STDIN_FILENO, F_SETFL, oldf);
		return 1;
	}

	fcntl(STDIN_FILENO, F_SETFL, oldf);
	return 0;
}

int getch()
{
	struct termios oldt, newt;
	unsigned char ch;
	ssize_t r;
	if (pushback.valid) {
		pushback.valid = 0;
		return (int)pushback.byte;
	}
	if (tcgetattr(STDIN_FILENO, &oldt) == -1)
		return -1;
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1)
		return -1;
	r = read(STDIN_FILENO, &ch, 1);

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	if (r == 1)
		return (int)ch;
	return -1;
}

int rand_color()
{
	int cc = rrange(1, 255);
	int tmp = can_see_on_black(cc);
	return tmp;
}

int main(int argc, char **argv)
{

	signal(SIGINT, sig_handle);
	audio_init();
	pthread_t audio_thread;
	const char *car_sound_file = "../sounds/motorseamless07.wav";
	int opt;
	int an_type = 1;
	while ((opt = getopt(argc, argv, "ph")) != EOF) {
		switch (opt) {
		case 'p':
			an_type = 2;
			break;
		case 'h':
			fprintf(stderr,
				"Usage: %s [-p] prints a walking person insted of the car, [-h] help\n",
				argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	pthread_create(&audio_thread, NULL, play_sound, (void*)car_sound_file);
	int wid = get_tty_wid() - 10;
	int wheel = 0;
	int wheel_state = 0;
	int sleep_time = 200000;
	int pos = 0;
	const int max_sleep = 600000;
	const int min_sleep = 10000;


	srand(time(NULL));
	struct color cc;
	cc.sr = 0;
	cc.cc = rand_color();

	

	while (keep_running) {
		cls();
		sr_color(&cc);
		if (an_type == 1) {
		print_car(pos, wheel);
		sr_color(&cc);
		print_bg(wid + 10);
		wheel ^= 1;
		} else if (an_type == 2) {
			print_person(pos, wheel_state);
			sr_color(&cc);
			print_bg(wid + 8);
			wheel_state = (wheel_state + 1) % 5;
		}
		if (kbhit()) {
			int ch = getch();
			if (ch != -1) {
				if (ch == 'f' || ch == '+')
					sleep_time -= 10000;
				else if (ch == 's' || ch == '-')
					sleep_time += 10000;
				else if (ch == 'q') {
					keep_running = 0;
					break;
				}

				if (sleep_time < min_sleep)
					sleep_time = min_sleep;
				if (sleep_time > max_sleep)
					sleep_time = max_sleep;
			}
		}
		printf("sleep_time\t%d\n", sleep_time);
		fflush(stdout);
		usleep((useconds_t) sleep_time);
		pos++;
		if (pos > wid)
			pos = 0;
	}

	fflush(stdout);

	printf("Exiting... Thank you for watching!\n");
	pthread_join(audio_thread, NULL);
	SDL_Quit();
	return 0;
}
