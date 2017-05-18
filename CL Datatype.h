#pragma once
#define CL_HPP_ENABLE_EXCEPTIONS
#pragma warning(disable : 4996)
#include<CL\cl2.hpp>

namespace cl_datatype {
	enum class preferred_device_type {
		CPU, GPU, ACCELERATOR, FPGA, OTHER
	};

	template<typename T>
	struct datatype {
		using _type = T;
		static constexpr bool valid = false; 
		static constexpr preferred_device_type preferred_type = preferred_device_type::OTHER;
		static constexpr const char * src = nullptr;
	};

	template<>
	struct datatype<cl_char> {
		using _type = cl_char;
		static constexpr bool valid = true;
		static constexpr preferred_device_type preferred_type = preferred_device_type::CPU;
		static constexpr const char * src = ""
#include "expand.cl"
#include "char.cl"
			;
	};

	template<>
	struct datatype<cl_short> {
		using _type = cl_short;
		static constexpr bool valid = true;
		static constexpr preferred_device_type preferred_type = preferred_device_type::CPU;
		static constexpr const char * src = ""
#include "expand.cl"
#include "short.cl"
			;
	};

	template<>
	struct datatype<cl_int> {
		using _type = cl_int;
		static constexpr bool valid = true;
		static constexpr preferred_device_type preferred_type = preferred_device_type::CPU;
		static constexpr const char * src = ""
#include "expand.cl"
#include "int.cl"
			;
	};

	template<>
	struct datatype<cl_long> {
		using _type = cl_long;
		static constexpr bool valid = true;
		static constexpr preferred_device_type preferred_type = preferred_device_type::CPU;
		static constexpr const char * src = ""
#include "expand.cl"
#include "long.cl"
			;
	};

	template<>
	struct datatype<cl_float> {
		using _type = cl_float;
		static constexpr bool valid = true;
		static constexpr preferred_device_type preferred_type = preferred_device_type::GPU;
		static constexpr const char * src = ""
#include "expand.cl"
#include "float.cl"
			;
	};

	template<>
	struct datatype<cl_double> {
		using _type = cl_double;
		static constexpr bool valid = true;
		static constexpr preferred_device_type preferred_type = preferred_device_type::GPU;
		static constexpr const char * src = ""
#include "expand.cl"
#include "double.cl"
			;
	};

	template<>
	struct datatype<cl_uchar> {
		using _type = cl_uchar;
		static constexpr bool valid = true;
		static constexpr preferred_device_type preferred_type = preferred_device_type::CPU;
		static constexpr const char * src = ""
#include "expand.cl"
#include "uchar.cl"
			;
	};

	template<>
	struct datatype<cl_ushort> {
		using _type = cl_ushort;
		static constexpr bool valid = true;
		static constexpr preferred_device_type preferred_type = preferred_device_type::CPU;
		static constexpr const char * src = ""
#include "expand.cl"
#include "ushort.cl"
			;
	};

	template<>
	struct datatype<cl_uint> {
		using _type = cl_uint;
		static constexpr bool valid = true;
		static constexpr preferred_device_type preferred_type = preferred_device_type::CPU;
		static constexpr const char * src = ""
#include "expand.cl"
#include "uint.cl"
			;
	};

	template<>
	struct datatype<cl_ulong> {
		using _type = cl_ulong;
		static constexpr bool valid = true;
		static constexpr preferred_device_type preferred_type = preferred_device_type::CPU;
		static constexpr const char * src = ""
#include "expand.cl"
#include "ulong.cl"
			;
	};
}