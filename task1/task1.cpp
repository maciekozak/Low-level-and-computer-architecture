// By Maciej Kopa 230451 and Dominik Czerwoniuk 230446 //
// --------------------------------------------------- //
#include <dos.h> //pokeb(), //inportb()
#include <conio.h> //clrscr(), getch(), _setcursortype(), kbhit()
#include <iostream.h>
//arrowkeys
#define KeyUp 72
#define KeyDown 80
#define KeyLeft 75
#define KeyRight 77
/// 
#define KeyESC 27 //ESC
#define Forgrnd 73 //PgUp
#define Bgk 81 //PgDn
#define Insert 82 //Insert

void animate_text(){
	
	char text[] = {'c','o','m','p','u','t','e','r',' ','a','r','c','h'};
	int text_length = 13;
	
	int x_offset = 0;
	int y_offset = 0;
	
	int animate_color=0x0F, foreground_range=15; //because we start with white starting foreground_range is 15
	
	 unsigned char choice;
	 clrscr();
	 _setcursortype(_NOCURSOR);
	 do
	 {
		 choice = inportb(0x60);
		 delay(75);
		 switch (choice){
			case KeyDown:
				if ((y_offset + 1) < 25) y_offset++;
				break;
			case KeyUp:
			if (y_offset > 0) y_offset--;
				break;
			case KeyLeft:
				if (x_offset > 0) x_offset--;
				break;
			case KeyRight:
				if ((x_offset + text_length) < 80) x_offset++; //we take for account text width while moving
				break;
			case Forgrnd:
				if(foreground_range == 15) {animate_color -= 15; foreground_range=0;} else {animate_color++; foreground_range++;}
				break;
			case Bgk:
				animate_color += 16;
				break;
			}
			clrscr();
			for (int i = 0; i < text_length; i++) {
				pokeb(0xB800, 0x0000 + (y_offset * 80 * 2) + ((x_offset + i) * 2) + 0, text[i]); //printing text at offset
				pokeb(0xB800, 0x0000 + (y_offset * 80 * 2) + ((x_offset + i) * 2) + 1, animate_color); //changing animate_color
			}
	 }
	 while (choice != 1); //exit on esc press
	 while (kbhit()) getch();
	 clrscr();
	 _setcursortype(_NORMALCURSOR);
}

int main(){
	animate_text();
	return 0;
}