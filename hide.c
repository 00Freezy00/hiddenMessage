//
//Created by Jack 26956047
//Hide the message
#include "hiddenMessage.h"
#include <stdbool.h>
#include <ctype.h>

/**
 * hide a bit in a byte
 * @param a char in integer form
 * @param the bit that are going to hide in the char
 * @return  char with a bit hidden in it
 */
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

/**
 * Check the input message is within the limit of the image
 * @param in a file pointer, the input PPM file
 * @param width: dimension from the image header
 * @param height: dimension from the image header
 * @return hidden message in a binary string
 */
char *checkMessageSize(FILE *in, int width, int height) {
    //getting hidden hiddenMessage
    int maxMessage = ((int) (floor((width * height * 3) / 8)) + (width * height * 3) % 8);
    char * hiddenMessage = malloc(sizeof(char) * maxMessage);
    if (hiddenMessage == NULL){
        fprintf(stderr, "Memory Error: Cannot allocate memory space\n");
        fclose(in);
        exit(-1);
    }
    hiddenMessage[0] = '\0';
    printf("Please enter a hiddenMessage to be hidden inside of the image: \n");


    int aChar = 0;
    while (aChar != EOF){
        aChar = fgetc(stdin);
        if (append(hiddenMessage,(size_t)maxMessage,(char)aChar) != 0){
            fprintf(stderr, "Input error: The message is too large for this image\n");
            fclose(in);
            free(hiddenMessage);
            exit(-1);
        }
    }

    if (strlen(hiddenMessage) == 0){
        fprintf(stderr, "Input error: The message is empty");
        fclose(in);
        free(hiddenMessage);
        exit(-1);
    }


    return hiddenMessage;// convert it to binary string
}
/**
 * hide message inside of the text
 * @param out: a file pointer, output file
 * @param in: a file pointer, input file
 * @param hiddenMessageBin: hidden message in binary form
 * @return the size of the image binary data
 */
int hideTheMessage(FILE *out, FILE *in, char *hiddenMessageBin) {
    //Assuming input validation has passed
    int aChar;
    for (int i = 0; i < strlen(hiddenMessageBin); i = i + 1) {
        aChar = fgetc(in);
        if (feof(in)) {
            return i;
        }
        aChar = hideABit(aChar, hiddenMessageBin[i]);
        fwrite(&aChar, 1, sizeof(unsigned char), out);
    }
    int i = (int)strlen(hiddenMessageBin)-1;
    while (1) {
        aChar = fgetc(in);
        if (feof(in)) {
            return i+1;
        }
        i = i + 1;
        fwrite(&aChar, 1, sizeof(unsigned char), out);
    }
}

int hideTheMessageM(FILE *out, FILE *in, char *hiddenMessageBin,int offset) {
    //Assuming input validation has passed
    int aChar;
    while (offset < strlen(hiddenMessageBin)){
        aChar = fgetc(in);
        if (feof(in)) {
            return offset;
        }
        aChar = hideABit(aChar, hiddenMessageBin[offset]);
        fwrite(&aChar, 1, sizeof(unsigned char), out);
        ++offset;
    }

    while (1) {//If the hiddenMessageBin has embed
        aChar = fgetc(in);
        if (feof(in)) {
            return offset;
        }
        fwrite(&aChar, 1, sizeof(unsigned char), out);
    }
}
/**
 * Turn input string into binary format
 * @param aString: input string
 * @return input string in binary format
 */
char *stringToBinary(char *aString) {
    if (aString == NULL) return 0; /* no input string */
    char *binary = malloc(
            (strlen(aString) + 1) * 8 + 1); // each char is one byte and we also need to include \0 and + 1 at the end for null terminator
    if (binary == NULL){
        return 0;
    }
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
    //strcat(binary,"11111111");
    return binary;
}
/**
 *
 * @param s
 * @return
 */
bool numbers_only(const char *s)
{
    while (*s) {
        if (isdigit(*s++) == 0) return false;
    }

    return true;
}

/**
 *
 * @param argc
 * @param argv
 * @return mode 0 normal 1 multiple file 2 parallel 3 before-after
 */
int inputValidation(int argc, char *argv[]){
    if (argc < 2){
        fprintf(stderr, "See -help for more info'\n");
        exit(-1);
    }
    if (strcmp(argv[1], "-m") == 0){
        if (argc == 5){
            if (numbers_only(argv[2]) == true && atoi(argv[2]) > 0 && atoi(argv[2]) <= 255){//number-of-files should be in the range of (0,255]
                return 1;
            }
        }
        fprintf(stderr, "-m Usage: hide -m number-of-files basename output-base-name'\n");
        exit(-1);
    }else if (strcmp(argv[1],"-p") == 0){
        //Do something
    }else if (strcmp(argv[1],"-s") == 0){
        //Do something
    }

    if (argc == 3){
        return 0;
    }else{
        fprintf(stderr, "See -help for more info'\n");
        exit(-1);
    }
}
/**
 *
 * @param file
 * @param index where the index up to
 */
void closePerviousFile(FILE *file[],int index){
    for(int i = 0; i < index; ++i){
        fclose(file[i]);
    }
}





int main(int argc, char *argv[]) {

    //====================
    //Input argument validation
    int mode = 0;
    mode = inputValidation(argc, argv);
    //=====================

    //==================
    if (mode == 0) {
        FILE *inputFile;
        inputFile = fopen(argv[1], "r");
        //File doesnt exist
        if (inputFile == NULL) {
            fprintf(stderr, "%s:error:Cannot open %s \n", argv[0], argv[1]);
            perror(0);
            exit(-1);
        }

        //====================
        //Checking the header and returning the hidden message in binary
        skipComment(inputFile);
        if (checkImageFormat(inputFile) == 1){
            exit(-1);
        }
        skipComment(inputFile);
        int *dimension = checkDimension(inputFile);
        if (dimension[3] == 1){
            exit(-1);
        }
        char *hiddenMessage = checkMessageSize(inputFile, dimension[0], dimension[1]);
        char *hiddenMessageBin = stringToBinary(hiddenMessage);
        free(hiddenMessage);
        skipComment(inputFile);
        if (checkColorChannel(inputFile)==1){
            exit(-1);
        }

        //======================
        //record offset, prepare to write out the header
        long fileBinaryPos = ftell(inputFile);
        //=======================

        rewind(inputFile);
        //read in output
        FILE *outputFile;
        outputFile = fopen(argv[2], "w");

        //Writing header info to the output file
        char headerInfo[fileBinaryPos];
        fread(headerInfo, (size_t) fileBinaryPos - 1, sizeof(char), inputFile);
        fwrite(headerInfo, (size_t) fileBinaryPos - 1, sizeof(char), outputFile);

        //write image binary to the output file and checks if the image's binary is matching up with the header
        if (dimension[0] * dimension[1] * 3 != hideTheMessage(outputFile, inputFile, hiddenMessageBin)) {
            fprintf(stderr, "Error: image's binary data is not matching up with the header\n");
            free(hiddenMessageBin);
            fclose(inputFile);
            fclose(outputFile);
            remove(argv[2]); //Remove the output file
            exit(-1);
        };
        free(hiddenMessageBin);

        //close the file
        fclose(inputFile);
        fclose(outputFile);

        //=====================
    }else if(mode == 1){

        int numberOfFiles = atoi(argv[2]);
        FILE *inputFile[atoi(argv[2])-1];
        long fileBinaryPos[atoi(argv[2])-1];

        char targetFileName[strlen(argv[3])+9];//Size should be length of the basename + 4 (-000) + 4 (.ppm) + 1 for the \0
        for (int i=0; i < numberOfFiles;++i){
            snprintf(targetFileName,strlen(argv[3])+9,"%s-%03d.%s",argv[3],i,"ppm");//Format string to basename-xxx.ppm
            inputFile[i] = fopen(targetFileName, "r");
            //File doesnt exist
            if (inputFile[i] == NULL) {
                fprintf(stderr, "%s:error:Cannot open %s \n", argv[0], targetFileName);
                closePerviousFile(inputFile,i);
                perror(0);
                exit(-1);
            }
        }//All of the file has been opened

        int maxMessage = 0;
        for (int i=0; i < numberOfFiles;++i){//Header validation

            skipComment(inputFile[i]);
            if (checkImageFormat(inputFile[i]) == 1){ //Header is not P6
                closePerviousFile(inputFile,i);//Close the previous one, not include the current one, because it has been closed by checkImageFormat
                exit(-1);
            }

            skipComment(inputFile[i]);

            int *dimension = checkDimension(inputFile[i]);
            if (dimension[2] == 1){// Index 2 is return status
                closePerviousFile(inputFile,i);//Close the previous one, not include the current one, because it has been closed by checkDimension
                exit(-1);
            }
            maxMessage += (int) (floor((dimension[0] * dimension[1] * 3) / 8) + (dimension[0] * dimension[1] * 3) % 8);
            skipComment(inputFile[i]);
            if (checkColorChannel(inputFile[i])==1){//Color channel is wrong
                closePerviousFile(inputFile,i);//Close the previous one, not include the current one, because it has been closed by checkColorChannel
                exit(-1);
            }
        }//Images header seems fine

        char *hiddenMessage = malloc(sizeof(char) * maxMessage);
        if (hiddenMessage == NULL) {
            fprintf(stderr, "Memory Error: Cannot allocate memory space\n");
            closePerviousFile(inputFile,numberOfFiles);
            exit(-1);
        }
        hiddenMessage[0] = '\0';
        printf("Please enter a hiddenMessage to be hidden inside of the image: \n");

        //Taking the input
        int aChar = 0;
        while (aChar != EOF){
            aChar = fgetc(stdin);
            if (append(hiddenMessage,(size_t)maxMessage,(char)aChar) != 0){
                fprintf(stderr, "Input error: The message is too large for this image\n");
                closePerviousFile(inputFile,numberOfFiles);
                free(hiddenMessage);
                exit(-1);
            }
        }

        if (strlen(hiddenMessage) == 0){
            fprintf(stderr, "Input error: The message is empty");
            closePerviousFile(inputFile,numberOfFiles);
            free(hiddenMessage);
            exit(-1);
        }

        char *hiddenMessageBin = stringToBinary(hiddenMessage);
        if (hiddenMessageBin == 0){
            fprintf(stderr, "Memory Error: Cannot allocate memory space\n");
            free(hiddenMessage);
            closePerviousFile(inputFile,numberOfFiles);
            exit(-1);
        }
        free(hiddenMessage);


        FILE *outputFile[atoi(argv[2])-1];
        char outputFileNameList[atoi(argv[2])-1][strlen(argv[4])+9];
        int startPoint = 0;

        for (int i=0; i < numberOfFiles;++i){

            //======================
            //record offset, prepare to write out the header
            fileBinaryPos[i] = ftell(inputFile[i]);
            //=======================
            rewind(inputFile[i]);
            //read in output
            outputFileNameList[i][0] = '\0';
            snprintf(outputFileNameList[i],strlen(argv[4])+9,"%s-%03d.%s",argv[4],i,"ppm");//Format string to basename-xxx.ppm
            outputFile[i] = fopen(outputFileNameList[i], "w");
            //File doesnt exist
            if (outputFile[i] == NULL) {
                fprintf(stderr, "%s:error:Cannot write %s \n", argv[0], outputFileNameList[i]);
                closePerviousFile(inputFile,numberOfFiles);
                closePerviousFile(outputFile,i);
                perror(0);
                exit(-1);
            }

            //Writing header info to the output file
            char *headerInfo = malloc(sizeof(char) * fileBinaryPos[i]);
            headerInfo[0] = '\0';
            fread(headerInfo, (size_t) fileBinaryPos[i] - 1, sizeof(char), inputFile[i]);
            fwrite(headerInfo, (size_t) fileBinaryPos[i] - 1, sizeof(char), outputFile[i]);
            free(headerInfo);

            //write image binary to the output file and checks if the image's binary is matching up with the header
            startPoint = hideTheMessageM(outputFile[i], inputFile[i], hiddenMessageBin,startPoint);
            if (startPoint == strlen(hiddenMessageBin)){
                break;
            }
            fclose(outputFile[i]);

        }
        if (startPoint != strlen(hiddenMessageBin)) {
                fprintf(stderr, "Error: image's binary data is not matching up with the header\n");
                free(hiddenMessageBin);
                closePerviousFile(inputFile,numberOfFiles);
                for (int i = 0; i < numberOfFiles;++i){
                    remove(outputFileNameList[i]);//Remove the output file
                }
                exit(-1);
            };
        free(hiddenMessageBin);
        closePerviousFile(inputFile,numberOfFiles);

    }

    return 0;
}



