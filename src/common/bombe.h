#ifndef BOMBE_BOMBE_H
#define BOMBE_BOMBE_H

#include "scrambler.h"

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

	void printLoopStatistics() const;

	static Menu loadMenu(std::span<const std::string> lines);

private:
	struct ScramblerMap
	{
		SingleMap map;
		std::pair<Letter, Letter> nodes;
		uint32_t padding;
	};
	static_assert(sizeof(ScramblerMap) == 32, "Invalid ScramblerMap size");

	void addResult(const Scrambler& first_scrambler, Letter reg_letter, size_t num_on);

private:
	std::array<std::bitset<NUM_LETTERS>, NUM_LETTERS> wire_groups_;
	const Menu& menu_;
	const ReflectorModel reflector_model_;
	const std::vector<RotorModel> rotor_models_;
	std::vector<Scrambler> scramblers_;
	std::vector<ScramblerMap> scrambler_maps_;
	std::vector<Stop> stops_;

	// Loop statistics
	size_t total_loops_{0};
	size_t max_loops_{0};
	size_t num_orders_{0};
};

} // namespace bombe

#endif // BOMBE_BOMBE_H
