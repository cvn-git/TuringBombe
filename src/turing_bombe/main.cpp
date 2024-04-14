#include "bombe.h"
#include "cli_tools.h"

#include <chrono>

namespace {

std::string usageSyntax()
{
	return "Using: turing_bombe <menufile> <UKW> <R1> <R2> <R3> [R4]";
}

template <size_t NUM_ROTORS>
void runBombe(const typename bombe::Bombe<NUM_ROTORS>::Menu& menu,
              bombe::ReflectorModel reflector_model,
              std::span<const bombe::RotorModel, NUM_ROTORS> rotor_models)
{
	bombe::Bombe<NUM_ROTORS> my_bombe(menu, reflector_model, rotor_models);

	const auto tic = std::chrono::steady_clock::now();
	const auto& stops = my_bombe.run();
	const auto duration =
		std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - tic).count();

	std::cout << "Bombe run takes " << duration << " sec\n";

	bombe::cli::printStops<NUM_ROTORS>(stops);
}

} // anonymous namespace

int main(int argc, char** argv)
{
	try
	{
		std::span<const char* const> args(argv + 1, argc - 1);
		const auto menu = bombe::cli::parseMenu(args);
		const size_t num_rotors = std::holds_alternative<typename bombe::Bombe<3>::Menu>(menu) ? 3 : 4;
		const auto reflector_model = bombe::cli::parseReflectorModel(args, num_rotors);
		const auto rotor_models = bombe::cli::parseRotorModels(args, num_rotors);

		if(num_rotors == 3)
		{
			runBombe<3>(std::get<bombe::Bombe<3>::Menu>(menu),
			            reflector_model,
			            std::span<const bombe::RotorModel, 3>(rotor_models.data(), 3));
		}
		else
		{
			runBombe<4>(std::get<bombe::Bombe<4>::Menu>(menu),
			            reflector_model,
			            std::span<const bombe::RotorModel, 4>(rotor_models.data(), 4));
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
