#pragma once

namespace cl_reduction_type {
	template<typename T>
	struct result_struct {
		T reduced_value;
		size_t reduced_index;
	};

	enum class type {
		minimum, maximum, accumulation
	};

	template<type T>
	struct reduction_type {
		static constexpr type _type = T;
		static constexpr bool valid = false;
		static constexpr const char * src = nullptr;

		template<typename _T>
		static constexpr result_struct<_T> reduce(_T a, _T b, size_t indexa, size_t indexb) {
			return {a, indexa};
		}
	};

	template<>
	struct reduction_type<type::minimum> {
		static constexpr type _type = type::minimum;
		static constexpr bool valid = true;
		static constexpr const char * src = ""
#include "min.cl"
			;

		template<typename T>
		static constexpr result_struct<T> reduce(T a, T b, size_t indexa, size_t indexb) {
			return (a < b) ? result_struct<T>{a, indexa} : result_struct<T>{ b, indexb };
		}

		template<typename T>
		static constexpr T identity() {
			return std::numeric_limits<T>::max();
		}
	};

	template<>
	struct reduction_type<type::maximum> {
		static constexpr type _type = type::maximum;
		static constexpr bool valid = true;
		static constexpr const char * src = ""
#include "max.cl"
			;

		template<typename T>
		static constexpr result_struct<T> reduce(T a, T b, size_t indexa, size_t indexb) {
			return (a > b) ? result_struct<T>{a, indexa} : result_struct<T>{b, indexb};
		}

		template<typename T>
		static constexpr T identity() {
			return std::numeric_limits<T>::lowest();
		}
	};

	template<>
	struct reduction_type<type::accumulation> {
		static constexpr type _type = type::accumulation;
		static constexpr bool valid = true;
		static constexpr const char * src = ""
#include "sum.cl"
			;

		template<typename T>
		static constexpr result_struct<T> reduce(T a, T b, size_t indexa, size_t indexb) {
			return {a + b, 0};
		}

		template<typename T>
		static constexpr T identity() {
			return {};
		}
	};
}