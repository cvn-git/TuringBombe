#ifndef BOMBE_BOMBE_H
#define BOMBE_BOMBE_H

#include "scrambler.h"

#include <queue>

namespace bombe {

class Bombe
{
public:
	struct MenuEdge
	{
		std::vector<Letter> rotor_positions;
		std::pair<Letter, Letter> nodes;
	};

	struct Menu
	{
		std::vector<MenuEdge> edges;
		std::vector<std::pair<Letter, Letter>> registers;
	};

public:
	Bombe(const Menu& menu, ReflectorModel reflector_model, std::span<const RotorModel> rotor_models);

	void run();

	static Menu loadMenu(std::span<const std::string> lines);

private:
	static constexpr size_t MAX_CONNECTIONS = 7;

	struct Wire
	{
		std::array<Wire*, MAX_CONNECTIONS> connections;
		uint8_t num_connections;
		bool voltage_applied;
	};
	static_assert(sizeof(Wire) == 64, "Wire object does not fit a cache line");

	using WireGroup = std::array<Wire, NUM_LETTERS>;

	void resetWires();

	static void connectWire(Wire& from_wire, Wire& to_wire);

	void addResult(Letter reg_letter, size_t num_on);

private:
	std::array<WireGroup, NUM_LETTERS> wire_groups_;
	const Menu& menu_;
	std::vector<std::unique_ptr<Scrambler>> scramblers_;
	std::queue<Wire*> bft_queue_;
};

} // namespace bombe

#endif // BOMBE_BOMBE_H
