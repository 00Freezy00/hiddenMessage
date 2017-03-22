//
// Created by jack on 19/03/17.
//

#ifndef HIDDENMESSAGE_HIDDENMESSAGE_H
#define HIDDENMESSAGE_HIDDENMESSAGE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
void skipComment(FILE *imageFile) {
    /*
     * Skip a the line that starts with #
     * imageFile: a file pointer, the input PPM file
     */
    int commentHeader;
    char ignore[1024];
    while (!feof(imageFile)) {
        commentHeader = fgetc(imageFile);
        if (commentHeader == '#') {
            fgets(ignore, 1024, imageFile);//Skip a line
        } else {
            fseek(imageFile, -1, SEEK_CUR);//go back 1 byte
            return;
        }
    }
}



void checkImageFormat(FILE *in, char *filename) {
    /*
     * Checks the magic number, prints out error if it needs to.
     * in: a file pointer, the input PPM file
     * filename: input filename
     */
    char imageFormat[4];
    fscanf(in, "%3s ", imageFormat); //Space consumes \n
    if ((strcmp(imageFormat, "P6") != 0)) {
        fprintf(stderr, "Format Error: %s has to be in P6 ppm format", filename);
        fclose(in);
        exit(-1);
    }
}

int *checkDimension(FILE *in) {
    /*
     * Check if the width and height is integer and is positive
     * in: a file pointer, the input PPM file
     * return: a len 2 integer array index 0 width, index 1 height
     */
    int width = 0, height = 0;
    fscanf(in,"%d",&width);
    skipComment(in);
    fscanf(in,"%d",&height);
    //Check height and width is a positive integer


    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Format error: height and width in the image header has to be an positive integer");
        fclose(in);
        exit(-1);
    }
    static int dimension[2];
    dimension[0] = width;
    dimension[1] = height;
    return dimension;
}



void checkColorChannel(FILE *in) {
    /*
     * Checking maximum value for a color channel
     * in: a file pointer, the input PPM file
     */
    char colorSpecStr[4];
    fscanf(in, "%4s ", colorSpecStr);
    int colorSpec;

    if ((colorSpec = atoi(colorSpecStr)) == 0) {
        fprintf(stderr, "Format Error: %s is not a integer", colorSpecStr);
        fclose(in);
        exit(-1);

    }
    if (colorSpec != 255) {
        fprintf(stderr, "Format Error: maximum value for a color channel needs to be 255");
        fclose(in);
        exit(-1);
    }
}




#endif //HIDDENMESSAGE_HIDDENMESSAGE_H
