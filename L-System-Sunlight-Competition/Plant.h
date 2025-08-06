#pragma once
#include <iostream>
#include<random>
#include<vector>
#include<cmath>
#include"..\Interface\MYR.h"
#include "ComM.h"

class Display
{
public:
	Display(std::vector<MYR::Vertex>* vertices, std::vector<uint32_t>* indices) : vertices(vertices), indices(indices){}

	void set_pen(int width, glm::vec3 colour, double depth)
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
		vertices->push_back({ {get_corner(x1,y1,x2,y2,false) / (double) screen_width, get_corner(y1,x1,y2,x2,true) / (double)screen_height, pen_depth},pen_colour });
		vertices->push_back({ {get_corner(x1,y1,x2,y2,true) / (double)screen_width, get_corner(y1,x1,y2,x2,false) / (double)screen_height, pen_depth},pen_colour });
		vertices->push_back({ {get_corner(x2,y2,x1,y1,true) / (double)screen_width, get_corner(y2,x2,y1,x1,true) / (double)screen_height, pen_depth},pen_colour });
		vertices->push_back({ {get_corner(x2,y2,x1,y1,false) / (double)screen_width, get_corner(y2,x2,y1,x1,false) / (double)screen_height, pen_depth},pen_colour });

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

	int pen_width{};
	glm::vec3 pen_colour{};
	double pen_depth{};

	double get_corner(int x1, int y1, int x2, int y2, bool pos) const
	{
		if (pos)
			return (x1 + (pen_width * ((y2 - y1) / (double) std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)))));
		else
			return (x1 - (pen_width * ((y2 - y1) / (double) std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)))));
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

	L_Systems::Rules* get_DNA() { return &DNA; }
private:

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
	Plant(Species* species, int x_planted_position, int y_planted_position) : species(species), x_planted_position(x_planted_position) 
	{
		stick s{ {x_planted_position,y_planted_position},0 };
		plant_structure.push_back(s);
	}

	void grow(Display* display)
	{
		L_Systems::L_System system{ species->get_DNA(),&command_state };
		system.run(1);

		const int stick_length{ 3 };

		size_t index{ 0 };
		Cord at{ plant_structure[index].from };
		float pointing{ plant_structure[index].pointing };

		Cord new_position{};
		for (L_Systems::alphabet_T Cmd : command_state)
		{
			switch (static_cast<Cmd_lang>(Cmd))
			{
			case Cmd_lang::leaf:
				leafs.push_back(at);
				break;
			case Cmd_lang::stick:
				new_position.x = at.x + stick_length * std::sin(pointing);
				new_position.y = at.y + stick_length * std::cos(pointing);
				display->set_pen(1, {0.1,0.9,0.1},2);
				display->draw_line(at.x, at.y, new_position.x, new_position.y);
				plant_structure.push_back({ new_position,pointing });
				at = new_position;
				break;
			case Cmd_lang::left:
				pointing += 1;
				break;
			case Cmd_lang::right:
				pointing -= 1;
				break;
			case Cmd_lang::next:
				index += 1;
				at = plant_structure[index].from;
				pointing = plant_structure[index].pointing;
				break;
			case Cmd_lang::last:
				index -= 1;
				at = plant_structure[index].from;
				pointing = plant_structure[index].pointing;
				break;
			case Cmd_lang::enter_seed_ballot:
				break;
			default:
				break;
			}
		}
	}

	std::vector <Cord> leafs{};
	std::vector <stick> plant_structure{};

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
		display->clear_screen();
		draw_background();
		draw_ground();
		//std::cout << "--New Day-- " << "\n";
		growing();
		//std::cout << "Number of Living Plants: " << plants.size() << "\n";
		//std::cout << "Number of Living Species: " << species.size() << "\n";
		for (Plant* plant : plants) delete plant;
		plants.clear();
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

	void growing()
	{
		for (int segment{ 0 }; segment < day_length; segment++)
		{
			for (Plant* plant : plants) plant->grow(display);
		}
	}

	void plant_seeds()
	{
		for (Species* specie : species)
		{
			std::default_random_engine generator;
			std::uniform_int_distribution<int> distribution(-display->get_screen_width() / 2, display->get_screen_width() / 2);
			plants.push_back(new Plant(specie,distribution(generator), -display->get_screen_width()/2+75));
		}
	}

	void starting_species()
	{
		Species* new_specie{ new Species() };
		L_Systems::Rules* DNA{ new_specie->get_DNA() };
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::seed), { static_cast<L_Systems::alphabet_T>(Cmd_lang::stick) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::leaf), { static_cast<L_Systems::alphabet_T>(Cmd_lang::a) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::stick), { static_cast<L_Systems::alphabet_T>(Cmd_lang::leaf) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::left), { static_cast<L_Systems::alphabet_T>(Cmd_lang::left) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::right), { static_cast<L_Systems::alphabet_T>(Cmd_lang::right) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::next), { static_cast<L_Systems::alphabet_T>(Cmd_lang::next) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::last), { static_cast<L_Systems::alphabet_T>(Cmd_lang::last) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::enter_seed_ballot), { static_cast<L_Systems::alphabet_T>(Cmd_lang::enter_seed_ballot) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::a), { static_cast<L_Systems::alphabet_T>(Cmd_lang::b) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::b), { static_cast<L_Systems::alphabet_T>(Cmd_lang::c) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::c), { static_cast<L_Systems::alphabet_T>(Cmd_lang::enter_seed_ballot) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::d), { static_cast<L_Systems::alphabet_T>(Cmd_lang::d) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::e), { static_cast<L_Systems::alphabet_T>(Cmd_lang::e) });
		DNA->add(static_cast<L_Systems::alphabet_T>(Cmd_lang::f), { static_cast<L_Systems::alphabet_T>(Cmd_lang::f) });
		species.push_back(new_specie);
	}

	void draw_ground()
	{
		display->set_pen(150, { 0.9,0.10,0.10 },1);
		display->draw_line(-display->get_screen_width(), 0, display->get_screen_width(), 0);
	}

	void draw_background()
	{
		display->set_pen(display->get_screen_height()/2, { 0.3,0.30,0.90 },0);
		display->draw_line(-display->get_screen_width(), display->get_screen_height()/2, display->get_screen_width(), display->get_screen_height()/2);
	}
};
