/*
Convert an image to grayscale.

The code is written for a mini-project of ITCS 5145 Parallel Programming at UNCC.(Q.C.)

To compile the code, we use
        gcc -g -Wall -o color2grapy stb_image/stb_image.h stb_image/stb_image_write.h color2gray.c -lm

To run the code, type
        ./color2gray ${input color image} ${output grayscale image} ${image type}

        The format of images depends on its types.
        To specify image type, we have ${image type} as follows:
            1 is for .png file
            2 is for .jpg file
        
        For example,
        ./color2grapy lena1.png lena2.png 1
        ./color2grapy lizard1.jpg lizard2.jpg 2
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

void colorToGrayscale(unsigned char *gray_image, unsigned char * color_image, int width, int height);

const int IS_PNG = 1;
const int IS_JPG = 2;
const int DESIRED_CHANNELS = 3;
const int MAX_NAME_LENGTH = 500;

int main(int argc, char *argv[]) {
	int my_rank, process_count;
	if (MPI_Init(&argc,&argv)){
		printf("Error in initialization!");
	}

	if (MPI_Comm_size(MPI_COMM_WORLD, &process_count)){
		printf("Error to get proc count!");
	}
	if (MPI_Comm_rank(MPI_COMM_WORLD, &my_rank)){
		printf("Error to get rank!");
	}

	unsigned char *gray_image = NULL;
	unsigned char *color_image = NULL;
	unsigned char *local_gray_image = NULL;
	unsigned char *local_color_image = NULL;
	if (my_rank == 0){
	//fetch args
    if (argc < 4){
        printf("Usage: color2Grayscale ${input color image file} ${output grayscale image file} ${image type}\n Image Types:\n\t1: PGN\n\t2: JPG");
        exit(-1);
    }

    int width, height, channels, type;
    char in_name[MAX_NAME_LENGTH], out_name[MAX_NAME_LENGTH];
    strcpy(in_name, argv[1]);
    strcpy(out_name, argv[2]);
    type = atoi(argv[3]);

    color_image = stbi_load(in_name, &width, &height, &channels, 0); // load and conver the image to 3 channels (ignore the transparancy channel)
    if(color_image == NULL) {
        printf("Error in loading the image\n");
        exit(-1);
    }
    printf("Loaded image %s with a width of %dpx, a height of %dpx and %d channels\n", in_name, width, height, channels);

    // Convert the input image to gray
    int gray_channels = channels == 4 ? 2 : 1;
    // size_t
    int color_image_size = width * height * DESIRED_CHANNELS;
    int gray_image_size = width * height * gray_channels;
    
    gray_image = (unsigned char *)malloc(sizeof(unsigned char)*gray_image_size);
    if(gray_image == NULL) {
        printf("Unable to allocate memory for the gray image.\n");
        exit(1);
    }
    printf("Create a image array with a width of %dpx, a height of %dpx and %d channels\n", width, height, gray_channels);
    printf("%d,%d\n",gray_image_size/(process_count-1),gray_image_size%(process_count-1));
    // start mpi
//    MPI_Scatter(color_image_size, 1, MPI_INT, local_color_image_size, 1, MPI_INT);
//    MPI_Scatter(gray_image_size, 1, MPI_INT, local_gray_image_size, 1, MPI_INT);
    MPI_Scatter(color_image, color_image_size, MPI_UNSIGNED_CHAR, local_color_image, color_image_size, MPI_UNSIGNED_CHAR, 0 , MPI_COMM_WORLD);

//    MPI_Scatter(color_image, &color_image_size, MPI_CHAR, local_color_image, &color_image_size, MPI_CHAR, 0 , MPI_COMM_WORLD);
   //colorToGrayscale(gray_image, color_image, width, height);
    // end mpi
//    if (type == IS_PNG)
//    	stbi_write_png(out_name, width, height, gray_channels, gray_image, width * gray_channels);
//    else
//        if (type == IS_JPG)
//            stbi_write_jpg(out_name, width, height, gray_channels, gray_image, 100); //The last parameter of the stbi_write_jpg function is a quality parameter that goes from 1 to 100. Since JPG is a lossy image format, you can chose how much data is dropped at save time. Lower quality means smaller image size on disk and lower visual image quality.
//    printf("Wrote image %s with a width of %dpx, a height of %dpx and %d channels\n", out_name, width, height, channels);
//
//    stbi_image_free(gray_image);
    // end program
	}

	else
	{
		int color_image_size = 512*512*3, gray_image_size = 512*512;
		local_gray_image = (unsigned char *)malloc(sizeof(unsigned char)*gray_image_size);
		local_color_image = (unsigned char *)malloc(sizeof(unsigned char)*color_image_size);
		MPI_Scatter(color_image, color_image_size, MPI_UNSIGNED_CHAR, local_color_image, color_image_size, MPI_UNSIGNED_CHAR, 0 , MPI_COMM_WORLD);

////	    MPI_Scatter(color_image_size, 1, MPI_INT, &local_color_image_size, 1, MPI_INT);
////	    MPI_Scatter(gray_image_size, 1, MPI_INT, &local_gray_image_size, 1, MPI_INT);
////	    MPI_Scatter(color_image, color_image_size, MPI_CHAR, local_color_image, color_image_size, MPI_CHAR, 0 , MPI_COMM_WORLD);
//	    MPI_Scatter(gray_image, gray_image_size/(process_count-1), MPI_CHAR, local_gray_image, gray_image_size/(process_count-1), MPI_CHAR, 0 , MPI_COMM_WORLD);
////	    printf("%d,%d\n",local_color_image_size, local_gray_image_size);
	}
		MPI_Finalize();
	return 0;
}

void colorToGrayscale(unsigned char *gray_image, unsigned char * color_image, int width, int height){
    unsigned char pixel[DESIRED_CHANNELS];

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
           //If the input image has a transparency channel this will be simply copied to the second channel of the gray image, while the first channel of the gray image will contain the gray pixel values. If the input image has three channels, the output image will have only one channel with the gray data.

            int greyOffset = row * width + col;
            int rgbOffset = greyOffset * DESIRED_CHANNELS;
            pixel[0] = color_image[rgbOffset];
            pixel[1] = color_image[rgbOffset + 1];
            pixel[2] = color_image[rgbOffset + 2];

            gray_image[greyOffset] = pixel[0] * 0.3 + pixel[1] * 0.58 + pixel[2] * 0.11;
        }
    }
}
