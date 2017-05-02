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

void checkImageFormat(FILE *in, char *filename);

int *checkDimension(FILE *in);

void checkColorChannel(FILE *in);




#endif //HIDDENMESSAGE_HIDDENMESSAGE_H
