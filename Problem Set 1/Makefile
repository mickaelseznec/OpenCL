CLC=/System/Library/Frameworks/OpenCL.framework/Libraries/openclc
CLFLAGS =-emit-llvm -cl-fp32-correctly-rounded-divide-sqrt

OPENCV_LIBPATH=/usr/local/lib
OPENCV_INCLUDEPATH=/usr/local/include

OPENCV_LIBS=-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs

GCC_OPTS=-O3 -Wall -Wextra -m64

CL_SOURCES:=student_func.cl
BITCODE += ${CL_SOURCES:.cl=.cpu32.bc}
BITCODE += ${CL_SOURCES:.cl=.cpu64.bc}
BITCODE += ${CL_SOURCES:.cl=.gpu32.bc}
BITCODE += ${CL_SOURCES:.cl=.gpu64.bc}

student: main.o $(BITCODE) compare.o reference_calc.o Makefile
	g++ -o HW1 main.o compare.o reference_calc.o -framework OpenCL -L $(OPENCV_LIBPATH) $(OPENCV_LIBS)

main.o: main.cpp timer.h utils.h reference_calc.cpp compare.cpp HW1.cpp
	g++ -c main.cpp $(GCC_OPTS) -framework OpenCL -I $(OPENCV_INCLUDEPATH)

compare.o: compare.cpp compare.h
	g++ -c compare.cpp -I $(OPENCV_INCLUDEPATH) $(GCC_OPTS)

reference_calc.o: reference_calc.cpp reference_calc.h
	g++ -c reference_calc.cpp -I $(OPENCV_INCLUDEPATH) $(GCC_OPTS)

%.cpu32.bc: %.cl
	$(CLC) $(CLFLAGS) -c -arch i386 $< -o $@

%.cpu64.bc: %.cl
	$(CLC) $(CLFLAGS) -c -arch x86_64 $< -o $@

%.gpu32.bc: %.cl
	$(CLC) $(CLFLAGS) -c -arch gpu_32 $< -o $@

%.gpu64.bc: %.cl
	$(CLC) $(CLFLAGS) -c -arch gpu_64 $< -o $@

clean:
	rm -f *.o *.png hw $(BITCODE)
