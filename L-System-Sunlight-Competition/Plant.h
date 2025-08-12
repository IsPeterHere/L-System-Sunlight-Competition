#pragma once
#include <iostream>
#include<random>
#include<vector>
#include<algorithm>
#include<cmath>
#include"..\Interface\MYR.h"
#include "ComM.h"

const int MAX_ENERGY_DEBT{ -10 };
const int COST_OF_STICK{ 1 };
const int COST_OF_LEAF{ 1 };
const int COST_OF_SEED_BALLOT{ 1 };

const int NO_OF_WINNERS{ 1 };
const int NO_OF_LOSERS{ 1 };
const int NO_OF_RANDOM_NEW{ 1 };

const float ROTATION_RADS{ 0.2f };
const int STICK_LENGTH{ 10 };

const int GROUND_HEIGHT{ 50 };
constexpr glm::vec3 RGB(int r, int g, int b) { return { r / 255.0, g / 255.0, b / 255.0 }; }
const glm::vec3 GROUND_COLOUR{ RGB(139,69,4)};
const glm::vec3 SKY_COLOUR{ RGB(0,20,150) };
const glm::vec3 STICK_COLOUR{ 0.95,0.0,0.0 };
const glm::vec3 LEAF_COLOUR{ 0.0,0.95,0.0 };

class Display
{
public:
	Display(std::vector<MYR::Vertex>* vertices, std::vector<uint32_t>* indices) : vertices(vertices), indices(indices){}

	void set_pen(double width, glm::vec3 colour, double depth)
	{
		pen_colour = colour;
		pen_width = width;

		assert(depth >= 0 && depth <= 1000);
		pen_depth = depth/(double)1000;
	}

	void draw_line(int x1, int y1, int x2, int y2)
	{

		//std::cout << x1 << "," << y1 << "," << x2 << "," << y2 << "\n";
		//std::cout << pen_depth << "\n";
		vertices->push_back({ {get_corner(x1,y1,x2,y2,false) / (double)screen_width - 0.5, get_corner(y1,x1,y2,x2,true) / (double)screen_height - 0.5, pen_depth},pen_colour });
		vertices->push_back({ {get_corner(x1,y1,x2,y2,true) / (double)screen_width - 0.5, get_corner(y1,x1,y2,x2,false) / (double)screen_height - 0.5, pen_depth},pen_colour });
		vertices->push_back({ {get_corner(x2,y2,x1,y1,false) / (double)screen_width - 0.5, get_corner(y2,x2,y1,x1,true) / (double)screen_height - 0.5, pen_depth},pen_colour });
		vertices->push_back({ {get_corner(x2,y2,x1,y1,true) / (double)screen_width - 0.5, get_corner(y2,x2,y1,x1,false) / (double)screen_height - 0.5, pen_depth},pen_colour });

		indices->push_back(vertices->size()-4);
		indices->push_back(vertices->size()-3);
		indices->push_back(vertices->size()-2);

		indices->push_back(vertices->size()-2);
		indices->push_back(vertices->size()-1);
		indices->push_back(vertices->size()-4);

		indices->push_back(vertices->size() - 2);
		indices->push_back(vertices->size() - 3);
		indices->push_back(vertices->size() - 4);

		indices->push_back(vertices->size() - 4);
		indices->push_back(vertices->size() - 1);
		indices->push_back(vertices->size() - 2);
	}

	void update_screen_dimensions(int width, int height) { this->screen_width = width; this->screen_height = height; }
	int get_screen_width() { return screen_width; }
	int get_screen_height() { return screen_height; }
	void clear_screen() { vertices->clear(); indices->clear(); }

private:
	std::vector<MYR::Vertex>* vertices;
	std::vector<uint32_t>* indices;

	int screen_width{};
	int screen_height{};

	double pen_width{};
	glm::vec3 pen_colour{};
	double pen_depth{};

	double get_corner(int x1, int y1, int x2, int y2, bool pos) const
	{
		if (pos)
			return (x1 + ((pen_width/2) * ((y2 - y1) / (double)std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)))));
		else
			return (x1 - ((pen_width/2) * ((y2 - y1) / (double)std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)))));
	}
};

enum class Cmd_lang
{
	seed,//starting_state

	leaf,//Command Words
	stick,
	left,
	right,
	next,
	last,
	enter_seed_ballot,


	a,//Working letters
	b,
	c,
	d,
	e,
	f,

	NUMBER_OF_UNIQUE_COMMANDS
};

class Species
{
public:
	Species() {}
	
	Species* Mutation(int seed)
	{
		Species* mutated = new Species();
		L_Systems::Rules* new_DNA = mutated->get_DNA();

		generator.seed(seed);
		std::uniform_int_distribution<int> cmd_distribution(0, static_cast<int>(Cmd_lang::NUMBER_OF_UNIQUE_COMMANDS)-1);
		std::bernoulli_distribution bool_distribution{};

		Cmd_lang from{ cmd_distribution(generator)};
		bool add{ bool_distribution(generator) };
		
		for (int i{ 0 }; static_cast<Cmd_lang>(i) < Cmd_lang::NUMBER_OF_UNIQUE_COMMANDS; i++)
		{
			Cmd_lang cmd{ static_cast<Cmd_lang>(i) };
			std::vector< L_Systems::alphabet_T> to{ *DNA[static_cast<L_Systems::alphabet_T>(cmd)] };

			if (cmd == from)
			{
				std::vector<L_Systems::alphabet_T>::iterator index = to.begin();
				if (to.size() > 0)
				{
					std::uniform_int_distribution<int> where(0, to.size() - 1);
					index += where(generator);
				}
			


				if (add)
				{
					Cmd_lang new_item{ cmd_distribution(generator) };
					to.insert(index, static_cast<L_Systems::alphabet_T>(new_item));
				}
				else
				{
					if (to.size() > 0)
						to.erase(index);
				}
			}
			new_DNA->add(static_cast<L_Systems::alphabet_T>(cmd), to);
		}
		return mutated;
	}

	L_Systems::Rules* get_DNA() { return &DNA; }
	int seed_ballot_points{ 0 };
private:
	std::default_random_engine generator;

	L_Systems::Rules DNA{ static_cast<L_Systems::alphabet_T>(Cmd_lang::NUMBER_OF_UNIQUE_COMMANDS) };

};

class Plant
{
	struct Cord
	{
		float x;
		float y;
	};

	struct stick
	{
		Cord from;
		float pointing;
	};

public:
	Plant(Species* species, float x_planted_position, float y_planted_position) : species(species), x_planted_position(x_planted_position)
	{
		stick s{ {x_planted_position,y_planted_position},0 };
		plant_structure.push_back(s);
		at = { plant_structure[index].from };
		pointing = { plant_structure[index].pointing };
	}

	void grow(Display* display)
	{
		if (energy > MAX_ENERGY_DEBT && command_state.size() < 1000)
		{
			L_Systems::L_System system{ species->get_DNA(),&command_state };
			system.run(1);

			Cord new_position{};
			for (L_Systems::alphabet_T Cmd : command_state)
			{
				switch (static_cast<Cmd_lang>(Cmd))
				{
				case Cmd_lang::leaf:
					energy -= COST_OF_LEAF;

					display->set_pen(3, LEAF_COLOUR, 3);
					display->draw_line(at.x - 1, at.y, at.x + 1, at.y + 1);
					leafs.push_back(at);
					break;
				case Cmd_lang::stick:
					energy -= COST_OF_STICK;

					new_position.x = at.x + STICK_LENGTH * std::sin(pointing);
					new_position.y = at.y + STICK_LENGTH * std::cos(pointing);
					display->set_pen(3, STICK_COLOUR, 2);
					display->draw_line(at.x, at.y, new_position.x, new_position.y);
					plant_structure.push_back({ new_position,pointing });
					at = new_position;
					index += 1;
					break;
				case Cmd_lang::left:
					pointing += ROTATION_RADS;
					break;
				case Cmd_lang::right:
					pointing -= ROTATION_RADS;
					break;
				case Cmd_lang::next:
					if (index + 1 < plant_structure.size())
						index += 1;
					at = plant_structure[index].from;
					pointing = plant_structure[index].pointing;
					break;
				case Cmd_lang::last:
					if (index >= 1)
						index -= 1;
					at = plant_structure[index].from;
					pointing = plant_structure[index].pointing;
					break;
				case Cmd_lang::enter_seed_ballot:
					if (energy >= COST_OF_SEED_BALLOT)
					{
						energy -= COST_OF_SEED_BALLOT;
						species->seed_ballot_points += 1;
					}
					break;
				default:
					break;
				}
			}
		}	
	}

	std::vector <Cord> leafs{};
	std::vector <stick> plant_structure{};

	size_t index{ 0 };
	Cord at;
	float pointing;

	int energy{ 0 };
private:
	Species* species;
	int x_planted_position;

	std::vector<L_Systems::alphabet_T> command_state{ static_cast<L_Systems::alphabet_T>(Cmd_lang::seed)};

};

class World
{
public:
	World(Display* display) : display(display) {}

	void start()
	{
		starting_species();
		plant_seeds();
	}
	void day()
	{
		std::random_device rnd;
		generator.seed(rnd());

		display->clear_screen();
		draw_background();
		draw_ground();
		std::cout << "--New Day-- " << "\n";
		for (int segment{ 0 }; segment < day_length; segment++)
		{
			growth();
			sunlight();
		}
		std::cout << "Number of Species: " << species.size() << "\n";
		for (Plant* plant : plants) delete plant;
		plants.clear();

		evalate_winners();
		std::cout << "Number of Species after Evaluation: " << species.size() << "\n";
		new_mutations();
		plant_seeds();
	}

	void end()
	{
		for (Plant* plant : plants) delete plant;
		plants.clear();
		for (Species* specie : species) delete specie;
		species.clear();
	}

private:
	Display* display;
	std::vector<Species*> species{};
	std::vector<Plant*> plants{};

	const int day_length{ 10 };

	std::default_random_engine generator;

	void growth()
	{
		for (Plant* plant : plants) plant->grow(display);
	}
	void sunlight()
	{
		struct HighScore 
		{
			Plant* plant{ NULL };
			int height{ GROUND_HEIGHT };
		};
		std::vector<HighScore> sunlight_lines{};
		sunlight_lines.resize(display->get_screen_width());

		for (Plant* plant : plants)
		{
			for (auto& leaf : plant->leafs)
			{
				if (leaf.x >= 0 && leaf.x < display->get_screen_width())
				{
					if (leaf.y > sunlight_lines[leaf.x].height)
					{
						sunlight_lines[leaf.x].plant = plant;
						sunlight_lines[leaf.x].height = leaf.y;
					}
				}
			}
		}

		for (HighScore winner : sunlight_lines)
		{
			if (winner.plant != NULL)
				winner.plant->energy += 1;
		}

	}

	void evalate_winners()
	{
		std::sort(species.begin(), species.end(), [](Species* s1, Species* s2) {return s1->seed_ballot_points > s2->seed_ballot_points; });

		for (size_t index{ species.size()-1}; index > 0; index--)
		{
			if (species[index]->seed_ballot_points > 0)
				break;

			delete species[index];
			species.erase(species.begin() + index);
		}

		if (species.size() > NO_OF_LOSERS)
		{
			for (int i{ 0 }; i < NO_OF_LOSERS; i++)
			{
				delete species[species.size()-1];
				species.erase(species.end()-1);
			}
		}

		for (Species* specie : species) specie->seed_ballot_points = 0;
	}
	void new_mutations()
	{
		//winner gets one
		for (int i{0}; i< NO_OF_WINNERS;i++)
			species.push_back(species[0]->Mutation(generator()));

		//random one
		std::uniform_int_distribution<int> species_distribution(0, species.size()-1);
		for (int i{ 0 }; i < NO_OF_RANDOM_NEW; i++)
			species.push_back(species[species_distribution(generator)]->Mutation(generator()));
		
	}
	void plant_seeds()
	{
		std::uniform_int_distribution<int> width_distribution(0, display->get_screen_width());

		for (Species* specie : species)
		{
			plants.push_back(new Plant(specie, width_distribution(generator), 50));
		}
	}

	void starting_species()
	{
		Species* new_specie{ new Species() };
		L_Systems::Rules* DNA{ new_specie->get_DNA() };
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::seed), { static_cast<L_Systems::alphabet_T>(Cmd_lang::a),static_cast<L_Systems::alphabet_T>(Cmd_lang::stick),static_cast<L_Systems::alphabet_T>(Cmd_lang::b) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::leaf),  {});
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::stick), {});
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::left), { static_cast<L_Systems::alphabet_T>(Cmd_lang::left) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::right), { static_cast<L_Systems::alphabet_T>(Cmd_lang::right) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::next), { static_cast<L_Systems::alphabet_T>(Cmd_lang::next) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::last), {});
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::enter_seed_ballot), { static_cast<L_Systems::alphabet_T>(Cmd_lang::enter_seed_ballot) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::a), { static_cast<L_Systems::alphabet_T>(Cmd_lang::left),static_cast<L_Systems::alphabet_T>(Cmd_lang::left),static_cast<L_Systems::alphabet_T>(Cmd_lang::stick),static_cast<L_Systems::alphabet_T>(Cmd_lang::leaf),static_cast<L_Systems::alphabet_T>(Cmd_lang::last),static_cast<L_Systems::alphabet_T>(Cmd_lang::enter_seed_ballot) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::b), { static_cast<L_Systems::alphabet_T>(Cmd_lang::right),static_cast<L_Systems::alphabet_T>(Cmd_lang::right),static_cast<L_Systems::alphabet_T>(Cmd_lang::stick),static_cast<L_Systems::alphabet_T>(Cmd_lang::leaf),static_cast<L_Systems::alphabet_T>(Cmd_lang::last),static_cast<L_Systems::alphabet_T>(Cmd_lang::enter_seed_ballot) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::c), { static_cast<L_Systems::alphabet_T>(Cmd_lang::c) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::d), { static_cast<L_Systems::alphabet_T>(Cmd_lang::d) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::e), { static_cast<L_Systems::alphabet_T>(Cmd_lang::e) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::f), { static_cast<L_Systems::alphabet_T>(Cmd_lang::f) });
		species.push_back(new_specie);
	}

	void draw_ground()
	{
		
		display->set_pen(GROUND_HEIGHT, GROUND_COLOUR,1);
		display->draw_line(0, GROUND_HEIGHT/2, display->get_screen_width(), GROUND_HEIGHT/2);
	}
	void draw_background()
	{

		display->set_pen(display->get_screen_height(), SKY_COLOUR,0);
		display->draw_line(0, display->get_screen_height()/2, display->get_screen_width(), display->get_screen_height()/2);
	}
};
