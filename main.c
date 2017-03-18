#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


FILE* skipComment(FILE* imageFile){
    int c;
    char commentHeader;
    char ignore[1024];
    while(!feof(imageFile)) {
        c = fgetc(imageFile);
        commentHeader = (char) c;
        if (commentHeader == '#'){
            fgets(ignore,sizeof(ignore),imageFile);//Skip a line
        }else{
            fseek(imageFile,-1,SEEK_CUR);//go back 1 byte
            return imageFile;
        }
    }
    return imageFile;
}

char hideABit(char inputChar, int bit){
    int charInt = (int)inputChar;
    return (char)((charInt & ~1) | bit);
}
/*
char* stringToBinary(char* aString){
    if (aString == NULL){
        return 0;
    }
    char *bin = malloc(strlen(aString)*8+1);

}*/

int main(int argc, char * argv[]) {

    //Not enough or too many arguments
    if (argc != 3){
        fprintf(stderr,"Usage: hide 'filename' 'message'");
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


    //Incorrect image format

    inputFile = skipComment(inputFile);
    char imageFormat[4];
    fscanf(inputFile,"%3s ",imageFormat); //Space consume \n
    if ((strcmp(imageFormat,"P6") != 0)){
        fprintf(stderr,"Format Error: %s has to be in P6 ppm format", argv[1]);
        fclose(inputFile);
        exit(-1);
    }

    inputFile = skipComment(inputFile);
    char strWidth[10];
    char strHeight[10];
    fscanf(inputFile,"%10s %10s ",strWidth,strHeight);

    //Check height and width is a positive integer
    int width = atoi(strWidth);
    int height = atoi(strHeight);

    if (width <= 0 || height <= 0){
        fprintf(stderr,"Format error: height and width in the image header has to be an positive integer");
        fclose(inputFile);
        exit(-1);
    }


    //getting hidden message
    int maxMessage = ((int)(floor((width*height*3)/8)) + (width*height*3)%8 - 1);
    char message[maxMessage];//decimal
    do{
        printf("Please enter a message to be hidden inside of the image: ");
        fgets(message,sizeof(message),stdin);
        if (strcmp(message,"\n")==0){
            message[strlen(message)-1] = '\0';
        }
    }while (strcmp(message,"")==0);



    //Check if the message can fit in the image
    if (strlen(message)>maxMessage){
        fprintf(stderr,"Size error: hidden message is too large for this image need at least %u more bytes", abs(((int)strlen(message))+1-(width*height)));
        fclose(inputFile);
        exit(-1);
    }


    //Checking maximum value for a color channel
    inputFile = skipComment(inputFile);
    char colorSpecStr[4];
    fscanf(inputFile,"%4s ",colorSpecStr);
    int colorSpec;

    if ((colorSpec = atoi(colorSpecStr)) == 0){
        fprintf(stderr,"Format Error: %s is not a integer",colorSpecStr);
        fclose(inputFile);
        exit(-1);

    }

    if (colorSpec != 255){
        fprintf(stderr,"Format Error: maximum value for a color channel needs to be 255");
        fclose(inputFile);
        exit(-1);
    }

    //record offset, prepare for switching to rb mode
    long fileBinaryPos = ftell(inputFile);
    fclose(inputFile);

    inputFile = fopen(argv[1],"rb");


    if (inputFile == NULL){
        fprintf(stderr,"%s:error:Cannot open %s ",argv[0],argv[1]);
        perror(0);
        exit(-1);
    }
    fseek(inputFile,fileBinaryPos,SEEK_SET);

    //getting bytes data
    char * binString = (char*) malloc(sizeof(char) * ((width*height*3)));
    if (binString == NULL){
        fprintf(stderr,"Memory Error: Not enough memory to allocate");
        fclose(inputFile);
        exit(-1);
    }
    fread(binString,(size_t)((width*height*3)),1,inputFile);
    fclose(inputFile);

    for (int i = 0; i < strlen(binString),++i){
        binString[i] &= ~(1<<8);
    }




/*
    //Always grab one more char to check if the raw data exceeds the specification
    //import all binary data as char array
    inputFile = skipComment(inputFile);
    char * binString = (char*) malloc(sizeof(char) * ((width*height*3)+1));

    if (binString == NULL){
        fprintf(stderr,"Memory Error: Not enough memory to allocate");
        fclose(inputFile);
        exit(-1);
    }

    fgets(binString,(width*height*3)+1,inputFile);
    int c;
    c = fgetc(inputFile);
    if (feof(inputFile)){
        printf("end");
    }
*/
    printf("Code here");

    fclose(inputFile);
    //free(binString);

    return 0;
}



