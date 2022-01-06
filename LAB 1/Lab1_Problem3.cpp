/*
 * Author: Manan Patel
 * Class: ECE 6122
 * Last Date Modified: 09/21/2021
 *
 * Description:
 * Investigating multiple reflections of a laser beam. Calculate the total
	number of reflections before exiting.
 */

// Imports
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <math.h>

// Macros
#define PI 3.1416

#define DEG_TO_RAD (3.1416 / 180)
#define RAD_TO_DEG (180 / 3.1416)

#define A_X -10
#define A_Y	20 * sin(60 * DEG_TO_RAD)

#define B_X 10
#define B_Y 20 * sin(60 * DEG_TO_RAD)

// D represents the left side cut-out point
#define D_X -0.01 / tan( 60 * DEG_TO_RAD )		
#define	D_Y 0.01

// E represents the right side cut-out point
#define E_X 0.01 / tan( 60 * DEG_TO_RAD )
#define E_Y 0.01

#define C_X 0
#define C_Y 0

using namespace std;

class Line {
	/* Line class:
	*	Used to represent a line/ray.
	*/
public:
	double x1;
	double y1;
	double angle;			// radians
	string reflected_from = "";
};

void get_intersection_point(double* point, const Line* line1, const Line* line2) 
{
/*
	Description:
		Stores the point of intersection in point array
	Arguments:
		point: pointer to an array of two elements
		line1: represents a line (see line class)
		line2: represents a line (see line class)
	Returns:
		NA
*/
	double c1;
	double c2;
	double m1;
	double m2;
	double x;
	double y;

	// If slope of line1 is Infinity
	if (tan(line1->angle) == INFINITY) 
	{
		m2 = tan(line2->angle);
		x = line1->x1;							// x co-ordinate of intersection
		c2 = line2->y1 - m2 * line2->x1;		// c in y = m * x + c
		y = m2 * x + c2;						// y co-ordinate of intersection	
		point[0] = x;
		point[1] = y;
		return;
	}

	// If slope of line2 is Infinity
	if (tan(line2->angle) == INFINITY) 
	{
		m1 = tan(line1->angle);
		x = line2->x1;							// x co-ordinate of intersection
		c1 = line1->y1 - m1 * line1->x1;		// c in y = m * x + c
		y = m1 * x + c1;						// y co-ordinate of intersection
		point[0] = x;
		point[1] = y;
		return;
	}

	// If neither line has infinite slope
	m1 = tan(line1->angle);
	m2 = tan(line2->angle);
	c1 = line1->y1 - m1 * line1->x1;			// c in y = m * x + c
	c2 = line2->y1 - m2 * line2->x1;

	x = (c2 - c1) / (m1 - m2);					// x co-ordinate of intersection
	y = m1 * x + c1;							// y co-ordinate of intersection

	point[0] = x;
	point[1] = y;
	return;
}

bool check_intersection_AB(Line* line, Line* line_segment) 
{
/*
	Description:
		Check if line intersects with line_segment AB
	Arguments:
		line1: represents a line (see line class)
		line_segment: represents a line (see line class)
	Returns:
		true: intersection found
		false: intersection not found
*/

	// Return false if the line is coming after being reflected from AB
	if (line->reflected_from == "AB") 
	{
		return false;
	}

	double point[2];
	get_intersection_point(point, line, line_segment);
	double x0 = point[0];
	double y0 = point[1];

	// check if point is within the bounds of AB
	if ((y0 == 20 * sin(60 * DEG_TO_RAD)) && (x0 > -10 && x0 < 10)) 
	{
		return true;
	}

	return false;
}

bool check_intersection_BE(Line* line, Line* line_segment) 
{
/*
	Description:
		Check if line intersects with line_segment BE
	Arguments:
		line1: represents a line (see line class)
		line_segment: represents a line (see line class)
	Returns:
		true: intersection found
		false: intersection not found
*/
	// Return false if the line is coming after being reflected from BE
	if (line->reflected_from == "BE") 
	{
		return false;
	}

	double point[2];
	get_intersection_point(point, line, line_segment);
	double x0 = point[0];
	double y0 = point[1];

	// check if point is within the bounds of BE 
	if ((y0 > 0.01 && y0 < 20 * sin(60 * DEG_TO_RAD)) && (x0 > 0.01 / tan(60 * DEG_TO_RAD) && x0 < 10)) 
	{
		return true;
	}
	return false;
}

bool check_intersection_AD(Line* line, Line* line_segment) {
/*
	Description:
		Check if line intersects with line_segment AD
	Arguments:
		line1: represents a line (see line class)
		line_segment: represents a line (see line class)
	Returns:
		true: intersection found
		false: intersection not found
*/
	// Return false if the line is coming after being reflected from BE
	if (line->reflected_from == "AD") 
	{
		return false;
	}

	double point[2];
	get_intersection_point(point, line, line_segment);
	double x0 = point[0];
	double y0 = point[1];

	// check if point is within the bounds of AD
	if ((y0 > 0.01 && y0 < 20 * sin(60 * DEG_TO_RAD)) && (x0 > -10 && x0 < -0.01 / tan(60 * DEG_TO_RAD))) 
	{
		return true;
	}
	return false;
}

void form_line(Line* line, double x1, double y1, double x2, double y2) {
/*
	Description:
		Given co-ordinates of two points, store the parameters of the line
		passing through these points in line.
	Arguments:
		line1: represents a line (see line class)
		x1: x co-ordinates of point1
		x2: x co-ordinates of point2
		y1: y co-ordinates of point1
		y2: y co-ordinates of point2
	Returns:
		NA
*/
	line->x1 = x1;
	line->y1 = y1;
	line->angle = atan2((y2 - y1), (x2 - x1));		// slope
}

void reflect_line(Line* line, Line* ref_line, double x1, double y1, int* counter) {
/*
	Description:
		Given line, ref_line and co-ordinates of point of intersection, 
		update line with new parameters which is reflected about ref_line.
		Also updates the number of reflections counter
	Arguments:
		line1: represents a line (see line class)
		ref_line: represents the line about whose which line1 is getting reflected
		x1: x co-ordinates of point of intersection
		y1: y co-ordinates of point of intersection
	Returns:
		NA
*/
	double new_angle = PI - line->angle + ref_line->angle;		// angle of reflected line
	line->angle = new_angle;
	line->x1 = x1;
	line->y1 = y1;
	*counter = *counter + 1;									// update the number of reflections counter
}

int main(int argc, char** argv) {

	double x_input = strtod(argv[1], nullptr);					// get the input from command line

	Line line_AB;
	form_line(&line_AB, A_X, A_Y, B_X, B_Y);
	
	Line line_BE;
	form_line(&line_BE, B_X, B_Y, E_X, E_Y);
	
	Line line_AD;
	form_line(&line_AD, A_X, A_Y, D_X, D_Y);

	Line initial_line;
	form_line(&initial_line, C_X, C_Y, x_input, A_Y);
	
	double intersection_point[2] = { 0, 0 };
	get_intersection_point(intersection_point, &initial_line, &line_AB);

	int counter = 0;
	reflect_line(&initial_line, &line_AB, intersection_point[0], intersection_point[1], &counter);
	initial_line.reflected_from = "AB";

	// Keep on iterating untill the new line does not intersect any side of the triangle
	while (true) {		
		if (check_intersection_AB(&initial_line, &line_AB)) 
		{
			get_intersection_point(intersection_point, &initial_line, &line_AB);
			reflect_line(&initial_line, &line_AB, intersection_point[0], intersection_point[1], &counter);
			initial_line.reflected_from = "AB";
			continue;
		}
		if (check_intersection_AD(&initial_line, &line_AD)) 
		{
			get_intersection_point(intersection_point, &initial_line, &line_AD);
			reflect_line(&initial_line, &line_AD, intersection_point[0], intersection_point[1], &counter);
			initial_line.reflected_from = "AD";
			continue;
		}
		if (check_intersection_BE(&initial_line, &line_BE)) 
		{
			get_intersection_point(intersection_point, &initial_line, &line_BE);
			reflect_line(&initial_line, &line_BE, intersection_point[0], intersection_point[1], &counter);
			initial_line.reflected_from = "BE";
			continue;
		}
		break;
	}

	// Write to output file
	ofstream myfile;
	myfile.open("output3.txt", ios::trunc);
	myfile << counter;
	myfile.close();

	return 0;
}
