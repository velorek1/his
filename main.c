//TUI BASH HISTORY [ħis]
//Coded by v3l0r3k
//Date: Novemeber 2024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rterm.h"
#include "listc.h"
#include "keyb.h"
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define HISTORY_FILE "~/.bash_history"

//GLOBALS
int globalCursorX=0, globalCursorY=0;
int termR=0, termC=0;
int ntermR=0, ntermC=0;
int count=0;
int filterCount=0;
SCROLLDATA scrollData;

char *strdup(const char *s) {
    char *copy = malloc(strlen(s) + 1);
    if (copy) {
        strcpy(copy, s);
    }
    return copy;
}

ssize_t cgetline(char **lineptr, size_t *n, FILE *stream) {
    if (lineptr == NULL || n == NULL || stream == NULL) {
        return -1;  // Invalid arguments
    }

    const size_t CHUNK_SIZE = 128;  // Size to grow buffer incrementally
    size_t total_length = 0;        // Total length of the line
    char buffer[CHUNK_SIZE];        // Temporary buffer for reading chunks
    char *new_lineptr = NULL;

    while (fgets(buffer, sizeof(buffer), stream)) {
        size_t buffer_length = strlen(buffer);

        // Ensure the buffer is large enough
        if (*lineptr == NULL || total_length + buffer_length + 1 > *n) {
            *n = total_length + buffer_length + 1;  // Update size needed
            new_lineptr = realloc(*lineptr, *n);    // Expand buffer
            if (new_lineptr == NULL) {
                return -1;  // Memory allocation failure
            }
            *lineptr = new_lineptr;
        }

        // Append the read buffer to the line
        memcpy(*lineptr + total_length, buffer, buffer_length);
        total_length += buffer_length;

        // Check if the line is complete (contains a newline)
        if ((*lineptr)[total_length - 1] == '\n') {
            break;
        }
    }

    // Check if anything was read
    if (total_length == 0) {
        return -1;  // No input or error
    }

    // Null-terminate the line
    (*lineptr)[total_length] = '\0';
    return total_length;
}

// Function to expand '~' in file paths
char *expand_path(const char *path) {
    if (path[0] != '~') return strdup(path);
    const char *home = getenv("HOME");
    if (!home) return NULL;
    char *expanded = malloc(strlen(home) + strlen(path));
    if (!expanded) return NULL;
    sprintf(expanded, "%s%s", home, path + 1);
    return expanded;
}

// Function to read Bash history into a list of strings
char **read_bash_history(int *count) {
    FILE *file;
    char **lines = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    *count = 0;

    char *expanded_path = expand_path(HISTORY_FILE);
    if (!expanded_path) {
        fprintf(stderr, "Failed to resolve history file path\n");
        return NULL;
    }

    file = fopen(expanded_path, "r");
    free(expanded_path);

    if (!file) {
        perror("Error opening history file");
        return NULL;
    }

    while ((read = cgetline(&line, &len, file)) != -1) {
        lines = realloc(lines, sizeof(char *) * (*count + 1));
        if (!lines) {
            perror("Memory allocation error");
            fclose(file);
            return NULL;
        }
        line[read - 1] = '\0'; // Remove newline character
        lines[*count] = strdup(line);
        (*count)++;
    }

    free(line);
    fclose(file);
    return lines;
}
int options(){
  char och=0;
  int keypressed = 0;
  int i = 0;
  int whereX = 0, whereY = 0;
  int xDIR = 0, yDIR = 0;
  resetAnsi(0);
  draw_window(20, (termR/2) - 6, termC-20, (termR/2) +6, B_YELLOW,F_BLACK, B_WHITE,1,1,0);
  gotoxy((termC/2)-10,(termR/2)-5);
  outputcolor(F_BLACK,B_WHITE);
  printf("Interactive Bash History\n");
  gotoxy(22,(termR/2)-4);
  outputcolor(F_BLACK,B_YELLOW);
  printf("[+] KEYS:\n");
  gotoxy(22,(termR/2)-3);
  outputcolor(F_BLACK,B_YELLOW);
  printf("- [ENTER]: Run command.\n");
  gotoxy(22,(termR/2)-2);
  outputcolor(F_BLACK,B_YELLOW);
  printf("- [TAB]: Type command.\n");
  gotoxy(22,(termR/2)-1);
  outputcolor(F_BLACK,B_YELLOW);
  printf("- [ESC]: Exit.\n");
  gotoxy(22,(termR/2));
  outputcolor(F_BLACK,B_YELLOW);
  printf("- [x]: Invert list.\n");
  gotoxy(22,(termR/2)+1);
  outputcolor(F_BLACK,B_YELLOW);
  printf("- [f/r]: Add/remove filter.\n");
   gotoxy(22,(termR/2)+2);
  outputcolor(FH_BLACK,B_YELLOW);
  printf(":: his v0.1 - 2024 ::\n");
  for(i=22; i<=termC-20;i++){
    gotoxy(i,(termR/2)+3);
    outputcolor(F_BLACK,B_YELLOW);
    printf("%lc", HOR_LINE);
  }
  gotoxy(22,(termR/2)+7);
  outputcolor(FH_WHITE,B_YELLOW);
  printf("PRESS ANY KEY...\n");
  i=1;
     whereX = 22;
     xDIR = 1;
     yDIR = 1;
     whereY = (termR/2)+4;
  do{
	 keypressed= kbhit(100);
	 //wait for keypress
	 if (keypressed)
		 och = readch();
	//Animation
        gotoxy(whereX,whereY);
        outputcolor(F_WHITE,B_YELLOW);
        printf("                \n");
 	if (whereX == termC-20-16) xDIR = -1;
        if (whereX == 22) xDIR = 1;
        if (whereY == (termR/2)+4)  {yDIR = 1;}
        if (whereY == (termR/2)+6) {yDIR = -1;}
        whereX = whereX + xDIR;
        whereY = whereY + yDIR;

        get_terminal_dimensions (&termR,&termC);
        if ((termR != ntermR) || (termC != ntermC))
	{
		//screen was resized
		return -1;

	}
        gotoxy(whereX,whereY);
        outputcolor(FH_WHITE,B_BLACK);
        
	printf("Coded by v3l0r3k\n");
  } while(keypressed != 1);
  och++;
  return 0;
}
void mainwindow(){

  draw_window(20, (termR/2) - 6, termC-20, (termR/2) +6, B_CYAN,F_BLACK, B_WHITE,1,1,0);
  gotoxy((termC/2)-10,(termR/2)-5);
  outputcolor(F_BLACK,B_WHITE);
  printf("Interactive Bash History");
  gotoxy(22,(termR/2)+7);
  outputcolor(FH_WHITE,B_CYAN);
  printf("^v: SCROLL | SPACE : INFO");

}
void load_history(int ascending, const char *instrstr) {
    // Load list in ascending or descending order
    int i = 0;
    char **history = read_bash_history(&count);
    filterCount =0;
    if (!history) {
        close_term();
        showcursor();
        fprintf(stderr, "Failed to read Bash history\n");
        exit(0);
    }

    // Iterate through history entries based on the order
    if (ascending == 1) {
        for (i = 0; i < count; i++) {
            // Check if the entry contains the substring (if filtering is enabled)
            if (instrstr == NULL || instrstr[0] == '\0' || strstr(history[i], instrstr)) {
                listBox1 = addatend(listBox1, newitem(history[i], -1, -1, -1, -1));
            }
	    if (strstr(history[i], instrstr)) filterCount++;
            free(history[i]); // Free individual strings
        }
    } else {
        for (i = count - 1; i >= 0; i--) {
            if (instrstr == NULL || instrstr[0] == '\0' || strstr(history[i], instrstr)) {
                listBox1 = addatend(listBox1, newitem(history[i], -1, -1, -1, -1));
            }
	    if (strstr(history[i], instrstr)) filterCount++;
            free(history[i]); // Free individual strings
        }
    }
   free(history); // Free the array
   if (filterCount == 0) listBox1 = addatend(listBox1, newitem("Empty list", -1,-1,-1,-1));
}

void search(char csearch[MAX_TEXT]){
       textbox(21, (termR/2)+7-1, 20,"Filter:", csearch, B_BLACK, F_WHITE, F_WHITE, 1);
}

int main() {
    size_t k = 0;
    char ch=0;
    char command[500];
    int invert = 0;
   char csearch[MAX_TEXT];
   get_pos(&globalCursorY, &globalCursorX);
   get_terminal_dimensions (&termR,&termC);
   //check screen size
   if ((termR < 15) || (termC<70)){
         fprintf(stderr, "Screen is too small to display.\n");
        return EXIT_FAILURE;
   }
   ntermR = termR;
   ntermC = termC;
   init_term();
   hidecursor();   

    resetScrollData(&scrollData);
    setselectorLimit(termC-41);
 csearch[0] = '\0';
 do{ 
    load_history(invert,csearch);
    mainwindow();
    gotoxy((termC/2)-13,(termR/2)-5);
    outputcolor(F_BLACK,B_WHITE);
 
    printf("Interactive Bash History (%d)\n", count);
 
    ch = listBox(listBox1, 22, (termR/2)-4, &scrollData, B_CYAN, F_BLACK, B_BLACK,
	       FH_WHITE, 11, VERTICAL, 1,1);
   if (ch == K_ENTER) break;
   if (ch == K_TAB) break;
   if (ch == 'x') {if (invert == 1) invert = 0; else invert = 1;}
   if (ch == 'f') {search(csearch); if (strlen(csearch)==0 || filterCount==0) csearch[0]='\0';}
   if (ch == 'r') {csearch[0]='\0';}

   if (ch == K_SPACE) {
	   ch = 0; 
	  if (options() == -1 ) {scrollData.screenChanged = 1; scrollData.itemIndex = -1; break;}
  
   }
   if (listBox1!= NULL) removeList(&listBox1);  
 
} while (scrollData.lastch != K_ESCAPE);   
   showcursor();   
   ch++;
   draw_transparent(20, (termR/2) - 6, termC-20, (termR/2) +6);
   gotoxy(globalCursorX, globalCursorY-2);
    close_term();
    
   if (scrollData.screenChanged == 1) system("clear");
    printf("\n");
    //printf("First run: %d:%d:%c\n",globalCursorX,globalCursorY,ch);
   //write(STDOUT_FILENO, scrollData.item, strlen(scrollData.item)); 
   if (scrollData.itemIndex != -1)  {
     strcpy(command, "\0"); 
     strcat(command, scrollData.item);
     if (scrollData.lastch!=K_TAB) strcat(command, "\n");
     //system(command);
    for (k = 0; k < strlen(command); k++) {
     ioctl(STDIN_FILENO, TIOCSTI, &command[k]);
     printf("\r"); //avoids echo
    }
   }
    return EXIT_SUCCESS;
}
