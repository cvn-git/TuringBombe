#include "enigma.h"

#include <iostream>

std::string usageSyntax()
{
	return "Usage: enigma_app <numrotors> <UKW> <R1> <R2> <R3> [R4] [steckers] <ring> <grun>";
}

size_t parseNumRotors(std::span<const char* const>& args)
{
	if(args.empty())
	{
		throw std::invalid_argument("Cannot parse numrotors\n" + usageSyntax());
	}
	const size_t num_rotors = std::stoi(args[0]);
	args = args.subspan(1);
	return num_rotors;
}

bombe::ReflectorModel parseReflectorModel(std::span<const char* const>& args, size_t num_rotors)
{
	if(args.empty())
	{
		throw std::invalid_argument("Cannot parse reflector model\n" + usageSyntax());
	}
	const size_t model_number = std::stoi(args[0]);
	args = args.subspan(1);
	return bombe::getReflectorModel(num_rotors == 4, model_number);
}

std::vector<bombe::RotorModel> parseRotorModels(std::span<const char* const>& args, size_t num_rotors)
{
	if(args.size() < num_rotors)
	{
		throw std::invalid_argument("Cannot parse rotor models\n" + usageSyntax());
	}
	std::vector<bombe::RotorModel> models(num_rotors);
	for(size_t k = 0; k < num_rotors; ++k)
	{
		const bool is_thin = (num_rotors == 4) && (k == 0);
		const size_t model_number = std::stoi(args[k]);
		models[k] = bombe::getRotorModel(is_thin, model_number);
	}
	args = args.subspan(num_rotors);
	return models;
}

int main(int argc, char** argv)
{
	try
	{
		std::span<const char* const> args(argv + 1, argc - 1);
		const auto num_rotors = parseNumRotors(args);
		const auto reflector_model = parseReflectorModel(args, num_rotors);
		const auto rotor_models = parseRotorModels(args, num_rotors);

		bombe::Enigma enigma(reflector_model, rotor_models);

		if(args.size() >= 3)
		{
			enigma.configureSteckers(args[0]);
			args = args.subspan(1);
		}

		if(args.size() < 2)
		{
			throw std::invalid_argument("Cannot parse ringstellung/grundstellung\n" + usageSyntax());
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
		return -1;
	}
}
