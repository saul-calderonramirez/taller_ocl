#include "StdAfx.h"

/*
Loads an ImageRGB struct from file
@param ptrPath, absolute or relative path
@return ptrImageRGB, pointer to the RGB image
*/
ImageRGB* loadImageRGB(char* ptrPath){
	IplImage* ptrCvImage = cvLoadImage(ptrPath);	
	ImageRGB* ptrImageRGB = convertToImageRGB(ptrCvImage);	
	return ptrImageRGB;
} 

/*
Calculates the average pixel value of a given Gray Scale Image
@param ImageGS*, pointer to the gray scaled image
*/
double getAveragePixel(ImageGS* ptrImage){
	int i, j;
	double result = 0;
	for(i = 0; i < ptrImage->width; ++i){
		for(j = 0; j < ptrImage->height; ++j){
			result += (double)getPV(ptrImage, i, j);
		}
	}
	result = result / (double)ptrImage->lengthGS;
	return result;
}

/*
Converts an IplImage struct Image (OpenCV), to the ImageRGB struct
@param ptrCVImage, pointer to the OpenCV image
@return ImageRGB*, pointer to the new RGB image
*/
ImageRGB* convertToImageRGB(IplImage* ptrCvImage){
	int i, j, k;
	ImageRGB* ptrImageRGB = initImageRGB(ptrCvImage->width, ptrCvImage->height);
	cvCvtColor(ptrCvImage , ptrCvImage, CV_BGR2RGB); 	
	for (i = 0;i < ptrImageRGB->height; i++) {				
		for(j = 0; j < ptrImageRGB->width; j++) {
			for( k = 0; k < N_LAYERS; ++k){								
				setPV(ptrImageRGB, j, i, k, getPV(ptrCvImage, j, i, k));
			}
		}
	}
	return ptrImageRGB;
}
/*
Converts an ImageRGB struct to IplImage struct Image (OpenCV)
@param ptrCVImage, pointer to the OpenCV image
@return ImageRGB*, pointer to the new RGB image
*/
IplImage* convertToCvImage(ImageRGB* ptrImageRGB){
	int i, j, k;
	CvSize imageSize = cvSize(ptrImageRGB->width, ptrImageRGB->height);
	IplImage *ptrCvImage = cvCreateImage(imageSize , IPL_DEPTH_8U, 3); 
	for (i = 0;i < ptrImageRGB->height; i++) {		
		for(j = 0; j < ptrImageRGB->width; j++) {
			for( k = 0; k < N_LAYERS; ++k){								
				setPV(ptrCvImage, j, i, k, getPV(ptrImageRGB, j, i, k));
			}
		}
	}
	cvCvtColor(ptrCvImage , ptrCvImage, CV_RGB2BGR);
	return ptrCvImage;
}
/*
Converts an ImageRGB struct to IplImage struct Image (OpenCV)
@param ImageRGB*, pointer to the new RGB image
@return ptrCVImage, pointer to the OpenCV image
*/
IplImage* convertToCvImage(ImageGS* ptrImageGS){
	int i, j;
	CvSize imageSize = cvSize(ptrImageGS->width, ptrImageGS->height);
	IplImage *ptrCvImage = cvCreateImage(imageSize , IPL_DEPTH_8U, 1); 
	for (i = 0;i < ptrImageGS->height; i++) {		
		for(j = 0; j < ptrImageGS->width; j++) {									
			setPV(ptrCvImage, j, i, 0, getPV(ptrImageGS, j, i));
		}
	}	
	return ptrCvImage;
}
/*
Converts an ImageRGB struct to a Gray Scale struct Image (OpenCV), using the YUV format
@param ImageRGB*, pointer to the new RGB image
@return ptrImageGS, pointer to the Gray scaled image
*/
ImageGS* convertToImageGS_YUV(ImageRGB* ptrImageRGB){
	int i, j;
	ImageGS* ptrImageGS = initImageGS(ptrImageRGB->width, ptrImageRGB->height);
	unsigned char Y;
	for(i = 0; i < ptrImageGS->height; ++i){
		for(j = 0; j < ptrImageGS->width; ++j){
			//Intensity value
			Y = (unsigned char)((double)getPV(ptrImageRGB, j, i, R) * 0.299 + (double)getPV(ptrImageRGB, j, i, G) * 0.587 + (double)getPV(ptrImageRGB, j, i, B) * 0.114);
			setPV(ptrImageGS, j, i, Y);
		}
	}	
	return ptrImageGS;
}
/*
Saves an ImageRGB struct to file
@param ptrPath, absolute or relative path
*/
void saveImageToFile(char* ptrPath, ImageRGB* ptrImageRGB){
	IplImage* ptrCvImage = convertToCvImage(ptrImageRGB);
	cvSaveImage(ptrPath, ptrCvImage);
}
/*
Saves an ImageGS struct to file
@param ptrPath, absolute or relative path
*/
void saveImageToFile(char* ptrPath, ImageGS* ptrImageGS){
	IplImage* ptrCvImage = convertToCvImage(ptrImageGS);
	cvSaveImage(ptrPath, ptrCvImage);
}
/*
Upsizes the received image by a given factor
the algorithm uses bilineal interpolation
@param ImageGS*, pointer to the gray scaled image
@param factor, resize factor
@return ImageGS*, pointer to the resized image
*/
ImageGS* upsize(ImageGS* ptrOriginal, int factor){
	assert(factor > 0);
	ImageGS* ptrResized = initImageGS(ptrOriginal->width * factor, ptrOriginal->height * factor);
	//width columns, height rows
	//Resized image counters
	int Rx, Ry;	
	//current local Origin on Resized Image
	int LORx = 0;
	int LORy = 0;
	//current local Origin on the Original image
	int LOOx = 0;
	int LOOy = 0;
	unsigned char Y1, Y2, Y3, Y4;
	double a, b, c, d;
	//Origin inferior left of the image
	for(Ry = 0; Ry < ptrResized->height; ++Ry){
		for(Rx = 0; Rx < ptrResized->width; ++Rx){						
			if(Rx == 0 || Rx % factor == 0){
				LORx = Rx;
			}
			//In this pixel, the original value must be copied		
			if( (Ry == 0 || Ry % factor == 0)&& (Rx == 0 || Rx % factor == 0)){									
				setPV(ptrResized, Rx, Ry, getPV(ptrOriginal, Rx / factor, Ry / factor));									
			}
			//in this case, an interpolation of the pixel is necessary 
			else{
				//Formula variables initialiazed
				Y1 = 0;
				Y2 = 0;
				Y3 = 0;
				Y4 = 0;
				LOOx = LORx / factor;
				LOOy = LORy / factor;				
				//Formula taken from the course notes, Bilineal interpolation
				Y1 = getPV(ptrOriginal, LOOx, LOOy);
				//The value in the original image might be out range, anyway is null in the formula
				if(LOOx + 1 < ptrOriginal-> width)
					Y2 = getPV(ptrOriginal, LOOx + 1, LOOy);
				if(LOOx + 1 < ptrOriginal-> width && LOOy + 1 < ptrOriginal->height)
					Y3 = getPV(ptrOriginal, LOOx + 1, LOOy + 1);
				if(LOOy + 1 < ptrOriginal-> height)
					Y4 = getPV(ptrOriginal, LOOx, LOOy + 1);
				a = (double)(Y2 - Y1);
				b = (double)(Y4 - Y1);
				c = (double)(Y1 + Y3 - Y2 - Y4);
				d = (double)Y1;
				//normalized delta
				double deltaX = ((double)(Rx - LORx))/(double)factor;
				double deltaY = ((double)(Ry - LORy))/(double)factor;
				double dYe = (a * deltaX + b * deltaY + c * deltaY * deltaX + d);				
				/*if(dYe <= 0 || dYe > 255){
					printf("VALOR INCORRECTO\n");
				}*/
				setPV(ptrResized, Rx, Ry, (unsigned char)dYe);
			}
		}
		if(Ry == 0 || Ry % factor == 0 ){			
			//The Local Origin of the resized image is updated
			LORy = Ry;
			//The original image column counter is reseted
			LORx = 0;			
		}		
	}
	return ptrResized;

}
/*
Inits and allocates a Gray Scale image
@param width, Image width
@param height, Image height
@return ImageGS*, pointer to the new Gray Scale image
*/
ImageGS* initImageGS(int width, int height ){
	ImageGS* ptrImageGS = (ImageGS*)malloc(sizeof(ImageGS));
	ptrImageGS->width = width;
	ptrImageGS->height = height;
	ptrImageGS->lengthGS = ((ptrImageGS->width) * (ptrImageGS->height));
	ptrImageGS->ptrImGS = (unsigned char*)malloc(sizeof(unsigned char)*(unsigned int)ptrImageGS->lengthGS);
	int i;
	for(i = 0; i < ptrImageGS->lengthGS; ++i){
		ptrImageGS->ptrImGS[i] = 0;
	}
	return ptrImageGS;
}
/*
Inits and allocates a RGB image
@param width, Image width
@param height, Image height
@return ImageGS*, pointer to the new RGB image
*/
ImageRGB* initImageRGB(int width, int height){
	ImageRGB* ptrImageRGB = (ImageRGB*)malloc(sizeof(ImageRGB));
	ptrImageRGB->width = width;
	ptrImageRGB->height = height;
	ptrImageRGB->lengthRGB = ((ptrImageRGB->width) * (ptrImageRGB->height) * N_LAYERS);
	ptrImageRGB->ptrImRGB = (unsigned char*)malloc(sizeof(unsigned char)*(unsigned int)ptrImageRGB->lengthRGB);
	int i;
	for(i = 0; i < ptrImageRGB->lengthRGB; ++i){
		ptrImageRGB->ptrImRGB[i] = 0;
	}
	return ptrImageRGB;
}

/*
Set pixel value on RGB image
Origin: Inferior left pixel
@param ptrImage, RGB image to modify
@param x, x position
@param y, y position
@param value, value to set in the given position
*/
void setPV(ImageRGB* ptrImage, int x, int y, int layer, unsigned char value){
	assert(x < ptrImage->width && y < ptrImage->height);
	//translation to default axis settings			
	int cy = (ptrImage->height - 1) - y;		
	ptrImage->ptrImRGB[ cy * ptrImage->width * N_LAYERS + x * N_LAYERS + layer] = value;	
}

/*
gets pixel value on RGB image
Origin: Inferior left pixel
@param ptrImage, RGB image to modify
@param x, x position
@param y, y position
@return value, value to set in the given position
*/
unsigned char getPV(ImageRGB* ptrImage, int x, int y, int layer){
	unsigned char value = 0;
	assert(x < ptrImage->width && y < ptrImage->height);		
	//translation to default axis origin			
	int cy = (ptrImage->height - 1) - y;		
	value = ptrImage->ptrImRGB[ cy * ptrImage->width * N_LAYERS + x * N_LAYERS + layer];	
	return value;
}

/*
Set pixel value on GS image
Origin: Inferior left pixel
@param ptrImage, GS image to modify
@param x, x position
@param y, y position
@param value, value to set in the given position
*/
void setPV(ImageGS* ptrImage, int x, int y, unsigned char value){
	assert(x < ptrImage->width && y < ptrImage->height);	
	//translation to default axis origin			
	int cy = (ptrImage->height - 1) - y;		
	ptrImage->ptrImGS[ cy * ptrImage->width + x] = value;	
}
/*
gets pixel value on GS image
Origin: Inferior left pixel
@param ptrImage, GS image to modify
@param x, x position
@param y, y position
@return value, value to set in the given position
*/
unsigned char getPV(ImageGS* ptrImage, int x, int y){
	unsigned char value = 0;
	assert(x < ptrImage->width && y < ptrImage->height);
	//translation to default axis settings			
	int cy = (ptrImage->height - 1) - y;		
	value = ptrImage->ptrImGS[ cy * ptrImage->width + x];	
	return value;
}
/*
Set pixel value on Ipl image
Origin: Inferior left pixel
@param ptrImage, Ipl image to modify
@param x, x position
@param y, y position
@param value, value to set in the given position
*/
void setPV(IplImage* ptrCvImage, int x, int y, int layer, unsigned char value){
	int cy = (ptrCvImage->height - 1) - y;		
	unsigned char* ptrRowCv = (unsigned char *)(ptrCvImage->imageData + cy * ptrCvImage->widthStep);
	//translation to default axis origin
	if(ptrCvImage->nChannels == 1){
		ptrRowCv[x] = value;
	}
	if(ptrCvImage->nChannels == 3){				
		ptrRowCv[N_LAYERS * x + layer] = value;		
	}
}
/*
gets pixel value on GS image
Origin: Inferior left pixel
@param ptrImage, GS image to modify
@param x, x position
@param y, y position
@return value, value to set in the given position
*/

unsigned char getPV(IplImage* ptrCvImage, int x, int y, int layer){
	unsigned char value = 0;	
	int cy = (ptrCvImage->height - 1) - y;		
	unsigned char* ptrRowCv = (unsigned char *)(ptrCvImage->imageData + cy * ptrCvImage->widthStep);
	//translation to default axis origin
	if(ptrCvImage->nChannels == 1){
		value = ptrRowCv[x];
	}
	if(ptrCvImage->nChannels == 3){				
		value = ptrRowCv[N_LAYERS * x + layer];		
	}
	return value;
}
