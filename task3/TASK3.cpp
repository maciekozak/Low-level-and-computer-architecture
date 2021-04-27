// Created by Maciej Kopa 230451 and Dominik Czerwoniuk 230446.

#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <iostream.h>

#define ArrowUp 72
#define ArrowDown 80
#define Space 57
#define Esc 1

typedef unsigned char BYTE;
typedef unsigned int  WORD;
typedef unsigned int  UINT;
typedef unsigned long DWORD;
typedef unsigned long LONG;


struct BITMAPFILEHEADER
{
 UINT  bfType;
 DWORD bfSize;
 UINT  bfReserved1;
 UINT  bfReserved2;
 DWORD bfOffBits;
};

struct BITMAPINFOHEADER
{
 DWORD biSize;
 LONG  biWidth;
 LONG  biHeight;
 WORD  biPlanes;
 WORD  biBitCount;
 DWORD biCompression;
 DWORD biSizeImage;
 LONG  biXPelsPerMeter;
 LONG  biYPelsPerMeter;
 DWORD biClrUsed;
 DWORD biClrImportant;
};

struct RGBQUAD
{
 BYTE rgbBlue;
 BYTE rgbGreen;
 BYTE rgbRed;
 BYTE rgbReserved;
};

BITMAPFILEHEADER bmfh;
BITMAPINFOHEADER bmih;
RGBQUAD palette[256];
BYTE *video_memory = (BYTE *)0xA0000000L;
FILE *bitmap_file;

void set_video_mode(int mode)
{
	REGPACK regs;
	regs.r_ax = mode;
	intr(0x10, &regs);
}

void set_video_palette()
{
	outportb(0x03C8, 0);
	for (int i = 0; i < bmih.biClrUsed; i++)
	{
		outp(0x03C9, palette[i].rgbRed   * 63 / 255);
		outp(0x03C9, palette[i].rgbGreen * 63 / 255);
		outp(0x03C9, palette[i].rgbBlue  * 63 / 255);
	}
}

void display_image(char *file_name){
	bitmap_file = fopen(file_name, "rb");
	fread(&bmfh, sizeof(bmfh), 1, bitmap_file);
	fread(&bmih, sizeof(bmih), 1, bitmap_file);
	fread(&palette[0], bmih.biClrUsed * sizeof(RGBQUAD), 1, bitmap_file);
	set_video_palette();
	// displaying image properly, not inversed
	for(int i = 199; i >= 0; i--){
		fread(video_memory + i * 320, 320, 1, bitmap_file);
	}
	fclose(bitmap_file);
}

// we need extended range on colors to maintain original color data as we approoach total white/black image
struct MYRGBQUAD
{
	int rgbBlue;
	int rgbGreen;
	int rgbRed;
};
MYRGBQUAD my_palette[256];

void fill_extended_palette() {
	for (int i = 0; i < bmih.biClrUsed; i++) {
		my_palette[i].rgbRed = palette[i].rgbRed;
		my_palette[i].rgbGreen = palette[i].rgbGreen;
		my_palette[i].rgbBlue = palette[i].rgbBlue;
	}
}

void brightness (char change) {
	// port sterujący paletą kolorów
	outportb(0x03C8, 0);
		
		//check if we can modify brightness further (full white or black image check)
		int counter_whites = 0;
		int counter_blacks = 0;
		for (int i = 0; i < bmih.biClrUsed; i++) {
			if (my_palette[i].rgbRed >= 255)	counter_whites++;
			if (my_palette[i].rgbGreen >= 255)	counter_whites++;
			if (my_palette[i].rgbBlue >= 255)	counter_whites++;
			
			if (my_palette[i].rgbRed <= 0)		counter_blacks++;
			if (my_palette[i].rgbGreen <= 0)	counter_blacks++;
			if (my_palette[i].rgbBlue <= 0)		counter_blacks++;
		}
		
		// if image is fully white only decresing of brightness is possible
		// if image if fully black only increase of brightness is possible
		if ( !((counter_whites == (bmih.biClrUsed * 3)) && change > 0) &&
		!((counter_blacks == (bmih.biClrUsed * 3)) && change < 0) ) {
			for (int i = 0; i < bmih.biClrUsed; i++) {
				// we need to set ALL colours
				my_palette[i].rgbRed += change;
				if ( ( 0 <= (my_palette[i].rgbRed) && (my_palette[i].rgbRed) < 256) ) {
					outp(0x03C9, (my_palette[i].rgbRed) * 63 / 255);
				}
				else {
					if (my_palette[i].rgbRed > 255) outp(0x03C9, 63);
					else outp(0x03C9, 0);
				}
				my_palette[i].rgbGreen += change;
				if ( ( 0 <= (my_palette[i].rgbGreen) && (my_palette[i].rgbGreen) < 256) ) {
					outp(0x03C9, (my_palette[i].rgbGreen) * 63 / 255);
				}
				else {
					if (my_palette[i].rgbGreen > 255) outp(0x03C9, 63);
					else outp(0x03C9, 0);
				}
				my_palette[i].rgbBlue += change;
				if ( ( 0 <= (my_palette[i].rgbBlue) && (my_palette[i].rgbBlue) < 256) ) {
					outp(0x03C9, (my_palette[i].rgbBlue) * 63 / 255);
				}
				else {
					if (my_palette[i].rgbBlue > 255) outp(0x03C9, 63);
					else outp(0x03C9, 0);
				}
			}
		}
}

void invert() {
	outportb(0x03C8, 0);
	// -1 because we start counting colours from 0 to e.g. 255
	int centre = (bmih.biClrUsed / 2) - 1;
	
	for (int i = 0; i < bmih.biClrUsed; i++) {
			// inverting colors with respect to centre of range e.g. 0..255 -> 127
			my_palette[i].rgbRed  = centre - (my_palette[i].rgbRed - centre);
			my_palette[i].rgbGreen = centre - (my_palette[i].rgbGreen - centre);
			my_palette[i].rgbBlue = centre - (my_palette[i].rgbBlue - centre);
			
			// printing color if it is in range, if not print white or black
				if ( ( 0 <= (my_palette[i].rgbRed) && (my_palette[i].rgbRed) < 256) ) {
					outp(0x03C9, (my_palette[i].rgbRed) * 63 / 255);
				}
				else {
					if (my_palette[i].rgbRed >= 255) outp(0x03C9, 63);
					else outp(0x03C9, 0);
				}
				if ( ( 0 <= (my_palette[i].rgbGreen) && (my_palette[i].rgbGreen) < 256) ) {
					outp(0x03C9, (my_palette[i].rgbGreen) * 63 / 255);
				}
				else {
					if (my_palette[i].rgbGreen >= 255) outp(0x03C9, 63);
					else outp(0x03C9, 0);
				}
				if ( ( 0 <= (my_palette[i].rgbBlue) && (my_palette[i].rgbBlue) < 256) ) {
					outp(0x03C9, (my_palette[i].rgbBlue) * 63 / 255);
				}
				else {
					if (my_palette[i].rgbBlue >= 255) outp(0x03C9, 63);
					else outp(0x03C9, 0);
				}
	}
	delay(300);
}

void main(int argc, char *argv[])
{
	if (argc != 2) 
	{ 
		cout << "Incorrect number of arguments." << endl;
		return;
	}
	set_video_mode(0x13);
	display_image(argv[1]);
	fill_extended_palette();

	// in loop manipulate image until esc is pressed
	char ch;
	do {	
		ch = inportb(0x60);
		switch (ch){
			case ArrowUp:
				brightness(1); break;
			case ArrowDown:
				brightness(-1); break;
			case Space:
				invert(); break;
		} 
	} while(ch != Esc);
	
	clrscr();
	set_video_mode(0x03);
	delay(500);
}
