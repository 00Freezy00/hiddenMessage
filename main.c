#include "hiddenMessage.h"




int main(int argc, char *argv[]) {

    //Not enough or too many arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: hide 'filename' 'hiddenMessage'");
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

    //Checking the header and returning the hidden message in binary
    skipComment(inputFile);
    checkImageFormat(inputFile, argv[2]);
    skipComment(inputFile);
    int *dimension = checkDimension(inputFile);
    char *hiddenMessageBin = checkMessageSize(inputFile, dimension[0], dimension[1]);
    skipComment(inputFile);
    checkColorChannel(inputFile);

    //record offset, prepare for switching to rb mode
    long fileBinaryPos = ftell(inputFile);
    fclose(inputFile);

    //read in the input
    inputFile = fopen(argv[1], "rb");
    if (inputFile == NULL) {
        fprintf(stderr, "%s:error:Cannot open %s ", argv[0], argv[1]);
        perror(0);
        exit(-1);
    }

    //read in output
    FILE *outputFile;
    outputFile = fopen(argv[2], "wb");

    //Writing header info to the output file
    char headerInfo[fileBinaryPos];
    fread(headerInfo, (size_t) fileBinaryPos - 1, sizeof(char), inputFile);
    fwrite(headerInfo, (size_t) fileBinaryPos - 1, sizeof(char), outputFile);

    //write image binary to the output file
    hideTheMessage(outputFile, inputFile, hiddenMessageBin);

    //close the file
    fclose(inputFile);
    fclose(outputFile);

    return 0;
}



