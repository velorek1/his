/*
======================================================================
Module to control some display routines that implement ANSI functions.
on LINUX Terminals

@author : Velorek (routines extracted from the internet)
@version : 1.0

LAST MODIFIED : 29/11/2024 get_cursor_pos added / draw_window added
======================================================================
*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <locale.h>
#include <sys/ioctl.h>
#include <poll.h>
#include "rterm.h"
#include "keyb.h"
/*====================================================================*/
/* GLOBAL VARIABLES                                                   */
/*====================================================================*/

struct winsize max;
static struct termios term, failsafe;
static int peek_character = -1;

/*====================================================================*/
/* FUNCTIONS - CODE                                                   */
/*====================================================================*/

/*-------------------------------------*/
/* Initialize new terminal i/o settings*/
/*-------------------------------------*/
void pushTerm() {
//Save terminal settings in failsafe to be retrived at the end
  tcgetattr(0, &failsafe);
}

/*---------------------------*/
/* Reset terminal to failsafe*/
/*---------------------------*/
int resetTerm() {
  //tcsetattr(0, TCSANOW, &failsafe);
  /* flush and reset */
  if (tcsetattr(0,TCSAFLUSH,&failsafe) < 0) return -1;
  return 0;
}


/*--------------------------------------.*/
/* Detect whether a key has been pressed.*/
/*---------------------------------------*/

int kbhit(int timeout_ms)
{
    struct pollfd fds = {STDIN_FILENO, POLLIN, 0};
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    int ret = poll(&fds, 1, timeout_ms);
    fcntl(STDIN_FILENO, F_SETFL, 0);
    if (ret > 0) {
        return 1;
    } else if (ret == 0) {
        return 0; // timeout occurred
    } else {
        return -1; // error occurred
    }
}

/*----------------------*/
/*Read char with control*/
/*----------------------*/
char readch() {
  char    ch;
 if(peek_character != -1) {
    ch = peek_character;
    peek_character = -1;
    return ch;
  }
  read(0, &ch, 1);
  return ch;
}

void resetch() {
//Clear ch
  term.c_cc[VMIN] = 0;
  tcsetattr(0, TCSANOW, &term);
  peek_character = 0;
}

/*----------------------------------*/
/* Move cursor to specified position*/
/*----------------------------------*/
void gotoxy(int x, int y) {
  printf("%c[%d;%df", 0x1B, y, x);
}
/*----------------------------------*/
/* Get cursor  position             */
/*----------------------------------*/

int get_pos(int *y, int *x) {

 char buf[30]={0};
 int ret, i, pow;
 char ch;

*y = 0; *x = 0;

 struct termios term, restore;

 tcgetattr(0, &term);
 tcgetattr(0, &restore);
 term.c_lflag &= ~(ICANON|ECHO);
 tcsetattr(0, TCSANOW, &term);

 write(1, "\033[6n", 4);

 for( i = 0, ch = 0; ch != 'R'; i++ )
 {
    ret = read(0, &ch, 1);
    if ( !ret ) {
       tcsetattr(0, TCSANOW, &restore);
       //fprintf(stderr, "getpos: error reading response!\n");
       return 1;
    }
    buf[i] = ch;
    //printf("buf[%d]: \t%c \t%d\n", i, ch, ch);
 }

 if (i < 2) {
    tcsetattr(0, TCSANOW, &restore);
    //printf("i < 2\n");
    return(1);
 }

 for( i -= 2, pow = 1; buf[i] != ';'; i--, pow *= 10)
     *x = *x + ( buf[i] - '0' ) * pow;

 for( i-- , pow = 1; buf[i] != '['; i--, pow *= 10)
     *y = *y + ( buf[i] - '0' ) * pow;

 tcsetattr(0, TCSANOW, &restore);
 return 0;
}
/*---------------------*/
/* Change colour output*/
/*---------------------*/
void outputcolor(int foreground, int background) {
  printf("%c[%d;%dm", 0x1b, foreground, background);
}

/*-----------------------------------------------------*/
/* Change the whole color of the screen by applying CLS*/
/*-----------------------------------------------------*/
void screencol(int x) {
  gotoxy(0, 0);
  outputcolor(0, x);
  printf("%c[2J", 0x1b);
  outputcolor(0, 0);
}

/*-----------------------*/
/* Reset ANSI ATTRIBUTES */
/*-----------------------*/
void resetAnsi(int x) {
  switch (x) {
    case 0:         //reset all colors and attributes
      printf("%c[0m", 0x1b);
      break;
    case 1:         //reset only attributes
      printf("%c[20m", 0x1b);
      break;
    case 2:         //reset foreg. colors and not attrib.
      printf("%c[39m", 0x1b);
      break;
    case 3:         //reset back. colors and not attrib.
      printf("%c[49m", 0x1b);
      break;
    default:
      break;
  }
}

/*------------------------*/
/* Get terminal dimensions*/
/*------------------------*/
int get_terminal_dimensions(int *rows, int *columns) {
  ioctl(0, TIOCGWINSZ, &max);
  *columns = max.ws_col;
  *rows = max.ws_row;
  return 0;
}

/*--------------------------*/
/* Ansi function hide cursor*/
/*--------------------------*/
void hidecursor() {
  printf("\e[?25l");
}

/*--------------------------*/
/* Ansi function show cursor*/
/*--------------------------*/
void showcursor() {
  printf("\e[?25h");
}

/*--------------------------*/
/* Set up terminal          */
/*--------------------------*/

//For code simplification purposes

void init_term(){
  hidecursor();
  pushTerm();
  resetch();
  //Setup unicode  
  setlocale(LC_ALL, "");
}


void close_term(){
  showcursor();
  resetTerm();
  outputcolor(F_WHITE, B_BLACK);
  //screencol(B_BLACK);
  resetAnsi(0);
  printf("\n");
}

void write_ch(int x, int y, wchar_t ch, char backcolor, char forecolor) {
//Write Unicode char to screen raw
   resetAnsi(0);
   gotoxy(x+1, y+1);
   outputcolor(forecolor, backcolor);
   printf("%lc", ch);  //unicode
}

void draw_window(int x1, int y1, int x2, int y2, int backcolor, int bordercolor, int titlecolor, BOOL  border, BOOL title, BOOL shadow) {
/*
   Draw a box on screen
 */
  int     i, j;
  wchar_t ch=FILL_CHAR;
  i = x1;
  j = y1;
  //shadow
  resetAnsi(0);
  if (shadow == TRUE){
  for(j = y1 + 1; j <= y2 + 1; j++)
    for(i = x1 + 1; i <= x2 + 1; i++)
    {
      write_ch(i, j, ch, B_BLACK, F_WHITE);
    }
  }
  //window
  for(j = y1; j <= y2; j++)
    for(i = x1; i <= x2; i++)
      write_ch(i, j, FILL_CHAR, backcolor, bordercolor);
 
 //borders
  if(border == TRUE) {
    for(i = x1; i <= x2; i++) {
      //upper and lower borders
      write_ch(i, y1, HOR_LINE, backcolor, bordercolor);   //horizontal line box-like char
      write_ch(i, y2, HOR_LINE, backcolor, bordercolor);
    }
    for(j = y1; j <= y2; j++) {
      //left and right borders
      write_ch(x1, j, VER_LINE, backcolor, bordercolor);   //vertical line box-like char
      write_ch(x2, j, VER_LINE, backcolor, bordercolor);
    }
    write_ch(x1, y1, UPPER_LEFT_CORNER, backcolor, bordercolor);   //upper-left corner box-like char
    write_ch(x1, y2, LOWER_LEFT_CORNER, backcolor, bordercolor);   //lower-left corner box-like char
    write_ch(x2, y1, UPPER_RIGHT_CORNER, backcolor, bordercolor);  //upper-right corner box-like char
    write_ch(x2, y2, LOWER_RIGHT_CORNER, backcolor, bordercolor);  //lower-right corner box-like char
  }

  if (title == TRUE) {
    for(i = x1+1; i <= x2-1; i++)
      write_ch(i, y1, ' ', titlecolor, titlecolor);
  }
}
void draw_transparent(int x1, int y1, int x2, int y2) {
/*
   Draw a box on screen
 */
  int     i, j;
  i = x1;
  j = y1;
  //shadow
  resetAnsi(0);
 //window
  for(j = y1; j <= y2+1; j++)
    for(i = x1; i <= x2+1; i++){
      gotoxy(i,j);
      printf("%c", FILL_CHAR);
    }
}

