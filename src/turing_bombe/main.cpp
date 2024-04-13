#include "bombe.h"
#include "cli_tools.h"

#include <chrono>

namespace {

std::string usageSyntax()
{
	return "Using: turing_bombe <menufile> <UKW> <R1> <R2> <R3> [R4]";
}

} // anonymous namespace

int main(int argc, char** argv)
{
	try
	{
		std::span<const char* const> args(argv + 1, argc - 1);
		const auto menu = bombe::cli::parseMenu(args);
		const auto num_rotors = menu.numRotors();
		const auto reflector_model = bombe::cli::parseReflectorModel(args, num_rotors);
		const auto rotor_models = bombe::cli::parseRotorModels(args, num_rotors);

		bombe::Bombe my_bombe(menu, reflector_model, rotor_models);

		const auto tic = std::chrono::steady_clock::now();
		const auto& stops = my_bombe.run();
		const auto duration =
			std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - tic).count();

		std::cout << "Bombe run takes " << duration << " sec\n";

		bombe::cli::printStops(stops);

		return 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << "EXCEPTION: " << e.what() << '\n';
		std::cout << usageSyntax() << "\n";
		return -1;
	}
}
