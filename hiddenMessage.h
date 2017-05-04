//
// Created by jack on 19/03/17.
//

#ifndef HIDDENMESSAGE_HIDDENMESSAGE_H
#define HIDDENMESSAGE_HIDDENMESSAGE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void skipComment(FILE *imageFile);

int checkImageFormat(FILE *in);

int *checkDimension(FILE *in);

int checkColorChannel(FILE *in);

int append(char *aString, size_t stringLen, char aChar);




#endif //HIDDENMESSAGE_HIDDENMESSAGE_H
