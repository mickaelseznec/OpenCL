#include <iostream>
#include <exception>
#include <string>
#include <vector>

#include <OpenCL/opencl.h>

constexpr size_t STRING_SIZE = 256;

static void check_no_error(cl_int err, std::string str)
{
    if (err != CL_SUCCESS)
    {
        std::cerr << str << std::endl;
        throw std::runtime_error(str);
    }
}

static void print_device_info(cl_device_id device, cl_device_id default_id)
{
    cl_int err {CL_SUCCESS};

    std::cout << "Id: " << device << std::endl;
    std::cout << "Default: " << (device == default_id ? "Yes": "No") << std::endl;

    char str[STRING_SIZE];
    err = clGetDeviceInfo(device, CL_DEVICE_NAME, STRING_SIZE * sizeof(char), str, NULL);
    check_no_error(err, "Could not query device");
    std::cout << "Name: " << str << std::endl;

    err = clGetDeviceInfo(device, CL_DEVICE_VENDOR, STRING_SIZE * sizeof(char), str, NULL);
    check_no_error(err, "Could not query device");
    std::cout << "Vendor: " << str << std::endl;

    err = clGetDeviceInfo(device, CL_DEVICE_VERSION, STRING_SIZE * sizeof(char), str, NULL);
    check_no_error(err, "Could not query device");
    std::cout << "Version: " << str << std::endl;

    cl_device_type type {0};
    err = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, NULL);
    check_no_error(err, "Could not query device");
    std::cout << "Type(s):" << std::endl;
    if (type & CL_DEVICE_TYPE_CPU) {
        std::cout << '\t' << "CL_DEVICE_TYPE_CPU" << std::endl;
    }
    if (type & CL_DEVICE_TYPE_GPU) {
        std::cout << '\t' << "CL_DEVICE_TYPE_GPU" << std::endl;
    }
    if (type & CL_DEVICE_TYPE_ACCELERATOR) {
        std::cout << '\t' <<  "CL_DEVICE_TYPE_ACCELERATOR" << std::endl;
    }
    if (type & CL_DEVICE_TYPE_DEFAULT) {
        std::cout << '\t' <<  "CL_DEVICE_TYPE_DEFAULT" << std::endl;
    }

    cl_uint units {0};
    err = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(units), &units, NULL);
    check_no_error(err, "Could not query device");
    std::cout << "Compute units: " << units << std::endl;

    cl_device_local_mem_type mem_type;
    err = clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(mem_type), &mem_type, NULL);
    check_no_error(err, "Could not query device");
    std::string mem_type_string;
    cl_device_info query;
    if (mem_type == CL_GLOBAL) {
        mem_type_string = "Unified";
        query = CL_DEVICE_GLOBAL_MEM_SIZE;

    } else if (mem_type == CL_LOCAL) {
        mem_type_string = "Dedicated";
        query = CL_DEVICE_LOCAL_MEM_SIZE;
    } else {
        throw std::runtime_error("Unknown memory type");
    }
    std::cout << "Memory type: " << mem_type_string << std::endl;

    cl_ulong mem_size {0};
    err = clGetDeviceInfo(device, query, sizeof(mem_size), &mem_size, NULL);
    check_no_error(err, "Could not query device");
    std::cout << "Memory size: " << mem_size << std::endl;

    size_t group_size {0};
    err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(group_size), &group_size, NULL);
    check_no_error(err, "Could not query device");
    std::cout << "Max group size: " << group_size << std::endl;

    cl_uint max_dimensions {0};
    err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(max_dimensions), &max_dimensions, NULL);
    check_no_error(err, "Could not query device");

    std::vector<size_t> max_items_per_dim(max_dimensions);
    err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * max_items_per_dim.size(), max_items_per_dim.data(), NULL);
    check_no_error(err, "Could not query device");

    std::cout << "Max items per dimension: " << max_items_per_dim[0];
    for (size_t i = 1; i < max_items_per_dim.size(); i++) {
        std::cout << ", " << max_items_per_dim[i];
    }
    std::cout << std::endl;

    cl_bool image_support {false};
    err = clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(image_support), &image_support, NULL);
    check_no_error(err, "Could not query device");
    std::cout << "Images supported: " << (image_support ? "Yes" : "No") << std::endl;

    err = clGetDeviceInfo(device, CL_DEVICE_BUILT_IN_KERNELS, STRING_SIZE * sizeof(char), str, NULL);
    check_no_error(err, "Could not query device");
    if (str[0] != '\0') {
        std::cout << "Supported built-in kernels: " << str << std::endl;
    }

    std::cout << std::endl << std::endl;
}

int main(void)
{
    cl_int err {CL_SUCCESS};
    cl_uint device_id_size {0};

    err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 0, NULL, &device_id_size);
    check_no_error(err, "Could not get device ids");

    std::vector<cl_device_id> device_ids {device_id_size};
    err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, device_id_size, device_ids.data(), NULL);
    check_no_error(err, "Could not get device ids");

    cl_device_id default_id {0};
    err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_DEFAULT, 1, &default_id, NULL);
    check_no_error(err, "Could not get device ids");

    std::cout << device_id_size << " device(s) available on this platform" << std::endl << std::endl;

    for (auto device : device_ids) {
        print_device_info(device, default_id);
    }

    return EXIT_SUCCESS;
}
