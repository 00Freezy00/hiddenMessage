#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


void skipComment(FILE* imageFile){
    int commentHeader;
    char ignore[1024];
    while(!feof(imageFile)) {
        commentHeader = fgetc(imageFile);
        if (commentHeader == '#'){
            fgets(ignore,sizeof(ignore),imageFile);//Skip a line
        }else{
            fseek(imageFile,-1,SEEK_CUR);//go back 1 byte
            return;
        }
    }
}


char* stringToBinary(char* aString) {
    if(aString == NULL) return 0; /* no input string */
    char *binary = malloc((strlen(aString)+1)*8 + 1); // each char is one byte (8 bits) and + 1 at the end for null terminator
    binary[0] = '\0';
    for(size_t i = 0; i < strlen(aString); ++i) {
        char aChar = aString[i];
        for(int j = 7; j >= 0; j = j - 1){
            if(aChar & (1 << j)) {
                strcat(binary,"1");
            } else {
                strcat(binary,"0");
            }
        }
    }
    strcat(binary,"00000000");
    return binary;
}

int hideABit (int charInt, char bit){
    switch(bit){
        case '0':
            charInt &= ~(1 << 0);
            return  charInt;
        case '1':
            charInt |= 1 << 0;
            return  charInt;
        default:
            fprintf(stderr,"Is this a bit?");
            exit(-1);
    }
}

void checkImageFormat(FILE * in,char * filename){
    //Incorrect image format
    char imageFormat[4];
    fscanf(in,"%3s ",imageFormat); //Space consume \n
    if ((strcmp(imageFormat,"P6") != 0)){
        fprintf(stderr,"Format Error: %s has to be in P6 ppm format", filename);
        fclose(in);
        exit(-1);
    }
}

int* checkDimension(FILE * in){
    char strWidth[10], strHeight[10];
    fscanf(in,"%10s %10s ",strWidth,strHeight);

    //Check height and width is a positive integer
    int width = atoi(strWidth), height = atoi(strHeight);

    if (width <= 0 || height <= 0){
        fprintf(stderr,"Format error: height and width in the image header has to be an positive integer");
        fclose(in);
        exit(-1);
    }
    static int dimension[2];
    dimension[0] = width;
    dimension[1] = height;
    return dimension;
}

char* checkMessageSize(FILE * in, int width, int height){
    //getting hidden hiddenMessage
    int maxMessage = ((int)(floor((width*height*3)/8)) + (width*height*3)%8);
    char hiddenMessage[maxMessage];//decimal
    do{
        printf("Please enter a hiddenMessage to be hidden inside of the image: ");
        fgets(hiddenMessage,sizeof(hiddenMessage),stdin);
        hiddenMessage[strcspn(hiddenMessage,"\n")] = '\0';
    }while (strcmp(hiddenMessage,"")==0);

    //Check if the hiddenMessage can fit in the image
    if (strlen(hiddenMessage)>maxMessage){
        fprintf(stderr,"Size error: hidden hiddenMessage is too large for this image need at least %u more bytes", abs(((int)strlen(hiddenMessage))+1-(width*height)));
        fclose(in);
        exit(-1);
    }

    return stringToBinary(hiddenMessage);// convert it to binary string
}

void checkColorChannel(FILE * in){
    //Checking maximum value for a color channel
    char colorSpecStr[4];
    fscanf(in,"%4s ",colorSpecStr);
    int colorSpec;

    if ((colorSpec = atoi(colorSpecStr)) == 0){
        fprintf(stderr,"Format Error: %s is not a integer",colorSpecStr);
        fclose(in);
        exit(-1);

    }
    if (colorSpec != 255){
        fprintf(stderr,"Format Error: maximum value for a color channel needs to be 255");
        fclose(in);
        exit(-1);
    }
}

void hideTheMessage(FILE * out,FILE * in, char* hiddenMessageBin){
    //Assuming input validation has passed
    int aChar;

    for (int i = 0; i < strlen(hiddenMessageBin); i = i + 1){
        aChar = fgetc(in);
        aChar = hideABit(aChar,hiddenMessageBin[i]);
        fwrite(&aChar, 1, sizeof(unsigned char),out);
    }

    while (1){
        aChar = fgetc(in);
        if (feof(in)){
            return;
        }
        fwrite(&aChar, 1, sizeof(unsigned char),out);
    }
}

int main(int argc, char * argv[]) {

    //Not enough or too many arguments
    if (argc != 3){
        fprintf(stderr,"Usage: hide 'filename' 'hiddenMessage'");
        exit(-1);
    }

    FILE * inputFile;
    inputFile = fopen(argv[1],"r+");

    //file doesnt exist
    if (inputFile == NULL){
        fprintf(stderr,"%s:error:Cannot open %s ",argv[0],argv[1]);
        perror(0);
        exit(-1);
    }

    //Checking the header and returning the hidden message in binary
    skipComment(inputFile);
    checkImageFormat(inputFile,argv[2]);
    skipComment(inputFile);
    int * dimension = checkDimension(inputFile);
    char * hiddenMessageBin = checkMessageSize(inputFile,dimension[0],dimension[1]);
    skipComment(inputFile);
    checkColorChannel(inputFile);

    //record offset, prepare for switching to rb mode
    long fileBinaryPos = ftell(inputFile);
    fclose(inputFile);

    //read in the input
    inputFile = fopen(argv[1],"rb");
    if (inputFile == NULL){
        fprintf(stderr,"%s:error:Cannot open %s ",argv[0],argv[1]);
        perror(0);
        exit(-1);
    }

    //read in output
    FILE* outputFile;
    outputFile = fopen(argv[2],"wb");

    //Writing header info to the output file
    char headerInfo[fileBinaryPos];
    fread(headerInfo,(size_t)fileBinaryPos-1,sizeof(char),inputFile);
    fwrite(headerInfo, (size_t)fileBinaryPos-1, sizeof(char),outputFile);

    //write image binary to the output file
    hideTheMessage(outputFile,inputFile,hiddenMessageBin);

    //close the file
    fclose(inputFile);
    fclose(outputFile);

    return 0;
}



