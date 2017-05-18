#pragma once
#include "CL Datatype.h"
#include "CL Reduction Type.h"
#include<iostream>
#include<optional>
#include<future>

namespace cl_reduction {
	template<typename DataType, typename ReductionType>
	class reduction_context {
	public:
		using datatype = typename cl_datatype::datatype<typename DataType::_type>;
		using reduction = cl_reduction_type::reduction_type<ReductionType::_type>;
		using T = typename datatype::_type;
		using result = cl_reduction_type::result_struct<T>;
		static_assert(datatype::valid && reduction::valid, "Invalid Data Type or Reduction Type!");

		static constexpr const char * main_src = ""
#include "Reduction Main.cl"
			;

	private:

		cl::Context context;
		cl::CommandQueue queue;
		cl::Program program;
		cl::Kernel kernel;
		uint32_t compute_units;
		uint64_t work_group_size;

		static constexpr uint32_t get_ideal_work_group_size(uint32_t compute_units) {
			uint32_t working = compute_units * 128;
			working--;
			working |= working >> 1;
			working |= working >> 2;
			working |= working >> 4;
			working |= working >> 8;
			working |= working >> 16;
			working++;
			return working;
		}

	public:
		reduction_context(std::vector<cl::Device> const& devices, std::optional<std::reference_wrapper<std::ostream>> err_log = {}) :
		context(devices),
		queue(context, CL_QUEUE_PROFILING_ENABLE){
			for (auto const& device : devices) {
				std::cout << "Device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
				std::cout << "Platform: " << cl::Platform(device.getInfo<CL_DEVICE_PLATFORM>()).getInfo<CL_PLATFORM_NAME>() << std::endl;
			}
			std::string src = std::string(datatype::src) + reduction::src + main_src;
			try {
				program = cl::Program{ context, src };
				program.build();
			}
			catch (cl::BuildError const& e) {
				std::ostream & log = (err_log) ? *err_log : std::cerr;
				log << e.what() << std::endl;
				auto raw_log = e.getBuildLog();
				auto build_log = raw_log[0].second;
				log << build_log << std::endl;
				log << '\n' << src << std::endl;
				throw std::runtime_error("Unable to build Kernel");
			}
			std::vector<cl::Kernel> kernels;
			program.createKernels(&kernels);
			kernel = kernels[0];

			compute_units = 8;
			std::cout << "Num of compute units: " << compute_units << std::endl;
			for (auto const& device : devices) {
				work_group_size = std::min(get_ideal_work_group_size(compute_units), uint32_t(device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>()));
			}
			std::cout << "Work Group Size: " << work_group_size << std::endl;
		}

		std::future<result> perform_reduction(std::vector<T> const& values) {
			cl_long size = values.size();
			uint64_t num_of_work_groups = size / work_group_size;
			int64_t global_size = work_group_size * num_of_work_groups;
			if (global_size < size) {
				num_of_work_groups++;
				global_size = work_group_size * num_of_work_groups;
			}
			cl::Buffer input_buffer(context, CL_MEM_READ_ONLY, global_size * sizeof(T), nullptr);
			std::vector<cl::Event> write_events(1);
			queue.enqueueWriteBuffer(input_buffer, false, 0, size * sizeof(T), values.data(), nullptr, &write_events.back());
			if (global_size != size) {
				write_events.emplace_back();
				queue.enqueueFillBuffer(input_buffer, reduction::identity<T>(), size * sizeof(T), (global_size - size) * sizeof(T), nullptr, &write_events.back());
			}
			return std::async([size, num_of_work_groups, global_size, input_buffer, write_events, this] {
				cl::Buffer output_buffer( context, CL_MEM_WRITE_ONLY, num_of_work_groups * sizeof(T) );
				cl::Buffer output_index_buffer(context, CL_MEM_WRITE_ONLY, num_of_work_groups * sizeof(cl_long));
				kernel.setArg(0, input_buffer);
				kernel.setArg(1, output_buffer);
				kernel.setArg(2, sizeof(T) * work_group_size, nullptr);
				kernel.setArg(3, output_index_buffer);
				kernel.setArg(4, sizeof(cl_long) * work_group_size, nullptr);
				kernel.setArg(5, size);

				std::vector<cl::Event> kernel_event;
				kernel_event.emplace_back();
				queue.enqueueNDRangeKernel(kernel, {}, { uint64_t(global_size) }, { work_group_size }, &write_events, &kernel_event.back());
				std::vector<T> results;
				std::vector<int64_t> indexes;
				results.resize(num_of_work_groups);
				indexes.resize(num_of_work_groups);
				queue.enqueueReadBuffer(output_buffer, false, 0, num_of_work_groups * sizeof(T), results.data(), &kernel_event);
				queue.enqueueReadBuffer(output_index_buffer, false, 0, num_of_work_groups * sizeof(cl_long), indexes.data(), &kernel_event);
				queue.finish();
				std::cout << "Internal Duration: " << (kernel_event[0].getProfilingInfo<CL_PROFILING_COMMAND_END>() - kernel_event[0].getProfilingInfo<CL_PROFILING_COMMAND_START>()) << "ns" << std::endl;
				std::cout << "Num of Work Groups to sum up: " << num_of_work_groups << std::endl;
				result t{ reduction::identity<T>(), 0 };
				for (size_t i = 0; i < results.size(); i++) {
					T const& val = results[i];
					size_t const& index = indexes[i];
					t = reduction::reduce(t.reduced_value, val, t.reduced_index, index);
				}
				return t;
			});
		}
	};
}