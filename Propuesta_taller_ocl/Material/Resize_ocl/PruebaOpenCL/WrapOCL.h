#pragma once
#include <iostream>
#define NO_PARAMETER  -1
//compiler bug
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#define __NO_STD_STRING
#define __NO_STD_VECTOR // Use cl::vector instead of STL version
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <utility>
#include <iostream>
#include <fstream>
#include <string>
using namespace cl;
class WrapOCL{
	
	vector<Platform> platforms;
	Context* ptrContext;
	vector<Device> devices;
	CommandQueue* ptrQueue;
	std::ifstream* ptrSourceFile;
	std::string* ptrSourceCode;
	Program::Sources* ptrSourceProgram;
	Program* ptrProgram;
	Kernel* ptrDevKernel;
	NDRange* ptrGlobalNumThreads;
	NDRange* ptrLocalNumThreads;

public:
	
	/*
	Loads, compiles, and creates the kernel on the file specified, with the function specified
	*/
	void loadKernel(char* ptrFileName, char* ptrFunction);
	/*
	Writes the host buffer on the device and loads it to the kernel
	*/	
	template <class T> Buffer* loadArrayToDevice(bool readOnly, int size, T ptrHostBuffer, int numParam)throw (Error){
		
		int typeBuffer = CL_MEM_READ_ONLY;		
		Buffer* ptrDevBuffer = NULL;
		//BUG POSIBLE!!
		if(this->ptrContext != NULL && this->ptrQueue != NULL && this->ptrDevKernel != NULL){
			if(!readOnly){
				typeBuffer = CL_MEM_WRITE_ONLY;
			}
			ptrDevBuffer = new Buffer(*this->ptrContext, typeBuffer, size);
			if(ptrHostBuffer != NULL){
				this->ptrQueue->enqueueWriteBuffer(*ptrDevBuffer, CL_TRUE, 0, size, ptrHostBuffer);
			}
			this->ptrDevKernel->setArg(numParam, *ptrDevBuffer);
		}
		else{
			throw new Error(1, "The OCL device has not been initiated");
		}
		return ptrDevBuffer;
	}		

	template <class T> void loadScalarToDevice(bool readOnly,  T hostBuffer, int numParam)throw (Error){
		std::cout<< "El host buffer " << hostBuffer<< std::endl;
		int typeBuffer = CL_MEM_READ_ONLY;				
		T temp = hostBuffer;
		//BUG POSIBLE!!
		if(this->ptrContext != NULL && this->ptrQueue != NULL && this->ptrDevKernel != NULL){
			if(!readOnly){
				typeBuffer = CL_MEM_WRITE_ONLY;
			}			
			this->ptrDevKernel->setArg(numParam, temp);
		}
		else{
			throw new Error(1, "The OCL device has not been initiated");
		}		
	}
	
	/*
	Executes the loaded kernel	
	*/
	
//	void setGlobalSize(int dim1, int dim2);
	void setGlobalSize(int dim1, int dim2 = NO_PARAMETER, int dim3 = NO_PARAMETER);
	
	void setLocalSize(int dim1, int dim2 = NO_PARAMETER, int dim3 = NO_PARAMETER);	
	//void setLocalSize(int dim1, int dim2);
	
	void executeKernel();
	/*
	Reads back the buffer from the device
	*/
	template <class T> void readBufferFromDevice(int size, Buffer* ptrDeviceBuffer, T ptrHostBuffer){
		if(this->ptrQueue != NULL){
			this->ptrQueue->enqueueReadBuffer(*ptrDeviceBuffer, CL_TRUE, 0, size, ptrHostBuffer); 
		}
	}
	/*
	Inits the GPU device on the default platform number 0
	Creates its context and command queue
	*/
	WrapOCL(void);
	WrapOCL(char* ptrFileNameKernel, char* ptrFunction)throw (Error);
	~WrapOCL(void);
};
