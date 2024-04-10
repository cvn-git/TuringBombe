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

	struct Stop
	{
		ReflectorModel reflector_model;
		std::vector<RotorModel> rotor_models;
		std::vector<Letter> rotor_positions;
		std::pair<Letter, Letter> stecker;
	};

public:
	Bombe(const Menu& menu, ReflectorModel reflector_model, std::span<const RotorModel> rotor_models);

	const std::vector<Stop>& run();

	static Menu loadMenu(std::span<const std::string> lines);

private:
	static constexpr size_t MAX_SCRAMBLERS_PER_LETTER = 6;

	struct WireGroup
	{
		std::array<const DoubleMap*, MAX_SCRAMBLERS_PER_LETTER> scramblers{nullptr};
		std::array<Letter, MAX_SCRAMBLERS_PER_LETTER> scrambler_ends{0};
		std::bitset<NUM_LETTERS> wires;
		uint8_t num_scrambler{0};
	};
	static_assert(sizeof(WireGroup) == 64, "Group object does not fit a cache line");

	void resetWires();

	void connectScrambler(const Scrambler& scrambler, Letter from, Letter to);

	void addResult(const Scrambler& first_scrambler, Letter reg_letter, size_t num_on);

private:
	std::array<WireGroup, NUM_LETTERS> wire_groups_;
	const Menu& menu_;
	const ReflectorModel reflector_model_;
	const std::vector<RotorModel> rotor_models_;
	std::vector<std::unique_ptr<Scrambler>> scramblers_;
	std::queue<std::pair<Letter, Letter>> bft_queue_;
	std::vector<Stop> stops_;
};

} // namespace bombe

#endif // BOMBE_BOMBE_H
