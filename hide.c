#include "hiddenMessage.h"


int hideABit(int charInt, char bit) {
    /*
     * hide a bit in a byte
     * charInt: a char
     * bit: 1 or 0
     */
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

char *checkMessageSize(FILE *in, int width, int height) {
    /*
     * Check the input message is within the limit of the image
     * in: a file pointer, the input PPM file
     * width height: dimension from the image header
     * return hidden message in a binary string
     */
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
        fprintf(stderr, "Size error: Hidden message is too large for this image");
        fclose(in);
        free(hiddenMessage);
        exit(-1);
    }

    return hiddenMessage;// convert it to binary string
}

int hideTheMessage(FILE *out, FILE *in, char *hiddenMessageBin) {
    /*
     * hide message inside of the text
     * out: file pointer, output file
     * in: file pointer, input file
     * hiddenMessageBin: hidden message in binary form
     * return the size of the image binary data
     */
    //Assuming input validation has passed
    int aChar;
    for (int i = 0; i < strlen(hiddenMessageBin); i = i + 1) {
        aChar = fgetc(in);
        aChar = hideABit(aChar, hiddenMessageBin[i]);
        fwrite(&aChar, 1, sizeof(unsigned char), out);
    }
    int i = (int)strlen(hiddenMessageBin)-1;
    while (1) {
        aChar = fgetc(in);
        if (feof(in)) {
            return i;
        }
        i = i + 1;
        fwrite(&aChar, 1, sizeof(unsigned char), out);
    }
}

char *stringToBinary(char *aString) {
    if (aString == NULL) return 0; /* no input string */
    char *binary = malloc(
            (strlen(aString) + 1) * 8 + 1); // each char is one byte and we also need to include \0 and + 1 at the end for null terminator
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
    return binary;
}

int main(int argc, char *argv[]) {

    //Not enough or too many arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: hide 'Input Filename' 'Output Filename'");
        exit(-1);
    }

    FILE *inputFile;
    inputFile = fopen(argv[1], "r");

    //File doesnt exist
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
    char *hiddenMessage = checkMessageSize(inputFile, dimension[0], dimension[1]);
    char *hiddenMessageBin = stringToBinary(hiddenMessage);
    free(hiddenMessage);
    skipComment(inputFile);
    checkColorChannel(inputFile);

    //record offset, prepare to write out the header
    long fileBinaryPos = ftell(inputFile);

    rewind(inputFile);
    //read in output
    FILE *outputFile;
    outputFile = fopen(argv[2], "wb");

    //Writing header info to the output file
    char headerInfo[fileBinaryPos];
    fread(headerInfo, (size_t) fileBinaryPos - 1, sizeof(char), inputFile);
    fwrite(headerInfo, (size_t) fileBinaryPos - 1, sizeof(char), outputFile);

    //write image binary to the output file and checks if the image's binary is matching up with the header
    if (dimension[0]*dimension[1]*3 != hideTheMessage(outputFile, inputFile, hiddenMessageBin)){
        fprintf(stderr,"Error: image's binary data is not matching up with the header");
        free(hiddenMessageBin);
        fclose(inputFile);
        fclose(outputFile);
        remove(argv[2]);
        exit(-1);

    };
    free(hiddenMessageBin);

    //close the file
    fclose(inputFile);
    fclose(outputFile);

    return 0;
}



