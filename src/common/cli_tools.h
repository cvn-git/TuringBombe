#ifndef BOMBE_CLI_TOOLS_H
#define BOMBE_CLI_TOOLS_H

#include "bombe.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace bombe::cli {

ReflectorModel parseReflectorModel(std::span<const char* const>& args, size_t num_rotors)
{
	if(args.empty())
	{
		throw std::invalid_argument("Cannot parse reflector model\n");
	}
	const size_t model_number = std::stoi(args[0]);
	args = args.subspan(1);
	return getReflectorModel(num_rotors == 4, model_number);
}

std::vector<bombe::RotorModel> parseRotorModels(std::span<const char* const>& args, size_t num_rotors)
{
	if(args.size() < num_rotors)
	{
		throw std::invalid_argument("Cannot parse rotor models\n");
	}
	std::vector<bombe::RotorModel> models(num_rotors);
	for(size_t k = 0; k < num_rotors; ++k)
	{
		const bool is_thin = (num_rotors == 4) && (k == 0);
		const size_t model_number = std::stoi(args[k]);
		models[k] = getRotorModel(is_thin, model_number);
	}
	args = args.subspan(num_rotors);
	return models;
}

void printStops(std::span<const bombe::Bombe::Stop> stops)
{
	for(const auto& stop : stops)
	{
		std::ostringstream ss;
		ss << int(stop.reflector_model);
		for(const auto rotor_model : stop.rotor_models)
		{
			ss << ' ' << int(rotor_model);
		}

		std::string pos_letters(stop.rotor_positions.size(), ' ');
		letter2Char(stop.rotor_positions, pos_letters);
		ss << "    " << pos_letters;
		ss << " " << letter2Char(stop.stecker.first) << ':' << letter2Char(stop.stecker.second);

		std::cout << ss.str() << "\n";
	}
}

bombe::Bombe::Menu parseMenu(std::span<const char* const>& args)
{
	if(args.empty())
	{
		throw std::invalid_argument("Cannot parse menu\n");
	}

	std::vector<std::string> lines;
	const std::string filename(args[0]);
	std::ifstream ifs(filename);
	if(!ifs.is_open())
	{
		throw std::invalid_argument("Cannot open the menu file " + filename);
	}
	std::string line;
	while(std::getline(ifs, line, '\n'))
	{
		lines.push_back(line);
		//std::cout << std::format("line({}) size({})\n", line, line.size());
	}

	args = args.subspan(1);
	return bombe::Bombe::loadMenu(lines);
}

} // namespace bombe::cli

#endif // BOMBE_CLI_TOOLS_H
