/*
 * Copyright (c) 2026 joms-gvb
 * SPDX-License-Identifier: MIT
 *
 * Distributed under the MIT License. See the LICENSE file for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

struct color {
	int cc;
	int sr;
} color = { 0, 0 };

static struct {
	int valid;
	unsigned char byte;
} pushback = { 0, 0 };

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

int main()
{
	int wid = get_tty_wid() - 10;
	int wheel = 0;
	int sleep_time = 200000;
	int pos = 0;
	const int max_sleep = 600000;
	const int min_sleep = 10000;

	srand(time(NULL));
	struct color cc;
	cc.sr = 0;
	cc.cc = rand_color();

	for (;;) {
		cls();
		sr_color(&cc);
		print_car(pos, wheel);
		sr_color(&cc);
		print_bg(wid + 10);
		wheel ^= 1;
		if (kbhit()) {
			int ch = getch();
			if (ch != -1) {
				if (ch == 'f' || ch == '+')
					sleep_time -= sleep_time;
				else if (ch == 's' || ch == '-')
					sleep_time += 10000;
				else if (ch == 'q')
					break;

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
	return 0;
}
