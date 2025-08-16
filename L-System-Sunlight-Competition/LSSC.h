#pragma once
#include <iostream>
#include<random>
#include<vector>
#include<algorithm>
#include<cmath>
#include "ComM.h"

const int DAY_LENGTH{ 20 };//How many "segments" of growing is each day made up of
const int MAP_WIDTH{ 2500 };
const int NUMBER_OF_STARTING_PLANTS{ 10 };//Number of copies of the current default starting plant defined in world

const float MAX_ENERGY_DEBT{ -5 };// The min amount of energy a plant can have at any time
const float DEPTH_COST_OF_STICK{ 0.1 };// added to the energy cost of a stick for every stick between it and the "root"
const float COST_OF_STICK{ 0.5 };// energy cost of growing stick from "root"
const float COST_OF_LEAF{ 1 };// energy cost of growing leaf
const float COST_OF_SEED_BALLOT{ 10 };// energy cost of adding one point into the "seed ballot" for the next day (plants cant use debt to do this action)
const float ENERGY_PER_SUNLIGHT{ 0.1 };

const int NO_OF_WINNERS{ 0 };// The number of new species mutated from the highest ranking species in the seed ballot
const float PERCENT_OF_LOSERS{ 0.00 };// The percent of species removed based of the lowest ranking species in the seed ballot
const float PERCENT_OF_RANDOM_NEW{ 0.10 };// The percent of random new species each day
const int SEED_DISTRABUTION{ 90 };

const int MAX_CMD_WORD_SIZE{ 1000 };// How long can the word representing "growing commands" get (~mostly so they don't eat too much memory :) )

const float ROTATION_RADS{ 0.2f };// how many rads does a left/right command rotate a stick
const float STICK_LENGTH{ 10 };

const int LEAF_WIDTH{ 4 };//unlike the other too bellow this actually effects how much sunlight this leaf can catch (i.e for with = 3 could catch up to 3 sunlight)
const int GROUND_HEIGHT{ 50 };
const int STICK_THICKNESS{ 2 };
const int LEAF_THICKNESS{ 2 };
constexpr glm::vec3 RGB(int r, int g, int b) { return { r / 256.0, g / 256.0, b / 256.0 }; }

//LIGHT
//const glm::vec3 GROUND_COLOUR{ RGB(60, 48, 25)};
//const glm::vec3 SKY_COLOUR{ RGB(25,25,112) };
//const glm::vec3 STICK_COLOUR{ RGB(200,19,19) };
//const glm::vec3 LEAF_COLOUR{ RGB(0,200,0) };


//DARK
const glm::vec3 GROUND_COLOUR{ RGB(9,9,7) };
const glm::vec3 SKY_COLOUR{ RGB(0,1,0) };
const glm::vec3 STICK_COLOUR{ RGB(200,0,0) };
const glm::vec3 LEAF_COLOUR{ RGB(0,200,0) };

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
	Species(int position) : position(position) 
	{
		generator.seed(position);
		std::uniform_int_distribution<int> colour_distribution(0, 50);
		unique_colour_dif.x += colour_distribution(generator) / 256.0;
		unique_colour_dif.y += colour_distribution(generator) / 256.0;
		unique_colour_dif.z += colour_distribution(generator) / 256.0;
	}
	
	Species* Mutation(int seed)
	{
		generator.seed(seed);

		Species* mutated = new Species(position);
		std::uniform_int_distribution<int> seed_distribution(-SEED_DISTRABUTION, SEED_DISTRABUTION);
		mutated->position += seed_distribution(generator);

		L_Systems::Rules* new_DNA = mutated->get_DNA();
		std::bernoulli_distribution bool_distribution{};

		bool one{ bool_distribution(generator) };
		
		if (one) mutate_one(new_DNA);
		else mutate_copy(new_DNA);

		return mutated;
	}
	void PrintDNA()
	{
		std::cout << "DNA RULES:" << "\n";

		auto CMD = [](Cmd_lang in)
			{
				switch (static_cast<Cmd_lang>(in))
				{
				case(Cmd_lang::seed):
					std::cout << "SEED";
					break;
				case(Cmd_lang::leaf):
					std::cout << "LEAF";
					break;
				case(Cmd_lang::stick):
					std::cout << "STICK";
					break;
				case(Cmd_lang::left):
					std::cout << "LEFT";
					break;
				case(Cmd_lang::right):
					std::cout << "RIGHT";
					break;
				case(Cmd_lang::next):
					std::cout << "NEXT";
					break;
				case(Cmd_lang::last):
					std::cout << "LAST";
					break;
				case(Cmd_lang::enter_seed_ballot):
					std::cout << "BALLOT";
					break;
				case(Cmd_lang::a):
					std::cout << "a";
					break;
				case(Cmd_lang::b):
					std::cout << "b";
					break;
				case(Cmd_lang::c):
					std::cout << "c";
					break;
				case(Cmd_lang::d):
					std::cout << "d";
					break;
				case(Cmd_lang::e):
					std::cout << "e";
					break;
				case(Cmd_lang::f):
					std::cout << "f";
					break;
				default:
					throw std::runtime_error("UNRECOGNISED CMD");
				}
			};
		auto TO = [](std::vector<L_Systems::alphabet_T>* to, void (*DISP)(Cmd_lang in))
			{
				for (L_Systems::alphabet_T cmd : *to)
				{
					DISP(static_cast<Cmd_lang>(cmd));
					std::cout << " ";
				}
			};

		for (int i{ 0 }; static_cast<Cmd_lang>(i) < Cmd_lang::NUMBER_OF_UNIQUE_COMMANDS; i++)
		{
			CMD(static_cast<Cmd_lang>(i));
			std::cout << " -> ";
			TO(DNA[static_cast<L_Systems::alphabet_T>(i)], CMD);
			std::cout << "\n";
		}
	}

	L_Systems::Rules* get_DNA() { return &DNA; }
	int seed_ballot_points{ 0 };
	int position;

	glm::vec3 unique_colour_dif{};
private:
	std::default_random_engine generator;

	L_Systems::Rules DNA{ static_cast<L_Systems::alphabet_T>(Cmd_lang::NUMBER_OF_UNIQUE_COMMANDS) };

	void mutate_one(L_Systems::Rules* new_DNA)
	{
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
	}
	void mutate_copy(L_Systems::Rules* new_DNA)
	{
		std::uniform_int_distribution<int> cmd_distribution(0, static_cast<int>(Cmd_lang::NUMBER_OF_UNIQUE_COMMANDS) - 1);

		Cmd_lang copy_from{ cmd_distribution(generator) };
		Cmd_lang copy_to{ cmd_distribution(generator) };

		for (int i{ 0 }; static_cast<Cmd_lang>(i) < Cmd_lang::NUMBER_OF_UNIQUE_COMMANDS; i++)
		{
			Cmd_lang cmd{ static_cast<Cmd_lang>(i) };
			std::vector< L_Systems::alphabet_T> to{ *DNA[static_cast<L_Systems::alphabet_T>(cmd)] };

			if (cmd == copy_to)
				to = *DNA[static_cast<L_Systems::alphabet_T>(copy_from)] ;

			new_DNA->add(static_cast<L_Systems::alphabet_T>(cmd), to);
		}
	}
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
		int depth;
	};

public:
	Plant(Species* species, float y_planted_position) : species(species), x_planted_position(x_planted_position)
	{
		stick s{ {species->position,y_planted_position},0,0};
		plant_structure.push_back(s);
		at = { plant_structure[index].from };
		pointing = { plant_structure[index].pointing };
		depth = { plant_structure[index].depth };
	}

	void grow(Display* display)
	{
		if (command_state.size() < MAX_CMD_WORD_SIZE)
		{
			L_Systems::L_System system{ species->get_DNA(),&command_state };
			system.run(1);
		}

		Cord new_position{};
		for (L_Systems::alphabet_T Cmd : command_state)
		{
			if (energy < MAX_ENERGY_DEBT)
			{
				break;
			}

			switch (static_cast<Cmd_lang>(Cmd))
			{
			case Cmd_lang::leaf:
				energy -= COST_OF_LEAF;

				display->set_pen(LEAF_THICKNESS, LEAF_COLOUR, 3);
				display->draw_line(at.x - LEAF_WIDTH/2, at.y, at.x + LEAF_WIDTH/2, at.y);
				leafs.push_back(at);
				break;
			case Cmd_lang::stick:
				energy -= COST_OF_STICK + depth * DEPTH_COST_OF_STICK;

				new_position.x = at.x + STICK_LENGTH * std::sin(pointing);
				new_position.y = at.y + STICK_LENGTH * std::cos(pointing);
				display->set_pen(STICK_THICKNESS, STICK_COLOUR+ species->unique_colour_dif, 2);
				display->draw_line(at.x, at.y, new_position.x, new_position.y);
				plant_structure.push_back({ new_position,pointing,depth + 1});
				at = new_position;
				depth += 1;
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
				depth = plant_structure[index].depth;
				break;
			case Cmd_lang::last:
				if (index >= 1)
					index -= 1;
				at = plant_structure[index].from;
				pointing = plant_structure[index].pointing;
				depth = plant_structure[index].depth;
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

	std::vector <Cord> leafs{};
	std::vector <stick> plant_structure{};

	size_t index{ 0 };
	Cord at;
	float pointing;
	int depth;

	double energy{ 0 };
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
		for (int i{0};i < NUMBER_OF_STARTING_PLANTS; ++i)
			starting_species();
		plant_seeds();
	}

	void day(bool print = false)
	{
		if (segment == 0)
		{
			std::random_device rnd;
			generator.seed(rnd());

			display->clear_screen();
			draw_background();
			draw_ground();
		}
		if (print) std::cout << "--New Day-- " << "\n";
		while (segment<DAY_LENGTH)
		{
			growth();
			sunlight();
			segment += 1;
		}

		segment = 0;
		if (print) std::cout << "\n" << "\n"<< "Number of Species: " << species.size() << "\n";
		for (Plant* plant : plants) delete plant;
		plants.clear();

		evalate_winners();
		if (print) std::cout << "Number of Species after Evaluation: " << species.size() << "\n";

		if (print)
		{
			std::cout << "WINNER ";
			species[0]->PrintDNA();
		}

		if (species.size() > 0)
		{
			new_mutations();
			plant_seeds();
		}
	}

	void segment_day()
	{
		if (segment == 0)
		{
			std::random_device rnd;
			generator.seed(rnd());

			display->clear_screen();
			draw_background();
			draw_ground();
		}

		growth();
		sunlight();
		segment += 1;

		if (segment >= DAY_LENGTH)
		{
			segment = 0;
			for (Plant* plant : plants) delete plant;
			plants.clear();

			evalate_winners();
			if (species.size() > 0)
			{
				new_mutations();
				plant_seeds();
			}
		}
	}

	void end()
	{
		for (Plant* plant : plants) delete plant;
		plants.clear();
		for (Species* specie : species) delete specie;
		species.clear();
	}

	size_t get_number_of_species() { return species.size(); }
private:
	Display* display;
	std::vector<Species*> species{};
	std::vector<Plant*> plants{};

	std::default_random_engine generator;

	int segment{ 0 };

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
		sunlight_lines.resize(MAP_WIDTH);

		for (Plant* plant : plants)
		{
			for (auto& leaf : plant->leafs)
			{
				for (int x{ -(LEAF_WIDTH) / 2 }; x <= LEAF_WIDTH / 2; x++)
				{
					int X = x+leaf.x;
					if (X >= 0 && X < MAP_WIDTH)
					{
						if (leaf.y > sunlight_lines[X].height)
						{
							sunlight_lines[X].plant = plant;
							sunlight_lines[X].height = leaf.y;
						}
					}
				}
			}
		}

		for (HighScore winner : sunlight_lines)
		{
			if (winner.plant != NULL)
				winner.plant->energy += ENERGY_PER_SUNLIGHT;
		}

	}

	void evalate_winners()
	{
		std::sort(species.begin(), species.end(), [](Species* s1, Species* s2) {return s1->seed_ballot_points > s2->seed_ballot_points; });
		
		for (size_t index{ species.size()}; index > 0; index--)
		{
			if (species[index-1]->seed_ballot_points > 0)
				break;

			delete species[index-1];
			species.erase(species.begin() + index-1);
		}

		if (species.size()* PERCENT_OF_LOSERS >= 1)
		{
			for (int i{ 0 }; i < static_cast<int>(species.size() * PERCENT_OF_LOSERS); i++)
			{
				delete species[species.size()-1];
				species.erase(species.end()-1);
			}
		}

		for (Species* specie : species) specie->seed_ballot_points = 0;
	}
	void new_mutations()
	{
		//at least one random
		std::uniform_int_distribution<int> species_distribution(0, species.size() - 1);
		species.push_back(species[species_distribution(generator)]->Mutation(generator()));

		//winners get one
		for (int i{0}; i< NO_OF_WINNERS;i++)
			species.push_back(species[i]->Mutation(generator()));

		//random one
		if (species.size() * PERCENT_OF_RANDOM_NEW >= 1)
		{
			for (int i{ 0 }; i < static_cast<int>(species.size() * PERCENT_OF_RANDOM_NEW); i++)
				species.push_back(species[species_distribution(generator)]->Mutation(generator()));
		}
	}
	void plant_seeds()
	{
		std::uniform_int_distribution<int> seed_distribution(-1, 1);

		for (Species* specie : species)
		{
			specie->position += seed_distribution(generator);
			plants.push_back(new Plant(specie, GROUND_HEIGHT));
		}
	}

	void starting_species()
	{
		std::uniform_int_distribution<int> width_distribution(0, MAP_WIDTH);
		Species* new_specie{ new Species(width_distribution(generator)) };
		L_Systems::Rules* DNA{ new_specie->get_DNA() };
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::seed), { static_cast<L_Systems::alphabet_T>(Cmd_lang::a),static_cast<L_Systems::alphabet_T>(Cmd_lang::stick),static_cast<L_Systems::alphabet_T>(Cmd_lang::leaf),static_cast<L_Systems::alphabet_T>(Cmd_lang::b) });
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
		display->draw_line(0, GROUND_HEIGHT/2, MAP_WIDTH, GROUND_HEIGHT/2);
	}
	void draw_background()
	{

		display->set_pen(display->get_screen_height(), SKY_COLOUR,0);
		display->draw_line(0, display->get_screen_height()/2, MAP_WIDTH, display->get_screen_height()/2);
	}
};
