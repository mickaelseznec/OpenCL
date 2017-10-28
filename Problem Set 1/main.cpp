//Udacity HW1 Solution

#include <iostream>
#include "timer.h"
#include "utils.h"
#include <string>
#include <stdio.h>
#include "reference_calc.h"
#include "compare.h"
#include "custom_types.h"

#include <OpenCL/opencl.h>

//include the definitions of the above functions for this homework
#include "HW1.cpp"

const char *basename = "student_func";
const char *kernel_name = "rgba_to_greyscale";

cl_device_type   device_type;
cl_device_id     device;
cl_context       context;
cl_command_queue queue;
cl_program       program;
cl_kernel        kernel;

void your_rgba_to_greyscale(const uchar4 * const h_rgbaImage, cl_mem d_rgbaImage,
                            cl_mem d_greyImage, size_t numRows, size_t numCols)
{
  cl_int err = CL_SUCCESS;
  err |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_rgbaImage);
  err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_greyImage);
  err |= clSetKernelArg(kernel, 2, sizeof(size_t), &numRows);
  err |= clSetKernelArg(kernel, 3, sizeof(size_t), &numCols);
  checkOpenCLErrors(err, "Set args");

  size_t global[] = {numCols, numRows};
  err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, NULL, 0, NULL, NULL);
  checkOpenCLErrors(err, "Enqueue kernel");

  //You must fill in the correct sizes for the blockSize and gridSize
  //currently only one block with one thread is being launched
  /*TODO
  const dim3 blockSize(1, 1, 1);
  const dim3 gridSize( 1, 1, 1);  //TODO
  rgba_to_greyscale<<<gridSize, blockSize>>>(d_rgbaImage, d_greyImage, numRows, numCols);
  
  cudaDeviceSynchronize(); checkCudaErrors(cudaGetLastError());
  */

}

static void initOpenCL()
{
    cl_int err;
    err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_DEFAULT, 1, &device, NULL);
    checkOpenCLErrors(err, "Cannot get device id");

    cl_uint address_bits = 0;
    err = clGetDeviceInfo(device, CL_DEVICE_ADDRESS_BITS, sizeof(cl_uint), &address_bits, NULL);
    checkOpenCLErrors(err, "Cannot get address bits");

    if (address_bits != 32 && address_bits != 64) {
        std::cerr << "Unknown address bits: " << address_bits << std::endl;
        exit(1);
    }

    err = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);
    checkOpenCLErrors(err, "Cannot get device type");

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    checkOpenCLErrors(err, "clCreateContext");

    queue = clCreateCommandQueue(context, device, 0, &err);
    checkOpenCLErrors(err, "clCreateCommandQueue");

      unsigned int i;
    std::string bitcode_path = basename;
    if (device_type == CL_DEVICE_TYPE_GPU) {
        bitcode_path += ".gpu";
    } else {
        bitcode_path += ".cpu";
    }
    bitcode_path += std::to_string(address_bits) + ".bc";

    size_t len = bitcode_path.size();
    const char *data = bitcode_path.c_str();

    program = clCreateProgramWithBinary(context, 1, &device, &len,
            (const unsigned char**)&data, NULL, &err);
    checkOpenCLErrors(err, "clCreateProgramWithBinary");

    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    checkOpenCLErrors(err, "clBuildProgram");

    kernel = clCreateKernel(program, kernel_name, &err);
    checkOpenCLErrors(err, "clCreateKernel");
}

static void destroyOpenCL()
{
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);
}

int main(int argc, char **argv) {
  uchar4        *h_rgbaImage;
  unsigned char *h_greyImage;
  cl_mem d_rgbaImage, d_greyImage;

  std::string input_file;
  std::string output_file;
  std::string reference_file;
  double perPixelError = 0.0;
  double globalError   = 0.0;
  bool useEpsCheck = false;
  switch (argc)
  {
	case 2:
	  input_file = std::string(argv[1]);
	  output_file = "HW1_output.png";
	  reference_file = "HW1_reference.png";
	  break;
	case 3:
	  input_file  = std::string(argv[1]);
      output_file = std::string(argv[2]);
	  reference_file = "HW1_reference.png";
	  break;
	case 4:
	  input_file  = std::string(argv[1]);
      output_file = std::string(argv[2]);
	  reference_file = std::string(argv[3]);
	  break;
	case 6:
	  useEpsCheck=true;
	  input_file  = std::string(argv[1]);
	  output_file = std::string(argv[2]);
	  reference_file = std::string(argv[3]);
	  perPixelError = atof(argv[4]);
      globalError   = atof(argv[5]);
	  break;
	default:
      std::cerr << "Usage: ./HW1 input_file [output_filename] [reference_filename] [perPixelError] [globalError]" << std::endl;
      exit(1);
  }

  initOpenCL();

  //load the image and give us our input and output pointers
  preProcess(&h_rgbaImage, &h_greyImage, &d_rgbaImage, &d_greyImage, input_file);

  GpuTimer timer;
  timer.Start();
  //call the students' code
  your_rgba_to_greyscale(h_rgbaImage, d_rgbaImage, d_greyImage, numRows(), numCols());
  timer.Stop();
  //TODO cudaDeviceSynchronize(); checkCudaErrors(cudaGetLastError());

  int err = printf("Your code ran in: %f msecs.\n", timer.Elapsed());

  if (err < 0) {
    //Couldn't print! Probably the student closed stdout - bad news
    std::cerr << "Couldn't print timing information! STDOUT Closed!" << std::endl;
    exit(1);
  }

  size_t numPixels = numRows()*numCols();

  err = clEnqueueReadBuffer(queue, d_greyImage, CL_TRUE, 0, numPixels * sizeof(unsigned char), h_greyImage, 0, NULL, NULL);
  checkOpenCLErrors(err, "Read buffer back");
  //TODO checkCudaErrors(cudaMemcpy(h_greyImage, d_greyImage, sizeof(unsigned char) * numPixels, cudaMemcpyDeviceToHost));

  //check results and output the grey image
  postProcess(output_file, h_greyImage);

  referenceCalculation(h_rgbaImage, h_greyImage, numRows(), numCols());

  postProcess(reference_file, h_greyImage);

  //generateReferenceImage(input_file, reference_file);
  compareImages(reference_file, output_file, useEpsCheck, perPixelError, 
                globalError);

  cleanup();
  destroyOpenCL();

  return 0;
}
