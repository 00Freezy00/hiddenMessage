//
// Created by Jack 26956047
// Support library for hide and unhide
//
#include "hiddenMessage.h"
/**
 * Skip a the line that starts with #
 * @param imageFile: a file pointer, the input PPM file
 */
void skipComment(FILE *imageFile) {
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


/**
 * Checks the magic number, prints out error if it needs to.
 * @param in: a file pointer, the input PPM file
 * @param filename: input filename
 */
void checkImageFormat(FILE *in, char *filename) {
    char imageFormat[4];
    fscanf(in, "%3s ", imageFormat); //Space consumes \n
    if ((strcmp(imageFormat, "P6") != 0)) {
        fprintf(stderr, "Format Error: %s has to be in P6 ppm format\n", filename);
        fclose(in);
        exit(-1);
    }
}

/**
 * Check if the width and height is integer and is positive
 * @param in: a file pointer, the input PPM file
 * @return a len 2 integer array index 0 width, index 1 height
 */
int *checkDimension(FILE *in) {
    int width = 0, height = 0;
    fscanf(in,"%d",&width);
    skipComment(in);
    fscanf(in,"%d",&height);
    //Check height and width is a positive integer


    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Format error: height and width in the image header has to be an positive integer\n");
        fclose(in);
        exit(-1);
    }
    static int dimension[2];
    dimension[0] = width;
    dimension[1] = height;
    return dimension;
}


/**
 *  Checking maximum value for a color channel
 * @param in: a file pointer, the input PPM file
 */
void checkColorChannel(FILE *in) {
    char colorSpecStr[4];
    fscanf(in, "%4s ", colorSpecStr);
    int colorSpec;

    if ((colorSpec = atoi(colorSpecStr)) == 0) {
        fprintf(stderr, "Format Error: %s is not a integer", colorSpecStr);
        fclose(in);
        exit(-1);

    }
    if (colorSpec != 255) {
        fprintf(stderr, "Format Error: maximum value for a color channel needs to be 255\n");
        fclose(in);
        exit(-1);
    }
}

/**
 * Safely append the char into char array, return 1 if char array is full
 * @param aString: input string
 * @param stringLen: the len of input string
 * @param aChar: the char that are going to be appended
 * @return successful state
 */
int append(char *aString, size_t stringLen, char aChar) {
    if (strlen(aString) + 1 >= stringLen) {
        return 1;
    }
    int len = (int) strlen(aString);
    aString[len] = aChar;
    aString[len + 1] = '\0';
    return 0;
}