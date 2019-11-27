/* 
   Various ANSI and other stdout functions
   (C) mtech.dk, 2009-10

   Implements a number of ANSI functionality such as
   clear screen and goto x,y
   Also implements a few other printing routines,
   such as printing of fixed-point values
*/

#include <stdio.h>

void clrscr()
// ANSI clear screen
{
  printf("\33[2J"); // clrscr (33 = ESC in octal)
}

void gotoxy(char x, char y)
// ANSI goto xy
{
  printf("\33[%d;%dH",y,x); // gotoyx (33 = ESC in octal)
}

void printFix(long i)
// prints a 16.16 bit signed value (fixed comma representation with 16 bits for decimal part)
{
  if ((i & 0x80000000) != 0) {
    printf("-");
    i = ~i+1;
  }
  else
    printf(" ");

  printf("%ld.%04ld", i>>16, 10000*(unsigned long) (i & 0xFFFF) >> 16);
}
