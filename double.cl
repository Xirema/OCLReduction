R"D(
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

EXPAND(double);

#define SCALAR_MAXIMUM INFINITY;
#define SCALAR_MINIMUM -INFINITY;
#define SCALAR_ZERO 0;
)D"