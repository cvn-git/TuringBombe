#include "bombe.h"
#include "cli_tools.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <future>

namespace {

std::string usageSyntax()
{
	return "Using: turing_bombe_all_wheels <menufile> <threads>";
}

size_t numRotorPermulations(size_t pool_size)
{
	return pool_size * (pool_size - 1) * (pool_size - 2);
}

auto allocateLoads(size_t total, size_t num_threads)
{
	const size_t div = total / num_threads;
	std::vector<std::size_t> loads(num_threads, div);
	for(size_t k = 0; k < total - (div * num_threads); ++k)
	{
		++loads[k];
	}
	return loads;
}

template <size_t NUM_ROTORS>
using ThreadWorks = std::vector<std::pair<bombe::ReflectorModel, std::array<bombe::RotorModel, NUM_ROTORS>>>;

template <size_t NUM_ROTORS>
auto generateThreadWorks(size_t num_threads)
{
	std::vector<ThreadWorks<NUM_ROTORS>> works(1);

	if constexpr(NUM_ROTORS == 3)
	{
		std::vector<bombe::ReflectorModel> reflector_pool = {bombe::ReflectorModel::REGULAR_B,
		                                                     bombe::ReflectorModel::REGULAR_C};
		std::vector<bombe::RotorModel> rotor_pool = {bombe::RotorModel::M_I,
		                                             bombe::RotorModel::M_II,
		                                             bombe::RotorModel::M_III,
		                                             bombe::RotorModel::M_IV,
		                                             bombe::RotorModel::M_V};
		const auto loads = allocateLoads(numRotorPermulations(rotor_pool.size()) * reflector_pool.size(), num_threads);
		for(const auto reflector : reflector_pool)
		{
			auto rotors = rotor_pool;
			do
			{
				if(works.back().size() >= loads[works.size() - 1])
				{
					works.emplace_back();
				}
				auto& work = works.back().emplace_back();
				work.first = reflector;
				std::copy(rotors.begin(), rotors.begin() + 3, work.second.begin());
				std::reverse(rotors.begin() + 3, rotors.end());
			} while(std::next_permutation(rotors.begin(), rotors.end()));
		}
	}
	else
	{
		std::vector<bombe::ReflectorModel> reflector_pool = {bombe::ReflectorModel::THIN_B,
		                                                     bombe::ReflectorModel::THIN_C};
		std::vector<bombe::RotorModel> thin_rotor_pool = {bombe::RotorModel::M_BETA, bombe::RotorModel::M_GAMMA};
		std::vector<bombe::RotorModel> rotor_pool = {bombe::RotorModel::M_I,
		                                             bombe::RotorModel::M_II,
		                                             bombe::RotorModel::M_III,
		                                             bombe::RotorModel::M_IV,
		                                             bombe::RotorModel::M_V,
		                                             bombe::RotorModel::M_VI,
		                                             bombe::RotorModel::M_VII,
		                                             bombe::RotorModel::M_VIII};
		const auto loads = allocateLoads(
			numRotorPermulations(rotor_pool.size()) * reflector_pool.size() * thin_rotor_pool.size(), num_threads);
		for(const auto reflector : reflector_pool)
		{
			for(const auto thin_rotor : thin_rotor_pool)
			{
				auto rotors = rotor_pool;
				do
				{
					if(works.back().size() >= loads[works.size() - 1])
					{
						works.emplace_back();
					}
					auto& work = works.back().emplace_back();
					work.first = reflector;
					work.second[0] = thin_rotor;
					std::copy(rotors.begin(), rotors.begin() + 3, work.second.begin() + 1);
					std::reverse(rotors.begin() + 3, rotors.end());
				} while(std::next_permutation(rotors.begin(), rotors.end()));
			}
		}
	}

#if 1
	std::cout << "Work allocation:\n";
	size_t total = 0;
	for(size_t k = 0; k < works.size(); ++k)
	{
		std::cout << "Thread #" << k + 1 << ": " << works[k].size() << " wheel orders\n";
		total += works[k].size();

#	if 0
		for(const auto& work : works[k])
		{
			std::cout << int(work.first);
			for(const auto rotor : work.second)
			{
				std::cout << ' ' << int(rotor);
			}
			std::cout << '\n';
		}
#	endif
	}
	std::cout << "Total: " << total << " wheel orders\n";
#endif

	return works;
}

template <size_t NUM_ROTORS>
std::vector<typename bombe::Bombe<NUM_ROTORS>::Stop>
threadProcessing(const typename bombe::Bombe<NUM_ROTORS>::Menu& menu, ThreadWorks<NUM_ROTORS>&& works)
{
	std::vector<typename bombe::Bombe<NUM_ROTORS>::Stop> all_stops;

	for(const auto& work : works)
	{
		bombe::Bombe<NUM_ROTORS> my_bombe(menu, work.first, work.second);
		const auto& stops = my_bombe.run();
		const size_t old_size = all_stops.size();
		all_stops.resize(old_size + stops.size());
		std::copy(stops.begin(), stops.end(), all_stops.begin() + old_size);
	}

	return all_stops;
}

template <size_t NUM_ROTORS>
void runAllBombes(const typename bombe::Bombe<NUM_ROTORS>::Menu& menu, size_t num_threads)
{
	using Stop = typename bombe::Bombe<NUM_ROTORS>::Stop;

	auto thread_works = generateThreadWorks<NUM_ROTORS>(num_threads);
	std::vector<std::thread> threads;
	std::vector<std::future<std::vector<Stop>>> results;
	const auto tic = std::chrono::steady_clock::now();
	for(size_t thread_idx = 0; thread_idx < num_threads; ++thread_idx)
	{
		std::packaged_task<std::vector<Stop>(const typename bombe::Bombe<NUM_ROTORS>::Menu&, ThreadWorks<NUM_ROTORS>&&)>
			task(&threadProcessing<NUM_ROTORS>);
		results.push_back(task.get_future());
		threads.emplace_back(std::move(task), std::ref(menu), std::move(thread_works[thread_idx]));
	}

	std::vector<Stop> all_stops;
	for(size_t thread_idx = 0; thread_idx < num_threads; ++thread_idx)
	{
		threads[thread_idx].join();
		const auto& stops = results[thread_idx].get();
		const size_t old_size = all_stops.size();
		all_stops.resize(old_size + stops.size());
		std::copy(stops.begin(), stops.end(), all_stops.begin() + old_size);
	}
	const auto duration =
		std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - tic).count();

	//bombe::cli::printStops(all_stops);
	std::cout << "Total " << all_stops.size() << " stops\n";
	std::cout << "All bombe runs take " << duration << " sec\n";
}

} // anonymous namespace

int main(int argc, char** argv)
{
	try
	{
		std::span<const char* const> args(argv + 1, argc - 1);
		const auto menu = bombe::cli::parseMenu(args);

		if(args.empty())
		{
			throw std::invalid_argument("Cannot parse number of threads");
		}
		const size_t num_threads = std::stoi(args[0]);

		if(std::holds_alternative<typename bombe::Bombe<3>::Menu>(menu))
		{
			runAllBombes<3>(std::get<bombe::Bombe<3>::Menu>(menu), num_threads);
		}
		else
		{
			runAllBombes<4>(std::get<bombe::Bombe<4>::Menu>(menu), num_threads);
		}

		return 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << "EXCEPTION: " << e.what() << '\n';
		std::cout << usageSyntax() << "\n";
		return -1;
	}
}
