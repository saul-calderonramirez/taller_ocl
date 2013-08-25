// stdafx.h: archivo de inclusión de los archivos de inclusión estándar del sistema
// o archivos de inclusión específicos de un proyecto utilizados frecuentemente,
// pero rara vez modificados
//

#pragma once
#define ROWS 1200
#define COLUMNS 1920
#define TOTAL_ELEMENTS  ROWS * COLUMNS
#include "targetver.h"
#include <cv.h>
#include <cxcore.h>
#include <stdio.h>
#include <tchar.h>
#include "Image.h"
#include "WrapOCL.h"
#include <highgui.h>

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

#include <sys/timeb.h>
using namespace cl;
// TODO: mencionar aquí los encabezados adicionales que el programa necesita
