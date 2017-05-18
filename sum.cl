R"D(
constant Scalar IDENTITY = SCALAR_ZERO;

#define REDUCE_IMPL(a, b, indexa, indexb, reduced_value, reduced_index) \
reduced_value = a + b;\
reduced_index = 0;
)D"