#!/usr/bin/env sh

gcc Parser.c \
  && ./a.out ../input/add/Add.asm \
  && ./a.out ../input/max/MaxL.asm \
  && ./a.out ../input/pong/PongL.asm \
  && ./a.out ../input/rect/RectL.asm \

