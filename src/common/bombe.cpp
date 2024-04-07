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

		scramblers_.push_back(std::move(scrambler));
	}

	// Connect diagonal board
	for(Letter group_idx = 0; group_idx < NUM_LETTERS; ++group_idx)
	{
		for(Letter wire_idx = 0; wire_idx < NUM_LETTERS; ++wire_idx)
		{
			wire_groups_[group_idx][wire_idx].connections[0] = &wire_groups_[wire_idx][group_idx];
		}
	}
}

void Bombe::run()
{
	const size_t num_edges = scramblers_.size();
	const size_t num_rotors = scramblers_[0]->numRotors();
	std::vector<Letter> rotor_offsets(num_rotors, 0);
	const DoubleMap& null_map = nullDoubleMap();

	for(bool terminated = false; !terminated;)
	{
		resetWires();

		// Connect scramblers
		for(size_t edge_idx = 0; edge_idx < num_edges; ++edge_idx)
		{
			const auto& edge = menu_.edges[edge_idx];
			auto& group1 = wire_groups_[edge.nodes.first];
			auto& group2 = wire_groups_[edge.nodes.second];
			const auto& scrambler_map = scramblers_[edge_idx]->map();
			for(Letter letter1 = 0; letter1 < NUM_LETTERS; ++letter1)
			{
				auto& wire1 = group1[letter1];
				auto& wire2 = group2[scrambler_map[letter1]];
				connectWire(wire1, wire2);
				connectWire(wire2, wire1);
			}
		}

		// Apply voltage to registers
		for(const auto& reg : menu_.registers)
		{
			auto& wire = wire_groups_[reg.first][reg.second];
			wire.voltage_applied = true;
			bft_queue_.push(&wire);
		}

		// Propagate voltage (breadth-first traversal)
		while(!bft_queue_.empty())
		{
			auto& wire = *bft_queue_.front();
			bft_queue_.pop();
			for(decltype(wire.num_connections) k = 0; k < wire.num_connections; ++k)
			{
				auto* const other = wire.connections[k];
				if(!other->voltage_applied)
				{
					other->voltage_applied = true;
					bft_queue_.push(other);
				}
			}
		}

		// Check register
		size_t num_on = 0;
		const Letter reg_letter = menu_.registers[0].first;
		for(const auto& wire : wire_groups_[reg_letter])
		{
			if(wire.voltage_applied)
				++num_on;
		}
		if((num_on == 1) || (num_on == (NUM_LETTERS - 1)))
		{
			addResult(reg_letter, num_on);
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
}

void Bombe::resetWires()
{
	for(auto& group : wire_groups_)
	{
		for(auto& wire : group)
		{
			wire.num_connections = 1; // Keep just the diagonal board
			wire.voltage_applied = false;
		}
	}

	if(!bft_queue_.empty())
	{
		throw std::runtime_error("BFT queue not empty");
	}
}

void Bombe::connectWire(Wire& from_wire, Wire& to_wire)
{
	if(from_wire.num_connections >= MAX_CONNECTIONS)
	{
		throw std::runtime_error("Too many connections per wire");
	}
	from_wire.connections[from_wire.num_connections++] = &to_wire;
}

} // namespace bombe
