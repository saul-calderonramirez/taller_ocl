// PruebaOpenCL.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"
//#include <CL/cl.h>
#include <iostream>
#define TAMANIO  10
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


void executeHostCode() {
    // Create the two input vectors
   // Create the two input vectors
    const int LIST_SIZE = 1000;
    int *ptrHostA = new int[LIST_SIZE]; 
    int *ptrHostB = new int[LIST_SIZE];
    for(int i = 0; i < LIST_SIZE; i++) {
        ptrHostA[i] = i;
        ptrHostB[i] = LIST_SIZE - i;
    }
 
   try { 
        // Get available platforms
        vector<Platform> platforms;
        Platform::get(&platforms); 
		//STEP 1: Discover and initialize the platforms---------------------------------------------------
        // Select the default platform and create a context using this platform and the GPU
        cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };
		//STEP 3: Create the context----------------------------------------------------------------------
        Context context( CL_DEVICE_TYPE_GPU, cps); 
        // Get a list of devices on this platform
		//STEP 2: Discover and initialize the devices---------------------------------------------------
        vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>(); 
		//STEP 4: Create command queue------------------------------------------------------------------
        // Create a command queue and use the first device
        CommandQueue queue = CommandQueue(context, devices[0]); 
        // Read source file
		//STEP 7: Create and compile the program------------------------------------------------------------------
        std::ifstream sourceFile("vector_add_kernel.cl");
        std::string sourceCode(std::istreambuf_iterator<char>(sourceFile),(std::istreambuf_iterator<char>()));
        Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1)); 
        // Make program of the source code in the context
        Program program = Program(context, source); 
        // Build program for these specific devices
        program.build(devices); 
        // Make kernel
        Kernel kernel(program, "vector_add"); 
        
		//STEP 5: Create memory buffers------------------------------------------------------------------
        Buffer bufDeviceA = Buffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int));
        Buffer bufDeviceB = Buffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int));
        Buffer bufDeviceRes = Buffer(context, CL_MEM_WRITE_ONLY, LIST_SIZE * sizeof(int)); 
        // Copy lists A and ptrHostB to the memory buffer
		//STEP 6: Copy host memory buffer to device buffer------------------------------------------------------------------
        queue.enqueueWriteBuffer(bufDeviceA, CL_TRUE, 0, LIST_SIZE * sizeof(int), ptrHostA);
        queue.enqueueWriteBuffer(bufDeviceB, CL_TRUE, 0, LIST_SIZE * sizeof(int), ptrHostB); 
        // Set arguments to kernel
        kernel.setArg(0, bufDeviceA);
        kernel.setArg(1, bufDeviceB);
        kernel.setArg(2, bufDeviceRes); 
        // Run the kernel on specific ND range
		//STEP 10: Configure the work item structure ------------------------------------------------------------------
		//Number of threads
        NDRange global(LIST_SIZE);
        NDRange local(1);
		//STEP 11: Enqueue the kernel for execution------------------------------------------------------------------
        queue.enqueueNDRangeKernel(kernel, NullRange, global, local); 
		//STEP 5: Create memory buffers------------------------------------------------------------------
        // Read buffer C into a local list
        int *ptrHostRes = new int[LIST_SIZE];
        queue.enqueueReadBuffer(bufDeviceRes, CL_TRUE, 0, LIST_SIZE * sizeof(int), ptrHostRes);
 
        for(int i = 0; i < LIST_SIZE; i ++)
             std::cout << ptrHostA[i] << " + " << ptrHostB[i] << " = " << ptrHostRes[i] << std::endl; 

		
		delete ptrHostA;
		delete ptrHostB;
		delete ptrHostRes;

    } catch(Error error) {
       std::cout << error.what() << "(" << error.err() << ")" << std::endl;
    }  
   std::cin.get();

}
int main(void) {	
	executeHostCode();
	return EXIT_SUCCESS;
}

