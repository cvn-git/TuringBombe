#include "bombe.h"

#include <iostream>

namespace bombe {

Bombe::Menu Bombe::loadMenu(std::span<const std::string> lines)
{
	Menu menu;

	if(lines.empty())
	{
		throw std::invalid_argument("Empty bombe menu");
	}
	const size_t num_rotors = lines[0].size() - 2;
	if((num_rotors != 3) && (num_rotors != 4))
	{
		throw std::invalid_argument("Invalid bombe menu");
	}

	std::vector<Letter> letters(num_rotors + 2);
	for(const auto& line : lines)
	{
		if(line.size() != (num_rotors + 2))
		{
			break;
		}

		if(line[0] == '=')
		{
			menu.registers.emplace_back(char2Letter(line[1]), char2Letter(line[3]));
			break;
		}
		else
		{
			char2Letter(line, letters);
			menu.edges.emplace_back();
			auto& edge = menu.edges.back();
			edge.nodes = {letters[num_rotors], letters[num_rotors + 1]};
			edge.rotor_positions = {letters.begin(), letters.begin() + num_rotors};
		}
	}

	if(menu.registers.empty())
	{
		throw std::invalid_argument("Bombe menu does not have any register");
	}

	return menu;
}

Bombe::Bombe(const Menu& menu, ReflectorModel reflector_model, std::span<const RotorModel> rotor_models)
	: menu_{menu}
	, reflector_model_{reflector_model}
	, rotor_models_{rotor_models.begin(), rotor_models.end()}
{
	// Create scramblers
	const size_t num_edges = menu.edges.size();
	const size_t num_rotors = rotor_models.size();
	scramblers_.reserve(num_edges);
	scrambler_maps_.resize(num_edges);
	for(size_t edge_idx = 0; edge_idx < num_edges; ++edge_idx)
	{
		const auto& edge = menu.edges[edge_idx];
		if(edge.rotor_positions.size() != num_rotors)
		{
			throw std::invalid_argument("Invalid bombe menu");
		}

		scramblers_.emplace_back(reflector_model, rotor_models);
		for(size_t k = 0; k < num_rotors; ++k)
		{
			scramblers_.back().setRotorPosition(k, edge.rotor_positions[k]);
		}

		scrambler_maps_[edge_idx].nodes = edge.nodes;
	}
}

const std::vector<Bombe::Stop>& Bombe::run()
{
	const size_t num_edges = scramblers_.size();
	const size_t num_rotors = scramblers_[0].numRotors();
	std::vector<Letter> rotor_offsets(num_rotors, 0);
	const DoubleMap& null_map = nullDoubleMap();

	stops_.clear();
	for(bool terminated = false; !terminated;)
	{
		// Reset wires
		for(auto& group : wire_groups_)
		{
			group.reset();
		}

		for(size_t k = 0; k < num_edges; ++k)
		{
			const auto& from = scramblers_[k].map();
			std::copy(from.begin(), from.begin() + NUM_LETTERS, scrambler_maps_[k].map.begin());
		}

		// Apply voltage to registers
		for(const auto& reg : menu_.registers)
		{
			wire_groups_[reg.first].set(reg.second);
			wire_groups_[reg.second].set(reg.first); // via diagonal board
		}

		// Propagate voltage
		bool changed = true;
		size_t num_loops = 0;
		while(changed)
		{
			++num_loops;
			changed = false;
			for(const auto& scrambler_map : scrambler_maps_)
			{
				const auto [group_idx1, group_idx2] = scrambler_map.nodes;
				auto& group1 = wire_groups_[group_idx1];
				auto& group2 = wire_groups_[group_idx2];
				for(Letter wire1 = 0; wire1 < NUM_LETTERS; ++wire1)
				{
					const Letter wire2 = scrambler_map.map[wire1];
					if(group1[wire1] != group2[wire2])
					{
						group1.set(wire1);
						group2.set(wire2);
						wire_groups_[wire1].set(group_idx1); // via diagonal board
						wire_groups_[wire2].set(group_idx2); // via diagonal board
						changed = true;
					}
				}
			}
		}
		max_loops_ = std::max(max_loops_, num_loops);
		total_loops_ += num_loops;
		++num_orders_;

		// Check register
		const Letter reg_letter = menu_.registers[0].first;
		const size_t num_on = wire_groups_[reg_letter].count();
		if((num_on == 1) || (num_on == (NUM_LETTERS - 1)))
		{
			addResult(scramblers_[0], reg_letter, num_on);
		}

		// Step rotors
		size_t rotor_idx = num_rotors - 1;
		for(;; --rotor_idx)
		{
			if(++rotor_offsets[rotor_idx] >= NUM_LETTERS)
			{
				rotor_offsets[rotor_idx] = 0;
				if(rotor_idx == 0)
				{
					terminated = true;
					break;
				}
			}
			else
			{
				break;
			}
		}
		for(size_t edge_idx = 0; edge_idx < num_edges; ++edge_idx)
		{
			const auto& edge = menu_.edges[edge_idx];
			auto& scrambler = scramblers_[edge_idx];
			for(size_t k = rotor_idx; k < num_rotors; ++k)
			{
				scrambler.setRotorPosition(k, null_map[edge.rotor_positions[k] + rotor_offsets[k]]);
			}
		}
	}

	return stops_;
}

void Bombe::addResult(const Scrambler& first_scrambler, Letter reg_letter, size_t num_on)
{
	stops_.emplace_back();
	auto& stop = stops_.back();

	stop.reflector_model = reflector_model_;
	stop.rotor_models = rotor_models_;

	const auto num_rotors = first_scrambler.numRotors();
	stop.rotor_positions.resize(num_rotors);
	for(size_t k = 0; k < num_rotors; ++k)
	{
		stop.rotor_positions[k] = first_scrambler.rotor(k).position();
	}

	stop.stecker.first = reg_letter;
	const bool voltaged = (num_on == 1);
	const auto& wires = wire_groups_[reg_letter];
	for(Letter k = 0; k < NUM_LETTERS; ++k)
	{
		if(wires[k] == voltaged)
		{
			stop.stecker.second = k;
			break;
		}
	}
}

void Bombe::printLoopStatistics() const
{
	std::cout << num_orders_ << " rotor orders\n";
	std::cout << "Maximum: " << max_loops_ << " loops per rotor order\n";
	std::cout << "Average: " << double(total_loops_) / num_orders_ << " loops per rotor order\n";
}

} // namespace bombe
