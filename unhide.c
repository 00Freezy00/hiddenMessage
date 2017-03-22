//
// Created by jack on 18/03/17.
//

#include "hiddenMessage.h"

int append(char *aString, size_t stringLen, char aChar) {
    /*
     * Safely append the char into char array, return 1 if char array is full
     *
     */
    if (strlen(aString) + 1 >= stringLen) {
        return 1;
    }
    int len = (int) strlen(aString);
    aString[len] = aChar;
    aString[len + 1] = '\0';
    return 0;
}

char *revealMessage(FILE *in, int width, int height) {
    /*
     * reveals the message inside of the image
     * return: the message
     */
    int aChar;
    char messageBits[8];
    messageBits[0] = '\0';
    size_t maxMessage = sizeof(char) * (size_t) ((floor((width * height * 3) / 8)) + (width * height * 3) % 8 + 1);
    char *message = malloc(maxMessage);
    message[0] = '\0';
    while (1) {
        aChar = fgetc(in);
        if (feof(in)) {
            fprintf(stderr, "Is the message even in the image??");
            free(message);
            fclose(in);
            exit(-1);
        }
        switch (aChar & 1) {
            case 0:
                strcat(messageBits, "0");
                break;
            case 1:
                strcat(messageBits, "1");
                break;
            default:
                fprintf(stderr, "Is this a bit?");
                fclose(in);
                free(message);
                exit(-1);
        }
        if (strlen(messageBits) == 8) {//concat binary into a char
            aChar = (char) strtol(messageBits, 0, 2);
            messageBits[0] = '\0';
            if (append(message, maxMessage, aChar) == 1) {
                fprintf(stderr, "String has been exceeded");
                fclose(in);
                free(message);
                exit(-1);
            }
            if (aChar == '\0') {
                return message;
            }
        }
    }
}

int main(int argc, char *argv[]) {

    //Not enough or too many arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: hide 'filename'");
        exit(-1);
    }

    FILE *inputFile;
    inputFile = fopen(argv[1], "r");

    //file doesnt exist
    if (inputFile == NULL) {
        fprintf(stderr, "%s:error:Cannot open %s ", argv[0], argv[1]);
        perror(0);
        exit(-1);
    }

    //Checking the header
    skipComment(inputFile);
    checkImageFormat(inputFile, argv[2]);
    skipComment(inputFile);
    int *dimension = checkDimension(inputFile);
    skipComment(inputFile);
    checkColorChannel(inputFile);

    fseek(inputFile, -1, SEEK_CUR);

    char *message = revealMessage(inputFile, dimension[0], dimension[1]);

    printf("%s\n", message);
    free(message);

    fclose(inputFile);


    return 0;
}