// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.
// Keyboard memory location: 24576
// Screen memory location:   16384

// Put your code here.

@16384
D=A
@screen_base
M=D

// screen_end = screen_base + 8192
@screen_base
D=M
@8192
D=D+A
@screen_end
M=D

// screen_curr = screen_base
@screen_base
D=M
@screen_curr
M=D
@color // 1 = black; 0 = white
M=0

(MAIN)
  @24576 // keyboard address
  D=M

  @SET_NOT_PRESSED
  D;JEQ
  @SET_PRESSED
  0;JMP

(SET_NOT_PRESSED)
  @pressed
  M=0
  @MAIN_1
  0;JMP

(SET_PRESSED)
  @pressed
  M=1
  @MAIN_1
  0;JMP

(MAIN_1)
  // check if (color != pressed) then goto RESET else goto MAIN_2
  @pressed
  D=M
  @color
  D=D-M
  @RESET
  D;JNE
  @MAIN_2
  0;JMP

(RESET)
  // color = pressed
  @pressed
  D=M
  @color
  M=D
  // screen_curr = screen_base
  @screen_base
  D=M
  @screen_curr
  M=D

  @MAIN_2
  0;JMP

(MAIN_2)
  // if screen_curr == screen_end
  @screen_end
  D=M
  @screen_curr
  D=D-M
  @MAIN
  D;JEQ

  D=1
  @color
  D=D&M
  @BLACKED
  D;JGT
  @WHITED
  0;JMP

(BLACKED)
  @screen_curr
  A=M
  M=-1
  @MAIN_3
  0;JMP

(WHITED)
  @screen_curr
  A=M
  M=0
  @MAIN_3
  0;JMP

(MAIN_3)
  @screen_curr
  M=M+1
  @MAIN
  0;JMP

