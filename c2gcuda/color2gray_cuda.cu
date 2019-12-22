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
#include <cuda.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

__global__
void colorToGrayscale(unsigned char *gray_img, unsigned char * color_img, int width, int height);

const int IS_PNG = 1;
const int IS_JPG = 2;
const int DESIRED_CHANNELS = 3;
const int MAX_NAME_LENGTH = 500;

int main(int argc, char *argv[]) {
	argv[1] = "//home//student//Desktop//cuda_image//lena1.png";
	argv[2] = "//home//student//Desktop//cuda_image//out.png";
	argv[3] = "1";

    int width, height, channels, type;
    char  in_name[MAX_NAME_LENGTH], out_name[MAX_NAME_LENGTH];
    strcpy(in_name, argv[1]);
    strcpy(out_name, argv[2]);
    type = atoi(argv[3]);
    printf("name: %s\n",in_name);
    unsigned char *color_img = stbi_load(in_name, &width, &height, &channels, 0); // load and conver the image to 3 channels (ignore the transparancy channel)
    if(color_img == NULL) {
        printf("Error in loading the image\n");
        exit(-1);
    }
    printf("Loaded image %s with a width of %dpx, a height of %dpx and %d channels\n", in_name, width, height, channels);

    // Convert the input image to gray
    int gray_channels = channels == 4 ? 2 : 1;
    size_t gray_img_size = width * height * gray_channels;
    size_t color_img_size = width * height * DESIRED_CHANNELS;
    
    unsigned char *gray_img = (unsigned char *)malloc(gray_img_size);
    if(gray_img == NULL) {
        printf("Unable to allocate memory for the gray image.\n");
        exit(1);
    }
    printf("Create a image array with a width of %dpx, a height of %dpx and %d channels\n", width, height, gray_channels);

    //setup device side memory
    unsigned char *d_color_img, *d_gray_img;
    cudaMalloc((void **) &d_color_img, color_img_size);
    cudaMalloc((void **) &d_gray_img, gray_img_size);
    cudaMemcpy(d_color_img, color_img, color_img_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_gray_img, gray_img, gray_img_size, cudaMemcpyHostToDevice);
    //kernel function starts
    int block_size = 16;
    int per_block = (height-1)/block_size +1;
    dim3 DimGrid(per_block,per_block,1);
    dim3 DimBlock(block_size,block_size,1);
    printf("per_block: %d\n",per_block);
    colorToGrayscale<<<DimGrid,DimBlock>>>(d_gray_img, d_color_img, width, height);
    //kernel function ends
    cudaMemcpy(gray_img, d_gray_img, gray_img_size, cudaMemcpyDeviceToHost);
    cudaFree(d_color_img);
    cudaFree(d_gray_img);


    if (type == IS_PNG)
    	stbi_write_png(out_name, width, height, gray_channels, gray_img, width * gray_channels);
    else
        if (type == IS_JPG)
            stbi_write_jpg(out_name, width, height, gray_channels, gray_img, 100); //The last parameter of the stbi_write_jpg function is a quality parameter that goes from 1 to 100. Since JPG is a lossy image format, you can chose how much data is dropped at save time. Lower quality means smaller image size on disk and lower visual image quality.
    printf("Wrote image %s with a width of %dpx, a height of %dpx and %d channels\n", out_name, width, height, channels);

    stbi_image_free(gray_img); 
}

__global__
void colorToGrayscale(unsigned char *gray_img, unsigned char * color_img, int width, int height){
    unsigned char pixel[DESIRED_CHANNELS];

   //If the input image has a transparency channel this will be simply copied to the second channel of the gray image, while the first channel of the gray image will contain the gray pixel values. If the input image has three channels, the output image will have only one channel with the gray data.
    int row = threadIdx.x + blockDim.x*blockIdx.x;
    int col = threadIdx.y + blockDim.y*blockIdx.y;
//    printf("%d,%d\n",row,col);
    if (row<width && col<height){
		int greyOffset = col * width + row;
		//printf("%d\n",greyOffset);
		int rgbOffset = greyOffset * DESIRED_CHANNELS;
		pixel[0] = color_img[rgbOffset];
		pixel[1] = color_img[rgbOffset + 1];
		pixel[2] = color_img[rgbOffset + 2];

		gray_img[greyOffset] = pixel[0] * 0.3 + pixel[1] * 0.58 + pixel[2] * 0.11;
    }
}
