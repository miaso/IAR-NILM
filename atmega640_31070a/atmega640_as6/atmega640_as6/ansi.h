/* 
   Various ANSI and other stdout functions
   (C) mtech.dk, 2009-10

   Implements a number of ANSI functionality such as
   clear screen and goto x,y
   Also implements a few other printing routines,
   such as printing of fixed-point values
*/

#ifndef _ansi_h_
#define _ansi_h_

  void clrscr();
  void gotoxy(char x, char y);
  void printFix(long i);

#endif
