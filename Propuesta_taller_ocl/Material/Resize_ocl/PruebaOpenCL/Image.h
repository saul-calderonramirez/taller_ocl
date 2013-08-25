#pragma once
#define R 0
#define G 1
#define B 2
#define N_LAYERS 3
#include "StdAfx.h"
/*
RGB Image structure
*/
typedef struct ImageRGB{
	int width;  //image width
	int height; // image height
	int lengthRGB;// number of pixels	
	unsigned char *ptrImRGB; //Pointer to the first pixel of the RGB input image    
}ImageRGB;

//Intensity Image
typedef struct ImageGS{
	int width;  // image width
	int height; // image height	
	int lengthGS;//total number of pixels
	unsigned char *ptrImGS; //Pointer to the first pixel of the intensity image	   
}ImageGS;
/*
Inits and allocates a Gray Scale image
@param width, Image width
@param height, Image height
@return ImageGS*, pointer to the new Gray Scale image
*/
ImageGS* initImageGS(int width, int height);
/*
Inits and allocates a RGB image
@param width, Image width
@param height, Image height
@return ImageGS*, pointer to the new RGB image
*/
ImageRGB* initImageRGB(int width, int height);
/*
Converts an IplImage struct Image (OpenCV), to the ImageRGB struct
@param ptrCVImage, pointer to the OpenCV image
@return ImageRGB*, pointer to the new RGB image
*/
ImageRGB* convertToImageRGB(IplImage* ptrCvImage);
/*
Converts an ImageRGB struct to IplImage struct Image (OpenCV)
@param ptrCVImage, pointer to the OpenCV image
@return ImageRGB*, pointer to the new RGB image
*/
IplImage* convertToCvImage(ImageRGB* ptrImageRGB);
/*
Converts an ImageRGB struct to IplImage struct Image (OpenCV)
@param ImageRGB*, pointer to the new RGB image
@return ptrCVImage, pointer to the OpenCV image
*/
IplImage* convertToCvImage(ImageGS* ptrImageGS);
/*
Converts an ImageRGB struct to a Gray Scale struct Image (OpenCV), using the YUV format
@param ImageRGB*, pointer to the new RGB image
@return ptrImageGS, pointer to the Gray scaled image
*/
ImageGS* convertToImageGS_YUV(ImageRGB* ptrImageRGB);
/*
Loads an ImageRGB struct from file
@param ptrPath, absolute or relative path
@return ptrImageRGB, pointer to the RGB image
*/
ImageRGB* loadImageRGB(char* ptrPath);
/*
Loads an ImageGS struct from file
@param ptrPath, absolute or relative path
@return ptrImageGS, pointer to the GS image
*/
ImageGS* loadImageYUV(char* ptrPath);
/*
Saves an ImageRGB struct to file
@param ptrPath, absolute or relative path
*/
void saveImageToFile(char* ptrPath, ImageRGB* ptrImage);
/*
Saves an ImageGS struct to file
@param ptrPath, absolute or relative path
*/
void saveImageToFile(char* ptrPath, ImageGS* ptrImageGS);
/*
Calculates the average pixel value of a given Gray Scale Image
@param ImageGS*, pointer to the gray scaled image
*/
double getAveragePixel(ImageGS* ptrImage);
/*
Upsizes the received image by a given factor
the algorithm uses bilineal interpolation
@param ImageGS*, pointer to the gray scaled image
@param factor, resize factor
@return ImageGS*, pointer to the resized image
*/
ImageGS* upsize(ImageGS* ptrOriginal, int factor);
/*
Set pixel value on a Gray Scale Image
Origin: Inferior left pixel
@param ptrImage, RGB image to modify
@param x, x position
@param y, y position
@param value, value to set in the given position
*/
void setPV(ImageGS* ptrImage, int x, int y, unsigned char value);
/*
gets pixel value on a Gray Scale Image
Origin: Inferior left pixel
@param ptrImage, Gray Scale image to modify
@param x, x position
@param y, y position
@return pixel value
*/
unsigned char getPV(ImageGS* ptrImage, int x, int y);
/*
sets pixel value on a RGB Scale Image
Origin: Inferior left pixel
@param ptrImage, RGB scale image 
@param x, x position
@param y, y position
@param pixel value
*/
void setPV(ImageRGB* ptrImage, int x, int y, int layer, unsigned char value);
/*
gets pixel value on a RGB Image
Origin: Inferior left pixel
@param ptrImage, RGB image 
@param x, x position
@param y, y position
@return pixel value
*/
unsigned char getPV(ImageRGB* ptrImage, int x, int y, int layer);
/*
sets pixel value on a IplImage Image
Origin: Inferior left pixel
@param ptrImage, IplImage image 
@param x, x position
@param y, y position
@param value, pixel value
*/
void setPV(IplImage* ptrImage, int x, int y, int layer, unsigned char value);
/*
gets pixel value on a IplImage Image
Origin: Inferior left pixel
@param ptrImage, IplImage image 
@param x, x position
@param y, y position
@return value, pixel value
*/
unsigned char getPV(IplImage* ptrImage, int x, int y, int layer);

