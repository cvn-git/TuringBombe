#include "cli_tools.h"
#include "enigma.h"

namespace {

std::string usageSyntax()
{
	return "Usage: enigma_app <numrotors> <UKW> <R1> <R2> <R3> [R4] [steckers] <ring> <grun>";
}

size_t parseNumRotors(std::span<const char* const>& args)
{
	if(args.empty())
	{
		throw std::invalid_argument("Cannot parse numrotors\n");
	}
	const size_t num_rotors = std::stoi(args[0]);
	args = args.subspan(1);
	return num_rotors;
}

} // anonymous namespace

int main(int argc, char** argv)
{
	try
	{
		std::span<const char* const> args(argv + 1, argc - 1);
		const auto num_rotors = parseNumRotors(args);
		const auto reflector_model = bombe::cli::parseReflectorModel(args, num_rotors);
		const auto rotor_models = bombe::cli::parseRotorModels(args, num_rotors);

		bombe::Enigma enigma(reflector_model, rotor_models);

		if(args.size() >= 3)
		{
			enigma.configureSteckers(args[0]);
			args = args.subspan(1);
		}

		if(args.size() < 2)
		{
			throw std::invalid_argument("Cannot parse ringstellung/grundstellung\n");
		}
		enigma.configureRotors(args[0], args[1]);

		std::string output;
		std::string input;
		while(std::getline(std::cin, input))
		{
			// Remove non-alphabet characters
			size_t num = 0;
			for(const char ch : input)
			{
				if(ch != ' ')
				{
					input[num++] = ch;
				}
			}
			input.resize(num);

			const size_t cur_size = output.size();
			output.resize(cur_size + input.size());
			enigma.process(input, {output.data() + cur_size, input.size()});
		}

		std::cout << output;
		return 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << "EXCEPTION: " << e.what() << '\n';
		std::cout << usageSyntax() << "\n";
		return -1;
	}
}
