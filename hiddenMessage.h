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
    int commentHeader;
    char ignore[1024];
    while (!feof(imageFile)) {
        commentHeader = fgetc(imageFile);
        if (commentHeader == '#') {
            fgets(ignore, sizeof(ignore), imageFile);//Skip a line
        } else {
            fseek(imageFile, -1, SEEK_CUR);//go back 1 byte
            return;
        }
    }
}


char *stringToBinary(char *aString) {
    if (aString == NULL) return 0; /* no input string */
    char *binary = malloc(
            (strlen(aString) + 1) * 8 + 1); // each char is one byte (8 bits) and + 1 at the end for null terminator
    binary[0] = '\0';
    for (size_t i = 0; i < strlen(aString); ++i) {
        char aChar = aString[i];
        for (int j = 7; j >= 0; j = j - 1) {
            if (aChar & (1 << j)) {
                strcat(binary, "1");
            } else {
                strcat(binary, "0");
            }
        }
    }
    strcat(binary, "00000000");
    free(aString);
    return binary;
}

int hideABit(int charInt, char bit) {
    switch (bit) {
        case '0':
            charInt &= ~(1 << 0);
            return charInt;
        case '1':
            charInt |= 1 << 0;
            return charInt;
        default:
            fprintf(stderr, "Is this a bit?");
            exit(-1);
    }
}

void checkImageFormat(FILE *in, char *filename) {
    //Incorrect image format
    char imageFormat[4];
    fscanf(in, "%3s ", imageFormat); //Space consume \n
    if ((strcmp(imageFormat, "P6") != 0)) {
        fprintf(stderr, "Format Error: %s has to be in P6 ppm format", filename);
        fclose(in);
        exit(-1);
    }
}

int *checkDimension(FILE *in) {
    char strWidth[10], strHeight[10];
    fscanf(in, "%10s %10s ", strWidth, strHeight);

    //Check height and width is a positive integer
    int width = atoi(strWidth), height = atoi(strHeight);

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

char *checkMessageSize(FILE *in, int width, int height) {
    //getting hidden hiddenMessage
    int maxMessage = ((int) (floor((width * height * 3) / 8)) + (width * height * 3) % 8);// grab one more byte to see if user input exceeds the spec
    char * hiddenMessage = malloc(sizeof(char) * maxMessage + 1);//decimal
    do {
        printf("Please enter a hiddenMessage to be hidden inside of the image: ");
        fgets(hiddenMessage, maxMessage + 1, stdin);
        hiddenMessage[strcspn(hiddenMessage, "\n")] = '\0';
    } while (strcmp(hiddenMessage, "") == 0);

    //Check if the hiddenMessage can fit in the image
    if (strlen(hiddenMessage) > maxMessage - 1) {
        fprintf(stderr, "Size error: hidden Message is too large for this image");
        fclose(in);
        free(hiddenMessage);
        exit(-1);
    }

    return stringToBinary(hiddenMessage);// convert it to binary string
}

void checkColorChannel(FILE *in) {
    //Checking maximum value for a color channel
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

void hideTheMessage(FILE *out, FILE *in, char *hiddenMessageBin) {
    //Assuming input validation has passed
    int aChar;

    for (int i = 0; i < strlen(hiddenMessageBin); i = i + 1) {
        aChar = fgetc(in);
        aChar = hideABit(aChar, hiddenMessageBin[i]);
        fwrite(&aChar, 1, sizeof(unsigned char), out);
    }

    while (1) {
        aChar = fgetc(in);
        if (feof(in)) {
            return;
        }
        fwrite(&aChar, 1, sizeof(unsigned char), out);
    }
}


#endif //HIDDENMESSAGE_HIDDENMESSAGE_H
