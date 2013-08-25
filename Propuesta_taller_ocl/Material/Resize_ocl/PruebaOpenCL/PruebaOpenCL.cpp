// PruebaOpenCL.cpp: define el punto de entrada de la aplicación de consola.
//
#pragma once

#include "stdafx.h"

using namespace cl;
int getMilliCount(){
	timeb tb;
	ftime(&tb);
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}

int getMilliSpan(int nTimeStart){
	int nSpan = getMilliCount() - nTimeStart;
	if(nSpan < 0)
		nSpan += 0x100000 * 1000;
	return nSpan;
}

int* operationSerial(int* ptrHostA, int* ptrHostB ){
	 int* ptrHostRes = new int[TOTAL_ELEMENTS];
	 for(int i = 0; i < TOTAL_ELEMENTS; i++) {
		int elementA = ptrHostA[i];
		int elementB = ptrHostB[i];
		int value = elementA + elementB*320 + 564*elementA + elementB % 5;
		ptrHostRes[i] = value;
    }	
	 return ptrHostRes;
}

int* operationOCL(int* ptrHostA, int* ptrHostB ){
	int* ptrHostRes = NULL;
	try{
		WrapOCL wrapOCL("vector_add_kernel.cl", "vector_add");	
		wrapOCL.loadArrayToDevice(true, TOTAL_ELEMENTS * sizeof(int), ptrHostA, 0);
		wrapOCL.loadArrayToDevice(true, TOTAL_ELEMENTS * sizeof(int), ptrHostB, 1);
		//NULL pointer indicates that no data will be copied to the device
		Buffer* ptrDevRes = wrapOCL.loadArrayToDevice(false, TOTAL_ELEMENTS * sizeof(int), ptrHostRes, 2);
		wrapOCL.loadScalarToDevice(true, ROWS, 3);
		wrapOCL.loadScalarToDevice(true, COLUMNS, 4);

		wrapOCL.setGlobalSize(ROWS, COLUMNS);
		wrapOCL.setLocalSize(1,1);

		int startMilli = getMilliCount();
		wrapOCL.executeKernel();
		int endMilli = getMilliSpan(startMilli);
		std::cout<< "Time OCL no param ms: " <<endMilli << " Total elements: " << TOTAL_ELEMENTS << std::endl;


		ptrHostRes = new int[TOTAL_ELEMENTS];
		for(int i = 0; i < TOTAL_ELEMENTS; i++) {
			ptrHostRes[i] = -1;			
		}

		wrapOCL.readBufferFromDevice(TOTAL_ELEMENTS * sizeof(int), ptrDevRes, ptrHostRes);		
		delete ptrDevRes;
		
	}
	catch(Error error ){
		std::cout << error.what() << "(" << error.err() << ")" << std::endl;
	}
	return ptrHostRes;
}

void testWrapOCL(){
	//TOTAL_ELEMENTS = ROWS * COLUMNS;	
    int *ptrHostA = new int[TOTAL_ELEMENTS]; 
    int *ptrHostB = new int[TOTAL_ELEMENTS];	
	int *ptrHostRes = new int[TOTAL_ELEMENTS];	
    for(int i = 0; i < TOTAL_ELEMENTS; i++) {
        ptrHostA[i] = i;
        ptrHostB[i] = TOTAL_ELEMENTS - i;
		ptrHostRes[i] = 0;
    }	
	int startMilli = getMilliCount();
	ptrHostRes = operationOCL(ptrHostA, ptrHostB);
	int endMilli = getMilliSpan(startMilli);
	std::cout<< "Time OCL ms: " <<endMilli << " Total elements: " << TOTAL_ELEMENTS << std::endl;


	startMilli = getMilliCount();
	ptrHostRes = operationSerial(ptrHostA, ptrHostB);
	endMilli = getMilliSpan(startMilli);
	std::cout<< "Time Serial ms: " <<endMilli << " Total elements: " << TOTAL_ELEMENTS << std::endl;
	/*for(int i = 0; i < TOTAL_ELEMENTS; i ++){
        std::cout << ptrHostA[i] << " + " << ptrHostB[i] << " = " << ptrHostRes[i] << std::endl; 
	}*/

	delete ptrHostA;
	delete ptrHostB;
	delete ptrHostRes;
	
	
}

int main(void) {
	//const long double sysTime = time(0);
	//const long double sysTimeMS = sysTime*1000;

//	std::cout << "System Time in milliseconds is " << sysTimeMS << "." << std::endl;
	testWrapOCL();
	std::cin.get();
	return EXIT_SUCCESS;
}

