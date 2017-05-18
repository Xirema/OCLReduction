R"D(
constant Scalar IDENTITY = SCALAR_MAXIMUM;

#define REDUCE_IMPL(a, b, indexa, indexb, reduced_value, reduced_index) \
if(a < b) {\
	reduced_value = a;\
	reduced_index = indexa;\
} else {\
	reduced_value = b;\
	reduced_index = indexb;\
}
)D"