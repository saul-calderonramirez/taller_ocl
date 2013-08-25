#pragma once
#include "stdafx.h"
#include "WrapOCL.h"
using namespace cl;

WrapOCL::WrapOCL(void){
}

WrapOCL::~WrapOCL(void){	
	delete this->ptrContext;
	delete this->ptrDevKernel;
	delete this->ptrGlobalNumThreads;
	delete this->ptrLocalNumThreads;
	delete this->ptrProgram;
	delete this->ptrQueue;
	delete this->ptrSourceCode;
	delete this->ptrSourceFile;
	delete this->ptrSourceProgram;
}

WrapOCL::WrapOCL(char* ptrFileNameKernel, char* ptrFunction)throw (Error){
	vector<Platform> platforms;
	//Gets the available platforms
	Platform::get(&platforms);
	//STEP 1: Discover and initialize the platforms---------------------------------------------------
    // Select the default platform and create a context using this platform and the GPU
    cl_context_properties contextProperties[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };
	//STEP 3: Create the context----------------------------------------------------------------------
	this->ptrContext = new Context( CL_DEVICE_TYPE_GPU, contextProperties);
	// Get a list of devices on this platform
	//STEP 2: Discover and initialize the devices---------------------------------------------------
    this->devices = this->ptrContext->getInfo<CL_CONTEXT_DEVICES>(); 
	//STEP 4: Create command queue------------------------------------------------------------------
    // Create a command queue and use the first device
    this->ptrQueue = new CommandQueue(*ptrContext, devices[0]); 
	// Read source file
	//STEP 7: Create and compile the program------------------------------------------------------------------
	this->ptrSourceFile = new std::ifstream(ptrFileNameKernel);
	this->ptrSourceCode = new std::string(std::istreambuf_iterator<char>(*ptrSourceFile),(std::istreambuf_iterator<char>()));
	this->ptrSourceProgram = new Program::Sources(1, std::make_pair(ptrSourceCode->c_str(), ptrSourceCode->length() + 1)); 
	// Make program of the source code in the context
    this->ptrProgram = new Program(*ptrContext, *ptrSourceProgram); 
	//Compiles the program on the devices
	this->ptrProgram->build(this->devices);
	//Creates the kernel with the compiled program
	this->ptrDevKernel = new Kernel(*ptrProgram, ptrFunction);
}




/*
void WrapOCL::setGlobalSize(int dim1, int dim2){
	this->ptrGlobalNumThreads = new NDRange(dim1, dim2);
}*/

void WrapOCL::setGlobalSize(int dim1,  int dim2, int dim3){
	if(dim1 != NO_PARAMETER && dim2 == NO_PARAMETER && dim3 == NO_PARAMETER)
		this->ptrGlobalNumThreads = new NDRange(dim1);
	if(dim1 != NO_PARAMETER && dim2 != NO_PARAMETER && dim3 == NO_PARAMETER)
		this->ptrGlobalNumThreads = new NDRange(dim1, dim2);
	if(dim1 != NO_PARAMETER && dim2 != NO_PARAMETER && dim3 != NO_PARAMETER)
		this->ptrGlobalNumThreads = new NDRange(dim1, dim2, dim3);
}

/*
void WrapOCL::setLocalSize(int dim1, int dim2){
	this->ptrLocalNumThreads = new NDRange(dim1, dim2);
}
*/
void WrapOCL::setLocalSize(int dim1, int dim2, int dim3){
	if(dim1 != NO_PARAMETER && dim2 == NO_PARAMETER && dim3 == NO_PARAMETER)
		this->ptrLocalNumThreads = new NDRange(dim1);
	if(dim1 != NO_PARAMETER && dim2 != NO_PARAMETER && dim3 == NO_PARAMETER)
		this->ptrLocalNumThreads = new NDRange(dim1, dim2);
	if(dim1 != NO_PARAMETER && dim2 != NO_PARAMETER && dim3 != NO_PARAMETER)
		this->ptrLocalNumThreads = new NDRange(dim1, dim2, dim3);
}


void WrapOCL::executeKernel(){
	
	this->ptrQueue->enqueueNDRangeKernel(*this->ptrDevKernel, NullRange, *this->ptrGlobalNumThreads, *this->ptrLocalNumThreads); 
}