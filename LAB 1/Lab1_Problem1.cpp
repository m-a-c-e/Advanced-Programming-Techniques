/*
 * Author: Manan Patel
 * Class: ECE 6122
 * Last Date Modified: 09/21/2021
 *
 * Description:
 * Calculate Prime factors
 */

// Necessary imports	
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

bool GetPrimeFactors(const unsigned long ulInputNumber, string& str_output) {
	/*
	This function takes in an unsigned long number, gets the prime factors
	and stores them as a string
		Args:
			ulInputNumber: number whose prime factors are to be calculated
			str_output:	stores the string of prime factors
		Return:
			NA
	*/
	unsigned long num = ulInputNumber;							// create a copy
	while (true) 
	{
		if (num % 2 == 0) 
		{														// check if 2 is a prime factor
			string str_2 = to_string(2);
			str_output.append(str_2);
			str_output.append(", ");
			break;
		}
		else 
		{
			break;
		}
	}

	while (num % 2 == 0) 
	{															// divide by 2 if possible
		num = num / 2;
	}
	
	for (int i = 3; i < sqrt(num); i = i + 2) 
	{					
		while (num % i == 0) 
		{
			string str_i = to_string(i);
			str_output.append(str_i);
			str_output.append(", ");
			num = num / i;
			break;
		}
		while (num % i == 0) 
		{
			num = num / i;
		}
	}

	if (num > 2) 
	{
		string str_num = to_string(num);
		str_output.append(str_num);
	}
	return true;
}

int main(int argc, char** argv) 
{
	ofstream myfile;

	// if invalid input is provided
	if (argc != 2) 
	{
		myfile.open("output1.txt", ios::trunc);
		myfile << "Invalid Input.";
		myfile.close();
		return 0;
	}

	// if input number is negative
	if (argv[1][0] == '-') 
	{
		myfile.open("output1.txt", ios::trunc);
		myfile << "Invalid Input.";
		myfile.close();
		return 0;
	}
	
	// convert from string to unsigned long
	unsigned long ul = stoul(argv[1], nullptr, 0);

	// check for input = 1
	if (ul <= 1) 
	{
		myfile.open("output1.txt", ios::trunc);
		myfile << "No prime factors.";
		myfile.close();
		return 0;
	}

	string output = "";
	bool h = GetPrimeFactors(ul, output);

	// Write to output file
	myfile.open("output1.txt", ios::trunc);
	myfile << output;
	myfile.close();

	return 0;
}
