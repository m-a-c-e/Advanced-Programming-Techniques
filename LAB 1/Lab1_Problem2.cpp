/*
 * Author: Manan Patel
 * Class: ECE 6122
 * Last Date Modified: 09/21/2021
 *
 * Description:
 * Langdon's Ant: Calculates the number of black squares
 */

// necessary imports
#include <iostream>
#include <stdio.h>
#include <unordered_map>
#include <string>
#include <fstream>

// define macros
#define NORTH 'N'
#define SOUTH 'S'
#define EAST 'E'
#define WEST 'W'

using namespace std;

class Ant
{
	/* Ant class:
	*	Used to represent the current location, orientation and color of square
		the ant is currently on.
	*/
public:
	unsigned long x_pos;
	unsigned long y_pos;
	char color;						// takes 'w' for white or 'b' for black
	char orientation;				// takes NORTH, SOUTH, EAST or WEST
};

void update_ant(Ant* ant, char color) 
{
	/* update_ant:
		Used to modify a given ant object based on its orientation and color of square
	 Arguments:
		Ant* : pointer to an ant object
		color: the color of the square on which the ant is on
	Returns:
		NA
	*/
	if (color == 'w') 
	{
		if (ant -> orientation == NORTH) 
		{
			ant -> orientation = EAST;							// rotate 90 clockwise
			ant -> x_pos++;										// move forward
		}
		else if (ant -> orientation == EAST) 
		{
			ant -> orientation = SOUTH;
			ant -> y_pos--;
		}
		else if (ant -> orientation == SOUTH) 
		{
			ant -> orientation = WEST;
			ant -> x_pos--;
		}
		else if (ant -> orientation == WEST) 
		{
			ant -> orientation = NORTH;
			ant -> y_pos++;
		}
	}
	else 
	{
		if (ant -> orientation == NORTH) 
		{
			ant -> orientation = WEST;					// rotate counter-clockwise
			ant -> x_pos--;								// move forward
		}		
		else if (ant -> orientation == EAST) 
		{
			ant -> orientation = NORTH;
			ant -> y_pos++;
		}
		else if (ant -> orientation == SOUTH) 
		{
			ant -> orientation = EAST;
			ant -> x_pos++;
		}
		else if (ant -> orientation == WEST) 
		{
			ant -> orientation = SOUTH;
			ant -> y_pos--;
		}
	}
}

int main(int argc, char** argv) 
{
	unsigned long input = stoul(argv[1]);				
	Ant ant = { 0, 0, 'w', NORTH};			
	unordered_map <string, char> umap;
	string str_pos = "(" + to_string(ant.x_pos) + ", " + to_string(ant.y_pos) + ")";
	umap[str_pos] = 'w';
	string curr_str_pos;
	char curr_color;

	string new_str_pos;
	string new_color;

	while (input != 0) 
	{
		curr_str_pos = "(" + to_string(ant.x_pos) + ", " + to_string(ant.y_pos) + ")";
		curr_color = ant.color;

		// swap color of current square
		if (curr_color == 'w') 
		{
			umap[curr_str_pos] = 'b';
		}
		else 
		{
			umap[curr_str_pos] = 'w';
		}

		// update ant object
		update_ant(&ant, curr_color);

		// New location
		new_str_pos = "(" + to_string(ant.x_pos) + ", " + to_string(ant.y_pos) + ")";
		if (umap.count(new_str_pos)) 
		{
			// 1. location in map
			unordered_map<string, char>::const_iterator got = umap.find(new_str_pos);
			ant.color = got->second;
		}
		else 
		{
			// 2. location not in map
			pair<string, char> element(new_str_pos, 'w');
			umap.insert(element);
			ant.color = 'w';
		}
		input--;
	};
	
	// count number of black squares
	unsigned long count = 0;
	for (auto it = umap.cbegin(); it != umap.cend(); ++it) 
	{
		if (it->second == 'b') 
		{
			count++;
		}
	}

	// Write to output file
	ofstream myfile;
	myfile.open("output2.txt", ios::trunc);
	myfile << count;
	myfile.close();

	return 0;
}
