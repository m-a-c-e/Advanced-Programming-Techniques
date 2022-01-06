/*
 * Author: Manan Patel
 * Class: ECE 6122
 * Last Date Modified: 15/11/2021
 *
 * Description:
 * Heat distribution on a thin plate.
 * Uses Laplace's finite difference method to calculte the 
 * approximate distribution of thin plate within a given number of iterations.
 */


// imports
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include <cuda.h>

using namespace std;

__global__ void compute_step(double* arr_list, double* updated_arr_list, int n_total)
{
/* Purpose: To compute the temperature distribution within a thin
*  plate using laplace finite difference method.
*  Args:
		arr_list -> array which holds the current temperature distribution
		updated_arr_list -> array which holds the newly computed temperature distribution
		n_total -> n_total * n_total is the total length of the array 
*  Returns:
		NA
*/
	int thId = threadIdx.x;
	int stride = blockDim.x;

	int total = n_total * n_total;
	for (int i = thId; i < total; i+=stride)
	{
		if(i <= n_total || (i + 1) % n_total == 0 || (i % n_total == 0) || (i >= total - n_total - 1))
		{
			updated_arr_list[i] = arr_list[i];
			continue;
		}
		updated_arr_list[i] = 0.25 * (arr_list[i - 1] +
			arr_list[i + 1] +
			arr_list[i - n_total] +
			arr_list[i + n_total]);
	}
}

__global__ void update_step(double* arr_list, double* updated_arr_list, int n_total)
{
/* Purpose: To set the current temperature distribution to
*  the computed one.
*  Args:
		arr_list -> array which holds the current temperature distribution
		updated_arr_list -> array which holds the newly computed temperature distribution
		n_total -> n_total * n_total is the total length of the array 
*  Returns:
		NA
*/
	int thId = threadIdx.x;
	int stride = blockDim.x;

	int total = n_total * n_total;
	for (int i = thId; i < total; i+=stride)
	{
		arr_list[i] = updated_arr_list[i];
	}
}

bool checkIfInt(string str)
{
/* Purpose: To check if the input string is an integer
*  Args:
		str -> string	
*  Returns:
		true or false
*/

	for(int i = 0; i < str.length(); i ++)
	{
		if(isdigit(str[i]) == false)
		{
			return false;
		}
	}
	return true;
}


int main(int argc, char* argv[])
{
	// Start to measure time
	cudaEvent_t start;
	cudaEvent_t stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start);

	// Checking for commmand line arguments
	if(argc != 5)
	{
		cout << "Invalid parameters. Please check your values." << endl;
		return 0;
	}

	string n_str = "-N";
	string iterations_str = "-l";
	double n = -1;
	double iterations = -1;


	for(int i = 0; i < argc; i ++)
	{
		if(n_str.compare(argv[i]) == 0)
		{
			// if -N input is non-numeric
			if(!checkIfInt(argv[i+1]))
			{
				cout << "Invalid parameters. Please check your values." << endl;
				return 0;
			}
			n = strtod(argv[i + 1], nullptr);
		}	
		if(iterations_str.compare(argv[i]) == 0)
		{
			// if -l input is non-numeric
			if(!checkIfInt(argv[i+1]))
			{
				cout << "Invalid parameters. Please check your values." << endl;
				return 0;
			}
			iterations = strtod(argv[i + 1], nullptr);
		}
	}

	// If n or iterations is negative
	if(n <= 0 || iterations <= 0)
	{
		cout << "Invalid parameters. Please check your values." << endl;
		return 0;
	}


	double n_total = n + 2;			// includes the points which are not to be updated as well

	// allocate memory and set initial temperature dist
	double* arr_list = (double*)malloc(sizeof(double) * n_total * n_total);
	for (int i = 0; i < n_total * n_total; i++)
	{
		if (i > (0.3 * (n_total - 1)) && i < (0.7 * (n_total - 1)))
		{
			arr_list[i] = 100.0;
		}
		else
		{
			arr_list[i] = 20.0;
		}
	}

	// Make memory on device and copy initial array
	double* d_arr_list;
	cudaMalloc((void**)&d_arr_list, sizeof(double) * n_total * n_total);
	cudaMemcpy(d_arr_list, arr_list, sizeof(double) * n_total * n_total, cudaMemcpyHostToDevice);

	double* d_updated_arr_list;
	cudaMalloc((void**)&d_updated_arr_list, sizeof(double) * n_total * n_total);

	// check the max number of threads allowed per block
	// set the number of threads to launch
	cudaSetDevice(0);
	cudaDeviceProp deviceProp;
	cudaGetDeviceProperties(&deviceProp, 0);
	int maxThreads = deviceProp.maxThreadsPerBlock;
	int n_threads = 0;
	if(maxThreads > n_total * n_total)
	{
		n_threads = n_total * n_total;
	}
	else
	{
		n_threads = maxThreads;
	}

	// Run the kernels iterations number of times
	for (int k = 0; k < iterations; k++)
	{
		// computes the temp dist
		compute_step<<<1, n_threads>>>(d_arr_list, d_updated_arr_list, n_total);
		cudaDeviceSynchronize();

		// sets the temp dist from d_updated_arr_list to d_arr_list
		update_step<<<1, n_threads>>>(d_arr_list, d_updated_arr_list, n_total);
		cudaDeviceSynchronize();
	}
	
	// copy array back to host
	cudaMemcpy(arr_list, d_arr_list, sizeof(double) * n_total * n_total, cudaMemcpyDeviceToHost);

	// writing to a file
	ofstream myfile;
	myfile.open("Temperatures.csv", ios::binary);
	for (int i = 0; i < n_total * n_total; i+=n_total)
	{
		stringstream ss;
		for (int j = 0; j < n_total; j++)
		{
			ss << arr_list[i + j] << ',';
		}
		ss << '\n';
		myfile << ss.str();
	}

	// Deallocate memory on device and host
	free(arr_list);
	cudaFree(d_arr_list);
	cudaFree(d_updated_arr_list);
	myfile.close();

	// Output total time taken by the program
	cout.precision(5);
	cudaEventRecord(stop);
	float milliseconds = 0;
	cudaEventElapsedTime(&milliseconds, start, stop);
	cout << milliseconds << endl;

	return 0;
}
