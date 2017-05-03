//
// Created by Jack 26956047
// unhide the message
//
#include "hiddenMessage.h"
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
/**
 * reveals the message inside of the image
 * @param in : input file
 * @param width: dimension of the image
 * @param height: dimension of the image
 * @return the message
 */
char *revealMessage(FILE *in, int width, int height) {
    int aChar;
    char messageBits[8];
    messageBits[0] = '\0';
    size_t maxMessage = sizeof(char) * (size_t) ((floor((width * height * 3) / 8)) + (width * height * 3) % 8 + 1);
    char *message = malloc(maxMessage);
    message[0] = '\0';
    while (1) {
        aChar = fgetc(in);
        if (feof(in)) {
            fprintf(stderr, "Is the message even in the image??\n");
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
                fprintf(stderr, "Is this a bit?\n");
                fclose(in);
                free(message);
                exit(-1);
        }
        if (strlen(messageBits) == 8) {//concat binary into a char
            aChar = (char) strtol(messageBits, 0, 2);
            if (aChar == EOF) {
                return message;
            }
            if (append(message, maxMessage, aChar) == 1) {
                fprintf(stderr, "String has been exceeded\n");
                fclose(in);
                free(message);
                exit(-1);
            }
            messageBits[0] = '\0'; //Reset messageBits array
        }
    }
}

int main(int argc, char *argv[]) {

    //Not enough or too many arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: hide 'filename'\n");
        exit(-1);
    }

    FILE *inputFile;
    inputFile = fopen(argv[1], "r");

    //file doesnt exist
    if (inputFile == NULL) {
        fprintf(stderr, "%s:error:Cannot open %s \n", argv[0], argv[1]);
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