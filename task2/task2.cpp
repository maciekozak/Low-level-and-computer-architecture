//////////////////////////////////////////////////////////////////////////////////
//	Created by Maciej Kopa 230451 and Dominik Czerwoniuk 230446.				//
//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>				//printf()
#include <dos.h> 				//pokeb(), inportb()
#include <conio.h> 				//clrscr(), getch(), _setcursortype(), kbhit()
#include <iostream.h>
#include <string.h>

// time structure e.g. hour.tm
struct dostime_t tm;
// used for storing keyboard input
int ch;
// determining current mode, if = 1 then we are in enhanced mode
char enhancedMode = 0;

// change enhanceMode value to opposite (we are treating it like a bool)
void switchMode(){
	switch (enhancedMode){
		case 0:
			enhancedMode = 1; break;
		case 1:
			enhancedMode = 0; break;
	}
}

// keyboard interrupt 09H
// check for F12 key press which changes modes
void interrupt (* OldHandler09H)(...);

void interrupt NewHandler09H(...)
{
	asm sti
	switch (ch){
		// 88 is f12 keyboard press code
		case 88:
			switchMode(); break;
	}

	OldHandler09H();
}

// system timer interrupt 08H
// programmed to display current time on the screen
void interrupt (* OldHandler08H)(...);

void interrupt NewHandler08H(...)
{
	asm sti
	// printing time
	pokeb(0xB800, 0x0000 + (1 * 80 * 2) + ((1) * 2) + 0, ((tm.hour)/10)+48);
	pokeb(0xB800, 0x0000 + (1 * 80 * 2) + ((2) * 2) + 0, ((tm.hour)%10)+48);
	pokeb(0xB800, 0x0000 + (1 * 80 * 2) + ((4) * 2) + 0, ((tm.minute)/10)+48);
	pokeb(0xB800, 0x0000 + (1 * 80 * 2) + ((5) * 2) + 0, ((tm.minute)%10)+48);
	pokeb(0xB800, 0x0000 + (1 * 80 * 2) + ((7) * 2) + 0, ((tm.second)/10)+48);
	pokeb(0xB800, 0x0000 + (1 * 80 * 2) + ((8) * 2) + 0, ((tm.second)%10)+48);
	// display enhanced time
	switch (enhancedMode){
		case 1:
		// wyswietl microsekundy !!!!!!
			pokeb(0xB800, 0x0000 + (1 * 80 * 2) + ((10) * 2) + 0, 'E');
			break;
		// tymczasowe drukowanie, pozniej usunac
		case 0:
			pokeb(0xB800, 0x0000 + (1 * 80 * 2) + ((10) * 2) + 0, 'b');
			break;
		}

	OldHandler08H();
}

// current system time using interrupts
int main(){
	clrscr();
	_setcursortype(_NOCURSOR);

	OldHandler09H = getvect(0x09);
	setvect(0x09, NewHandler09H);
	OldHandler08H = getvect(0x08);
	setvect(0x08, NewHandler08H);

	do{
		
		 _dos_gettime(&tm);
		 ch = inportb(0x60);
		 
		} while(ch != 1);
		
	setvect(0x09, OldHandler09H);
	setvect(0x08, OldHandler08H);
	clrscr();
	_setcursortype(_NORMALCURSOR);
	
	return 0;
}