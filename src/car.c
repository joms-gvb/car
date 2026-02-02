#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>

void cls()
{
	printf("\x1b[H\x1b[J");
}

void cpos(int pos)
{
	for(int i = 0; i < pos; i++)
		putchar(0x20);
}

void print_car(int pos, int wheel)
{
	cpos(pos);
	printf("   __\n");
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
	for(int i = 0; i < wid; i++) 
		printf("%c", (i % 2 == 0) ? '-' : ' ');
	putchar(0x0A);
}

char getch()
{
	struct termios oldt, newt;
	char ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}

int kbhit(void)
{
	struct termios oldt, newt;
	char ch;
	int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	if (read(STDIN_FILENO, &ch, 1) > 0) {
		ungetc(ch, stdin);
		return 1;
	}

	fcntl(STDIN_FILENO, F_SETFL, oldf);
	return 0;
}

	

int main()
{
	int wid = get_tty_wid() - 10;
	int wheel = 0;
	int sleep_time = 200000;
	int pos = 0;
	const int max_sleep = 600000;
	const int min_sleep = 10000;

	for( ;; ) {
		cls();
		print_car(pos, wheel);
		print_bg(wid + 10);
		wheel ^= 1;
		if(kbhit()) {
			char ch = getch();
			if (ch == 'f' || ch == '+')
				sleep_time = (sleep_time > 100000) ? sleep_time - 10000 : sleep_time;
			else if(ch == 's' ||ch == '-')
				sleep_time += 10000;
			else if(ch == 'q')
				break;
		}
		printf("sleep_time\t%d\n", sleep_time);
		usleep(sleep_time);
		pos++;
		if (pos > wid)
			pos = 0;
	}
	
	fflush(stdout);
	return 0;
}
