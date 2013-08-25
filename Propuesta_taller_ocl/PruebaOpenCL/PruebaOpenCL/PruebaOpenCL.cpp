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
		vector<Device> devices;
        Platform::get(&platforms); 
		platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
		
		//STEP 1: Discover and initialize the platforms---------------------------------------------------
        // Select the default platform and create a context using this platform and the GPU      
		//STEP 2: Create the context----------------------------------------------------------------------
        Context context(devices);         
        // Read source file
		//STEP 3: Create and compile the program------------------------------------------------------------------
        std::ifstream sourceFile("vector_add_kernel.cl");
        std::string sourceString(std::istreambuf_iterator<char>(sourceFile),(std::istreambuf_iterator<char>()));
        Program::Sources source(1, std::make_pair(sourceString.c_str(), sourceString.length() + 1)); 
        // Make program of the source code in the context
        Program program = Program(context, source); 
        // Build program for these specific devices
        program.build(); 
        // Make kernel
        Kernel kernel(program, "vector_add"); 
        //STEP 4: Create command queue------------------------------------------------------------------
        // Create a command queue and use the first device
        CommandQueue queue = CommandQueue(context, devices[0]); 
		
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
		//STEP 7: Configure the work item structure ------------------------------------------------------------------
		//Number of threads
        NDRange global(LIST_SIZE);
        NDRange local(1);
		//STEP 8: Enqueue the kernel for execution------------------------------------------------------------------
        queue.enqueueNDRangeKernel(kernel, NullRange, global, local); 
		//STEP 9: Create memory buffers------------------------------------------------------------------
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

