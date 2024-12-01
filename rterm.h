/*
========================================================================
- HEADER -
Module to control some display routines that implement ANSI functions.
@author : Velorek
@version : 1.0
LAST MODIFIED : 29/11/2024 get_cursor_pos added / draw_window added
========================================================================
*/

#ifndef _RTERM_H_
#define _RTERM_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

/*====================================================================*/
/* COLOR CONSTANTS                                                    */
/*====================================================================*/

// Background colors low intensity
#define B_BLACK 40
#define B_RED 41
#define B_GREEN 42
#define B_YELLOW 43
#define B_BLUE 44
#define B_MAGENTA 45
#define B_CYAN 46
#define B_WHITE 47

// Foreground colors low intensity
#define F_BLACK 30
#define F_RED 31
#define F_GREEN 32
#define F_YELLOW 33
#define F_BLUE 34
#define F_MAGENTA 35
#define F_CYAN 36
#define F_WHITE 37
//#define F_WHITE 37
#define F_GREY 90

// Background colors high intensity
#define BH_BLACK 100
#define BH_RED 101
#define BH_GREEN 102
#define BH_YELLOW 103
#define BH_BLUE 104
#define BH_MAGENTA 105
#define BH_CYAN 106
#define BH_WHITE 107

// Foreground colors high intensity
#define FH_BLACK 90
#define FH_RED 91
#define FH_GREEN 92
#define FH_YELLOW 93
#define FH_BLUE 94
#define FH_MAGENTA 95
#define FH_CYAN 96
#define FH_WHITE 97

#define TRUE 1
#define FALSE 0

//Keys used.
#define K_ENTER 13
#define K_ENTER2 10
#define K_ESCAPE 27
#define K_UP_TRAIL "\e[A\0\0"
#define K_DOWN_TRAIL "\e[B\0\0"
#define K_RIGHT_TRAIL "\e[C\0\0"
#define K_LEFT_TRAIL "\e[D\0\0"
#define HOR_LINE 9472
#define VER_LINE 9474
#define UPPER_LEFT_CORNER 9484
#define LOWER_LEFT_CORNER 9492
#define UPPER_RIGHT_CORNER 9488
#define LOWER_RIGHT_CORNER 9496
#define UNICODEBAR1 0x2592
#define FILL_CHAR 0x20
#define MAX_TEXT 255 

//#define K_UP_ARROW 'A'      // K_ESCAPE + 'A' -> UP_ARROW
//#define K_DOWN_ARROW 'B'    // K_ESCAPE + 'B' -> DOWN_ARROW
typedef int BOOL;

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

void    pushTerm();
int     resetTerm();
int kbhit(int timeout_ms);
	
char     readch();
char     getch();
void    resetch();
//char    getch(void);
void    gotoxy(int x, int y);
void    outputcolor(int foreground, int background);
void    screencol(int x);
void    resetAnsi(int x);
int     get_terminal_dimensions(int *rows, int *columns);
void    showcursor();
void    hidecursor();
int     get_pos(int *y, int *x);
void    init_term();
void    close_term();
void write_ch(int x, int y, wchar_t ch, char backcolor, char forecolor);
void draw_window(int x1, int y1, int x2, int y2, int backcolor, int bordercolor, int titlecolor, BOOL  border, BOOL title, BOOL shadow);
void draw_transparent(int x1, int y1, int x2, int );
void write_ch(int x, int y, wchar_t ch, char backcolor, char forecolor);
void write_str(int x, int y, char *str, char backcolor, char forecolor);
int write_num(int x, int y, int num, char backcolor, char forecolor); 
int textbox(int wherex, int wherey, int displayLength,char *label, char text[MAX_TEXT], int backcolor, int labelcolor, int textcolor, BOOL locked);
#endif
