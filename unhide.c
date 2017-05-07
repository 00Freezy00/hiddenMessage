//
// Created by Jack 26956047
// unhide the message
//
#include "hiddenMessage.h"

int inputValidation(int argc, char *argv[]){
    if (argc < 2){
        fprintf(stderr, "See -help for more info'\n");
        exit(-1);
    }
    if (argc == 2){
        return 0;
    }else if (strcmp(argv[1],"-m")==0){
        if (argc == 3){
            return 1;
        }
        fprintf(stderr, "-m Usage: hide -m number-of-files basename output-base-name'\n");
        exit(-1);
    }else{
        fprintf(stderr, "See -help for more info'\n");
        exit(-1);
    }
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
    size_t maxMessage = sizeof(char) * (int) ((floor((width * height * 3) / 8)) + (width * height * 3) % 8 + 1);
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
            if (append(message, maxMessage, (char)aChar) == 1) {
                fprintf(stderr, "String has been exceeded\n");
                fclose(in);
                free(message);
                exit(-1);
            }
            messageBits[0] = '\0'; //Reset messageBits array
        }
    }
}



int revealMessageM(FILE *in, char* message, int maxMessage ,char* previousMessageBits) {
    int aChar;
    while (1) {
        aChar = fgetc(in);
        if (feof(in)) {
            return 1;
        }
        switch (aChar & 1) {
            case 0:
                strcat(previousMessageBits, "0");
                break;
            case 1:
                strcat(previousMessageBits, "1");
                break;
            default:
                fprintf(stderr, "Is this a bit?\n");
                fclose(in);
                free(message);
                exit(-1);
        }
        if (strlen(previousMessageBits) == 8) {//concat binary into a char
            aChar = (char) strtol(previousMessageBits, 0, 2);
            if (aChar == EOF) {
                return 0;
            }
            if (append(message, (size_t)maxMessage, (char)aChar) == 1) {
                fprintf(stderr, "String has been exceeded\n");
                fclose(in);
                free(message);
                exit(-1);
            }
            previousMessageBits[0] = '\0'; //Reset messageBits array
        }
    }
}

int main(int argc, char *argv[]) {

    //====================
    //Input argument validation
    int mode = 0;
    mode = inputValidation(argc, argv);

    if (mode == 0) {
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
        if (checkImageFormat(inputFile) == 1) {
            exit(-1);
        }
        skipComment(inputFile);
        int *dimension = checkDimension(inputFile);
        skipComment(inputFile);
        checkColorChannel(inputFile);

        fseek(inputFile, -1, SEEK_CUR);

        char *message = revealMessage(inputFile, dimension[0], dimension[1]);

        printf("%s", message);
        free(message);

        fclose(inputFile);
    }else if (mode == 1){
        int i = 0;
        char *message = malloc(sizeof(char) * 2);
        message[0] = '\0';
        char previousMessageBits[8];
        previousMessageBits[0] = '\0';
        int maxMessage = 0;
        while (1) {
            char inputFileName[strlen(argv[2])+9];
            snprintf(inputFileName, strlen(argv[2]) + 9, "%s-%03d.%s", argv[2], i,
                     "ppm");//Format string to basename-xxx.ppm

            FILE *inputFile;
            inputFile = fopen(inputFileName, "r");

            //file doesnt exist
            if (inputFile == NULL) {
                fprintf(stderr, "%s:error:Cannot decode series of ppm file \n", argv[0]);
                perror(0);
                free(message);
                exit(-1);
            }

            //Checking the header
            skipComment(inputFile);
            if (checkImageFormat(inputFile) == 1) {
                free(message);
                exit(-1);
            }
            skipComment(inputFile);
            int *dimension = checkDimension(inputFile);
            if (dimension[2] == 1){
                fclose(inputFile);
                free(message);
                exit(-1);
            }

            maxMessage += (int) floor((dimension[0] * dimension[1] * 3) / 8 + (dimension[0] * dimension[1] * 3) % 8);
            message = realloc(message,(size_t)maxMessage+1);
            if (message == NULL){
                fprintf(stderr, "Memory Error: Cannot allocate memory space\n");
                fclose(inputFile);
                free(message);
                exit(-1);
            }

            skipComment(inputFile);

            if (checkColorChannel(inputFile) == 1){
                fclose(inputFile);//Close the previous one, not include the current one, because it has been closed by checkColorChannel
                free(message);
                exit(-1);
            }

            fseek(inputFile, -1, SEEK_CUR);

            if (revealMessageM(inputFile,message,maxMessage,previousMessageBits) == 0){
                fclose(inputFile);
                break;
            }

            fclose(inputFile);
            ++i;
        }

        printf("%s",message);
        free(message);
    }
    return 0;
}