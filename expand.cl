R"D(
#define EXPAND(type) \
typedef		type		Scalar;\
typedef		type ## 2	Vector2;\
typedef		type ## 4	Vector4;\
typedef		type ## 8	Vector8;\
typedef		type ## 16	Vector16;
)D"