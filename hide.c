//
//Created by Jack 26956047
//Hide the message
//Updated to Assignment 2
#include "hiddenMessage.h"
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <SDL.h>


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
    char *hiddenMessage = malloc(sizeof(char) * maxMessage + 1);
    if (hiddenMessage == NULL) {
        fprintf(stderr, "Memory Error: Cannot allocate memory space\n");
        fclose(in);
        exit(-1);
    }
    hiddenMessage[0] = '\0';
    printf("Please enter a hiddenMessage to be hidden inside of the image: \n");

    int aChar = 0;
    while (aChar != EOF) {
        aChar = fgetc(stdin);
        if (append(hiddenMessage, (size_t) maxMessage, (char) aChar) != 0) {
            fprintf(stderr, "Input error: The message is too large for this image\n");
            fclose(in);
            free(hiddenMessage);
            exit(-1);
        }
    }

    if (strlen(hiddenMessage) == 0) {
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
 * @return Amount of bytes have been read
 */
int hideTheMessage(FILE *out, FILE *in, char *hiddenMessageBin, int offset) {
    //Assuming input validation has passed
    int aChar;
    while (offset < strlen(hiddenMessageBin)) {
        aChar = fgetc(in);
        if (feof(in)) {
            return offset; //When the input file is finished, but message hasn't finish yet
        }
        aChar = hideABit(aChar, hiddenMessageBin[offset]);//Hide one bit into a byte
        fwrite(&aChar, 1, sizeof(unsigned char), out);//Write one byte into the output file
        ++offset;
    }
    while (1) { //When the message has finished encoding, but there are still some spaces left
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
            (strlen(aString) + 1) * 8 +
            1); // each char is one byte and we also need to include \0 and + 1 at the end for null terminator
    if (binary == NULL) {
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
    return binary;
}
/**
 * Check to see if there are only numbers in the string
 * @param s
 * @return
 */
bool numbersOnly(const char *s) {
    while (*s) {
        if (isdigit(*s++) == 0) return false;
    }
    return true;
}

/**
 * Validate the input and give out which mode the software is going to run
 * @param argc
 * @param argv
 * @return mode 0 normal 1 multiple file 2 parallel 3 before-after image comparision
 */
int inputValidation(int argc, char *argv[]) {

    if (argc < 2) {//Basic checking, the message should not be less then 2 arguments
        fprintf(stderr, "See -help for more info'\n");
        exit(-1);
    }
    if (strcmp(argv[1], "-m") == 0) {
        if (argc == 5) {
            if (numbersOnly(argv[2]) == true && atoi(argv[2]) > 0 &&
                atoi(argv[2]) <= 255) {//number-of-files should be in the range of (0,255]
                return 1;
            }
        }
        fprintf(stderr, "-m Usage: hide -m number-of-files basename output-base-name\n");
        exit(-1);
    } else if (strcmp(argv[1], "-p") == 0) {
        if (argc == 3) {
            return 2;
        }
        fprintf(stderr, "-p Usage: hide -p file\n");
        exit(-1);
    } else if (strcmp(argv[1], "-s") == 0) {
        if (argc == 4) {
            return 3;
        }
        fprintf(stderr, "-s Usage: hide -s inputFile outputFile\n");
        exit(-1);
    } else if (strcmp(argv[1], "-help") == 0) {
        printf("No flag ./hide inputFile outputFile\n-m Usage: hide -m number-of-files basename output-base-name\n-p Usage: hide -p file\n-s Usage: hide -s inputFile outputFile\n");
        exit(0);
    } else if (argc == 3) {
        return 0;
    } else {
        fprintf(stderr, "See -help for more info\n");
        exit(-1);
    }
}
/**
 *
 * @param file
 * @param index where the index up to
 */
void closePreviousFile(FILE **file, int index) {
    for (int i = 0; i < index; ++i) {
        fclose(file[i]);
    }
}

/**
 * Visualise ppm file to the SDL surface
 * @param surface is the current surface we are working on
 * @param x pos on the surface initially should be 0
 * @param y pos on the surface initially should be 0 and always be 0, this variable here is for future extension
 * @param image a PPM file assuming it reaches reading binary data position
 * @param w end pos on the surface
 * @param h end pos on the surface
 * Assume the hide function work correctly, It doesn't need to check the file
 */
int ppmToSurface(SDL_Surface *surface,int x, int y,FILE* image,int w, int h){
    int rgb[3]; //Array to store the R G B
    for (int i = y; i < h; ++i) {//looping through ppm
        for (int j = x; j < w; ++j){
            rgb[0] = fgetc(image);
            rgb[1] = fgetc(image);
            rgb[2] = fgetc(image);
            int *p = (int *) ((char *) surface->pixels + i * surface->pitch +
                              j * surface->format->BytesPerPixel);//Find the pixel
            *p=SDL_MapRGB(surface->format, rgb[0], rgb[1], rgb[2]);//set it to the color
        }
    }
    return 0;
}

int normalMode(char *executePath, char *inputFileName, char *outputFileName) {
    FILE *inputFile;
    inputFile = fopen(inputFileName, "r");
    //File doesnt exist
    if (inputFile == NULL) {
        fprintf(stderr, "%s:error:Cannot open %s \n", executePath, inputFileName);
        perror(0);
        exit(-1);
    }

    //====================
    //Checking the header and returning the hidden message in binary
    skipComment(inputFile);
    if (checkImageFormat(inputFile) == 1) {
        exit(-1);
    }
    skipComment(inputFile);
    int *dimension = checkDimension(inputFile);
    if (dimension[3] == 1) {
        exit(-1);
    }

    char *hiddenMessage = checkMessageSize(inputFile, dimension[0], dimension[1]);
    char *hiddenMessageBin = stringToBinary(hiddenMessage);
    free(hiddenMessage);
    skipComment(inputFile);
    if (checkColorChannel(inputFile) == 1) {
        exit(-1);
    }

    //======================
    //record offset, prepare to write out the header
    long fileBinaryPos = ftell(inputFile);
    //=======================

    rewind(inputFile);
    //read in output
    FILE *outputFile;
    outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        fprintf(stderr, "%s:error:Cannot open %s \n", executePath, inputFileName);
        perror(0);
        fclose(inputFile);
        free(hiddenMessageBin);
        exit(-1);
    }

    //Writing header info to the output file
    char headerInfo[fileBinaryPos];
    fread(headerInfo, (size_t) fileBinaryPos, sizeof(char), inputFile);
    fwrite(headerInfo, (size_t) fileBinaryPos, sizeof(char), outputFile);

    //write image binary to the output file and checks if the image's binary is matching up with the header
    if (dimension[0] * dimension[1] * 3 != hideTheMessage(outputFile, inputFile, hiddenMessageBin, 0)) {
        fprintf(stderr, "Error: image's binary data is not matching up with the header\n");
        free(hiddenMessageBin);
        fclose(inputFile);
        fclose(outputFile);
        remove(outputFileName); //Remove the output file
        exit(-1);
    };
    free(hiddenMessageBin);

    //close the file
    fclose(inputFile);
    fclose(outputFile);

    //=====================
    return 0;
}

int MMode(char *argv[]) {
    int numberOfFiles = atoi(argv[2]);
    FILE *inputFile[atoi(argv[2]) - 1];
    long fileBinaryPos[atoi(argv[2]) - 1];

    char targetFileName[
            strlen(argv[3]) + 9];//Size should be length of the basename + 4 (-000) + 4 (.ppm) + 1 for the \0

    for (int i = 0; i < numberOfFiles; ++i) {
        snprintf(targetFileName, strlen(argv[3]) + 9, "%s-%03d.%s", argv[3], i,
                 "ppm");//Format string to basename-xxx.ppm
        inputFile[i] = fopen(targetFileName, "r");
        //File doesnt exist
        if (inputFile[i] == NULL) {
            fprintf(stderr, "%s:error:Cannot open %s \n", argv[0], targetFileName);
            closePreviousFile(inputFile, i);
            perror(0);
            exit(-1);
        }
    }//All of the file has been opened

    int maxMessage = 0;
    for (int i = 0; i < numberOfFiles; ++i) {//Header validation

        skipComment(inputFile[i]);
        if (checkImageFormat(inputFile[i]) == 1) { //Header is not P6
            closePreviousFile(inputFile,
                              i);//Close the previous one, not include the current one, because it has been closed by checkImageFormat
            exit(-1);
        }

        skipComment(inputFile[i]);

        int *dimension = checkDimension(inputFile[i]);
        if (dimension[2] == 1) {// Index 2 is return status
            closePreviousFile(inputFile,
                              i);//Close the previous one, not include the current one, because it has been closed by checkDimension
            exit(-1);
        }
        maxMessage += (int) (floor((dimension[0] * dimension[1] * 3) / 8) + (dimension[0] * dimension[1] * 3) % 8);
        skipComment(inputFile[i]);
        if (checkColorChannel(inputFile[i]) == 1) {//Color channel is wrong
            closePreviousFile(inputFile,
                              i);//Close the previous one, not include the current one, because it has been closed by checkColorChannel
            exit(-1);
        }
    }//Images header seems fine

    char *hiddenMessage = malloc(sizeof(char) * maxMessage + 1);//+1 for \0
    if (hiddenMessage == NULL) {
        fprintf(stderr, "Memory Error: Cannot allocate memory space\n");
        closePreviousFile(inputFile, numberOfFiles);
        exit(-1);
    }
    hiddenMessage[0] = '\0';
    printf("Please enter a hiddenMessage to be hidden inside of the image: \n");

    //Taking the input
    int aChar = 0;
    while (aChar != EOF) {
        aChar = fgetc(stdin);
        if (append(hiddenMessage, (size_t) maxMessage, (char) aChar) !=
            0) {//Return 1 means maxMessage cannot take in chars anymore
            fprintf(stderr, "Input error: The message is too large for this image\n");
            closePreviousFile(inputFile, numberOfFiles);
            free(hiddenMessage);
            exit(-1);
        }
    }

    if (strlen(hiddenMessage) == 0) {
        fprintf(stderr, "Input error: The message is empty");
        closePreviousFile(inputFile, numberOfFiles);
        free(hiddenMessage);
        exit(-1);
    }

    char *hiddenMessageBin = stringToBinary(hiddenMessage);
    if (hiddenMessageBin == 0) {
        fprintf(stderr, "Memory Error: Cannot allocate memory space\n");
        free(hiddenMessage);
        closePreviousFile(inputFile, numberOfFiles);
        exit(-1);
    }
    free(hiddenMessage);

    //Prepare writing to output file
    FILE *outputFile[atoi(argv[2]) - 1];
    char outputFileNameList[atoi(argv[2]) - 1][strlen(argv[4]) + 9];
    int offset = 0;

    for (int i = 0; i < numberOfFiles; ++i) {

        //======================
        //record offset, prepare to write out the header
        fileBinaryPos[i] = ftell(inputFile[i]);
        //=======================
        rewind(inputFile[i]);//reset the file header to the start
        //read in output
        outputFileNameList[i][0] = '\0';//Every String should have \0, I don't know if it is needed
        snprintf(outputFileNameList[i], strlen(argv[4]) + 9, "%s-%03d.%s", argv[4], i,
                 "ppm");//Format string to basename-xxx.ppm
        outputFile[i] = fopen(outputFileNameList[i], "w");
        //File doesnt exist
        if (outputFile[i] == NULL) {
            fprintf(stderr, "%s:error:Cannot write %s \n", argv[0], outputFileNameList[i]);
            closePreviousFile(inputFile, numberOfFiles);
            closePreviousFile(outputFile, i);// Close to i, because not all file have been opened
            free(hiddenMessageBin);
            perror(0);
            exit(-1);
        }

        //Writing header info to the output file
        char *headerInfo = malloc(sizeof(char) * fileBinaryPos[i]);//Each file has different file binary position
        headerInfo[0] = '\0';
        fread(headerInfo, (size_t) fileBinaryPos[i], sizeof(char), inputFile[i]);
        fwrite(headerInfo, (size_t) fileBinaryPos[i], sizeof(char), outputFile[i]);
        free(headerInfo);

        //write image binary to the output file and checks if the image's binary is matching up with the header
        offset = hideTheMessage(outputFile[i], inputFile[i], hiddenMessageBin, offset);
        fclose(outputFile[i]);
        if (offset ==
            strlen(hiddenMessageBin)) {//if the offset is not equal to hiddenMessageBin, clearly some image binary data is not matching up with the header
            break;
        }

    }
    if (offset != strlen(hiddenMessageBin)) {
        fprintf(stderr, "Error: images' binary data is not matching up with the header\n");
        free(hiddenMessageBin);
        closePreviousFile(inputFile, numberOfFiles);

        for (int i = 0; i < numberOfFiles; ++i) {
            remove(outputFileNameList[i]);//Remove the output file
        }
        exit(-1);
    };
    free(hiddenMessageBin);
    closePreviousFile(inputFile, numberOfFiles);
    return 0;
}

int PMode(char *argv[]) {
    pid_t pid;
    //Should have done it in fgetc, that way I can realloc if the string is too large for the current message, but No enough time to implement this
    char inputStringFile[128];
    char inputImageName[128];
    char outputImageName[128];
    inputStringFile[0] = '\0';
    inputImageName[0] = '\0';
    inputImageName[0] = '\0';

    FILE *inputFile;
    inputFile = fopen(argv[2], "r");
    //file doesnt exist
    if (inputFile == NULL) {
        fprintf(stderr, "%s:error:Cannot open %s\n", argv[0], argv[2]);
        perror(0);
        exit(-1);
    }

    while ((fscanf(inputFile, "%128s %128s %128s\n", inputStringFile, inputImageName, outputImageName)) ==
           3) {//If the fscanf doesnt return 3 assignments then it is either hitted the EOF or the file format is wrong
        FILE *inputString;
        inputString = fopen(inputStringFile, "r");
        if (inputString == NULL) {
            fprintf(stderr, "%s:error:Cannot open %s \n", argv[0], inputStringFile);
            perror(0);
            fclose(inputFile);
            exit(-1);
        }

        dup2(fileno(inputString), STDIN_FILENO);//Spit the file stream into stdin
        fclose(inputString);
        if ((pid = fork()) == 0) {
            fclose(inputFile);//Need to close the input file on each process, because when fork it dups the file pointer as well
            normalMode(argv[0], inputImageName, outputImageName);
            exit(0);
        } else if (pid == -1) {
            fprintf(stderr, "%s:error:Cannot fork %s \n", argv[0], argv[1]);
            fclose(inputFile);
            exit(-1);
        }
    }
    if (feof(inputFile)) {
        fclose(inputFile);
        while (wait(NULL) > 0);//Wait until all of the children finishes
        return 0;
    } else {
        fprintf(stderr, "Error: file format invalid");
        fclose(inputFile);
        exit(-1);
    }


}

int SMode(char *argv[]) {
    if (normalMode(argv[0], argv[2], argv[3]) == 0) { //Let the normal mode processes it first
        FILE* originalImage;
        //Input validation again, It's not that efficient
        originalImage = fopen(argv[2],"r");
        if (originalImage == NULL){
            fprintf(stderr, "%s:error:Cannot open %s \n", argv[0], argv[2]);
            perror(0);
            exit(-1);
        }

        skipComment(originalImage);
        if (checkImageFormat(originalImage) == 1) {
            fclose(originalImage);
            exit(-1);
        }
        skipComment(originalImage);
        int *originalDimension = checkDimension(originalImage);
        if (originalDimension[3] == 1) {
            fclose(originalImage);
            exit(-1);
        }

        //Dimension of the window, these 2 values cannot be changed
        int SCREEN_WIDTH = originalDimension[0]*2 + 5; //5 pixel to separate 2 image
        if (SCREEN_WIDTH < 50) { // If the window is too small, you might not be able to hit close
            SCREEN_WIDTH = 50;
        }
        int SCREEN_HEIGHT = originalDimension[1] + 30;



        /* The window to render to */
        SDL_Window *window = NULL;

        /* The surface contained by the window */
        SDL_Surface *screenSurface = NULL;

        /* Initialize SDL */
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            fprintf(stderr,"SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
            exit(-1);
        }

        /* Create the window */
        window = SDL_CreateWindow("PPM Image comparision",
                                  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  SCREEN_WIDTH, SCREEN_HEIGHT,
                                  SDL_WINDOW_SHOWN);


        if (window == NULL) {
            fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
            exit(-1);
        }
        //Getting window's surface
        screenSurface = SDL_GetWindowSurface(window);

        SDL_FillRect(screenSurface, NULL,
                 SDL_MapRGB(screenSurface->format, 0xff, 0xff, 0xff));
    //==========================================================
        skipComment(originalImage);
        if (checkColorChannel(originalImage) == 1) {//Color channel is wrong
            fclose(originalImage);
            exit(-1);
        }


        int x = 0;
        int y = 0;
        //Starts from 0,0 draw the picture
        if (ppmToSurface(screenSurface,x,y,originalImage,originalDimension[0],originalDimension[1]) == 1){
            fclose(originalImage);
            fprintf(stderr,"Binary data is not matching the header");
            exit(-1);
        }
        fclose(originalImage);

        SDL_Rect drawSquare;

        drawSquare.x = originalDimension[0];//Start pic from the end of first picture
        drawSquare.y = 0;
        drawSquare.w = 5;
        drawSquare.h = originalDimension[1];

        SDL_FillRect(screenSurface, &drawSquare,
                     SDL_MapRGB(screenSurface->format, 0, 0, 0));//Draw a black rectangle to separate the image

        //Open the processed Image
        FILE* processedImage;
        processedImage = fopen(argv[3],"r");
        if (processedImage == NULL){
            fprintf(stderr, "%s:error:Cannot open %s \n", argv[0], argv[3]);
            perror(0);
            exit(-1);
        }

        //Header validation
        skipComment(processedImage);
        if (checkImageFormat(processedImage) == 1) {
            fclose(processedImage);
            exit(-1);
        }
        skipComment(processedImage);
        int *processedDimension = checkDimension(processedImage);
        if (processedDimension[3]  == 1) {
            fclose(processedImage);
            exit(-1);
        }

        skipComment(processedImage);
        if (checkColorChannel(processedImage) == 1) {//Color channel is wrong
            fclose(processedImage);
            exit(-1);
        }

        //get the offset of x
        x = x + drawSquare.x + 5;
        y = 0;
        if (ppmToSurface(screenSurface, x, y, processedImage, processedDimension[0] * 2 + 5, processedDimension[1]) ==
            1) {//Map the second image
            fclose(processedImage);
            fprintf(stderr,"Binary data is not matching the header");
            exit(-1);
        }
        fclose(processedImage);

        //Show the window
        SDL_UpdateWindowSurface(window);


        SDL_Event event;
        bool windowRunning = true;
        while (windowRunning)
        {
            if (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)//Quit if the user click on close
                {
                    windowRunning = false;
                }
            }
        }
        SDL_DestroyWindow(window);//Destroy window
        SDL_Quit();//Close it
        //Not sure if you need to destroy the surface
        return 0;
    };
    return 0;
}


int main(int argc, char *argv[]) {

    //====================
    //Input argument validation
    int mode = 0;
    mode = inputValidation(argc, argv);
    //=====================

    //==================
    if (mode == 0) {
        return normalMode(argv[0], argv[1], argv[2]);
    } else if (mode == 1) {
        return MMode(argv);
    } else if (mode == 2) {
        return PMode(argv);
    } else if (mode == 3){
        return SMode(argv);
    }
}



