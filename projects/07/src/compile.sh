#!/usr/bin/env sh
gcc Parser.c VMtranslator.c -o VMtranslator && ./VMtranslator ../input/StackArithmetic/StackTest/StackTest.vm

