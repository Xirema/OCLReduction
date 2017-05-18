#define _HAS_AUTO_PTR_ETC 1
#include <vector>
#include <list>
#include <memory>
#include <utility>
#include<fstream>
#include<chrono>
#include<numeric>
#include<random>
#include<iomanip>

#include "CL Reduction.h"

int main() {
	using type = float;
	using reduction_type = cl_reduction_type::reduction_type<cl_reduction_type::type::maximum>;
	using datatype = cl_datatype::datatype<type>;
	using context_t = cl_reduction::reduction_context<datatype, reduction_type>;
	std::ofstream err_log{ "err.txt" };
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	std::vector<cl::Device> our_devices;
	std::vector<cl::Device> devices;
	platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devices);
	devices.erase(devices.begin() + 2, devices.end());
	our_devices.insert(our_devices.end(), devices.begin(), devices.end());
	platforms[1].getDevices(CL_DEVICE_TYPE_ALL, &devices);
	our_devices.insert(our_devices.end(), devices.begin(), devices.end());

	try {
		cl_reduction::reduction_context<datatype, reduction_type> context{ {our_devices.back()}, err_log };
		std::vector<type> values;
		auto last_ping = std::chrono::steady_clock::now();
		std::default_random_engine engine{ std::random_device{}() };
		std::uniform_real_distribution<type> distribution{ -50'000.f, 0.f };
		//std::uniform_int_distribution<type> distribution(1, 500);
		values.resize(10'000ull);
		//values.resize(10'000);
		for (size_t i = 0; i < values.size(); i++) {
			values[i] = distribution(engine);
			if (std::chrono::steady_clock::now() - last_ping > std::chrono::seconds(1)) {
				std::cout << "i = " << i << '\r';
				last_ping += std::chrono::seconds(1);
			}
		}

		auto begin = std::chrono::steady_clock::now();
		auto future = context.perform_reduction(values);
		context_t::result t;
		try {
			t = future.get();
		}
		catch (cl::Error const& e) {
			err_log << e.what() << std::endl;
			err_log << e.err() << std::endl;

		}
		auto end = std::chrono::steady_clock::now();

		std::cout << "Reduced Value was detected to be:    " << t.reduced_value << std::endl;
		std::cout << "(Index):                             " << t.reduced_index << std::endl;
		std::cout << "Value at index is:                   " << values[t.reduced_index] << std::endl;
		std::cout << "Kernel Duration: " << std::setw(11) << (end - begin).count() << "ns" << std::endl;
		begin = std::chrono::steady_clock::now();
		//auto value = std::accumulate(values.begin(), values.end(), type(0));
		auto it = std::max_element(values.begin(), values.end());
		auto index = std::distance(values.begin(), it);
		auto value = values[index];
		end = std::chrono::steady_clock::now();
		std::cout << "Counting manually, Reduced Value is: " << value << std::endl;
		std::cout << "(Index of):                          " << index << std::endl;
		std::cout << "Value at index is:                   " << values[index] << std::endl;
		std::cout << "Manual Duration: " << std::setw(11) << (end - begin).count() << "ns" << std::endl;
	}
	catch (cl::Error const& e) {
		std::cerr << e.what() << ':' << e.err() << std::endl;
		if (e.err() == CL_INVALID_BUFFER_SIZE)
			std::cerr << devices[0].getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << std::endl;
	}
	system("pause");
	return 0;
}