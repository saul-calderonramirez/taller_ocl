int getMatrixValue(int* ptrArray, int row, int column){

}


__kernel void vector_add(__global const int *ptrDevA, __global const int *ptrDevB, __global int *ptrDevRes, int rows, int columns) {
	__local float localBuffer[1024];
	
    // Get the index of the current element to be processed
    int curRow = get_global_id(0);
	int curCol = get_global_id(1);	
	__local int elementA;
    __local int elementB;
    __local int value;
	elementA = ptrDevA[curRow * columns + curCol];
	elementB = ptrDevB[curRow * columns + curCol];
    value = elementA + elementB*320 + 564*elementA + elementB % 5;
	ptrDevRes[curRow * columns + curCol] = value;
}