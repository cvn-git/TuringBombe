#include "bombe.h"

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
	const size_t num_rotors = rotor_models.size();
	for(const auto& edge : menu.edges)
	{
		auto scrambler = std::make_unique<Scrambler>(reflector_model, rotor_models);
		if(edge.rotor_positions.size() != num_rotors)
		{
			throw std::invalid_argument("Invalid bombe menu");
		}
		for(size_t k = 0; k < num_rotors; ++k)
		{
			scrambler->setRotorPosition(k, edge.rotor_positions[k]);
		}

		connectScrambler(*scrambler, edge.nodes.first, edge.nodes.second);
		connectScrambler(*scrambler, edge.nodes.second, edge.nodes.first);

		scramblers_.push_back(std::move(scrambler));
	}
}

const std::vector<Bombe::Stop>& Bombe::run()
{
	const size_t num_edges = scramblers_.size();
	const size_t num_rotors = scramblers_[0]->numRotors();
	std::vector<Letter> rotor_offsets(num_rotors, 0);
	const DoubleMap& null_map = nullDoubleMap();

	stops_.clear();
	for(bool terminated = false; !terminated;)
	{
		resetWires();

		// Apply voltage to registers
		for(const auto& reg : menu_.registers)
		{
			wire_groups_[reg.first].wires.set(reg.second);
			bft_queue_.emplace(reg.first, reg.second);
		}

		// Propagate voltage (breadth-first traversal)
		while(!bft_queue_.empty())
		{
			const auto [group_idx, wire_idx] = bft_queue_.front();
			bft_queue_.pop();

			// Via diagonal board
			if(auto& wires = wire_groups_[wire_idx].wires; !wires[group_idx])
			{
				wires.set(group_idx);
				bft_queue_.emplace(wire_idx, group_idx);
			}

			// Via scramblers
			const auto& group = wire_groups_[group_idx];
			for(decltype(group.num_scrambler) k = 0; k < group.num_scrambler; ++k)
			{
				const Letter other_group_idx = group.scrambler_ends[k];
				const Letter other_wire_idx = (*group.scramblers[k])[wire_idx];
				auto& other_wires = wire_groups_[other_group_idx].wires;
				if(!other_wires[other_wire_idx])
				{
					other_wires.set(other_wire_idx);
					bft_queue_.emplace(other_group_idx, other_wire_idx);
				}
			}
		}

		// Check register
		const Letter reg_letter = menu_.registers[0].first;
		const size_t num_on = wire_groups_[reg_letter].wires.count();
		if((num_on == 1) || (num_on == (NUM_LETTERS - 1)))
		{
			addResult(*scramblers_[0], reg_letter, num_on);
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
			auto& scrambler = *scramblers_[edge_idx];
			for(size_t k = rotor_idx; k < num_rotors; ++k)
			{
				scrambler.setRotorPosition(k, null_map[edge.rotor_positions[k] + rotor_offsets[k]]);
			}
		}
	}

	return stops_;
}

void Bombe::resetWires()
{
	for(auto& group : wire_groups_)
	{
		group.wires.reset();
	}

	if(!bft_queue_.empty())
	{
		throw std::runtime_error("BFT queue not empty");
	}
}

void Bombe::connectScrambler(const Scrambler& scrambler, Letter from, Letter to)
{
	auto& group = wire_groups_[from];
	if(group.num_scrambler >= group.scramblers.size())
	{
		throw std::invalid_argument("Too many scramblers per letter");
	}
	group.scramblers[group.num_scrambler] = &scrambler.map();
	group.scrambler_ends[group.num_scrambler] = to;
	group.num_scrambler += 1;
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
	const auto& wires = wire_groups_[reg_letter].wires;
	for(Letter k = 0; k < NUM_LETTERS; ++k)
	{
		if(wires[k] == voltaged)
		{
			stop.stecker.second = k;
			break;
		}
	}
}

} // namespace bombe
