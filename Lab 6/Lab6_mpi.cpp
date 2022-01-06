/*
 * Author: Manan Patel
 * Class: ECE 6122
 * Last Date Modified: 24/11/2021
 *
 * Description:
 * Heat distribution on a thin plate.
 * Uses Laplace's finite difference method to calculte the 
 * approximate distribution of thin plate within a given number of iterations.
 * Multiprocessing used by requesting 8 nodes with 2 processors each.
 */


// imports
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <string>
#include <chrono>
#include "mpi.h"

using namespace std;

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


void print_array(double* array, int rows, int cols)
{
/* Purpose: To print an array in a matrix format to console
 * Args:
 * 	array -> holds the elements
 * 	rows -> no of rows
 * 	cols -> no of columns
 * Returns:
 * 	NA
 */
    for(int i = 0; i < rows * cols; i ++)
    {
	// setting precision to 6 decimals
        printf("%.6lf,", array[i]);	

	// output a new line at the end of each row
        if((i + 1) % cols == 0)
        {
            printf("\n");
        }
    }
}


void get_temperature_dist(double* arr_list, double* updated_arr_list, int n_total, int parts, int rank, int cumm_parts)
{
/* Purpose: To compute the temperature distribution within a thin
*  plate using laplace finite difference method.
*  Args:
		arr_list -> array which holds the current temperature distribution
		updated_arr_list -> array which holds the newly computed temperature distribution
		n_total -> n_total is the number of elements in each row (here a row has to be visualized as the input is a flattened array) 
		parts -> number of parts which are to be updated
		rank -> the rank of the node which made the call
		cumm_parts -> summation of all the parts which are to updated by nodes with with lower rank. Used to keep track of where to start.
*  Returns:
		NA
*/
	int total = n_total * parts;					// get the length of updated_arr_list
    int j = n_total  + cumm_parts * n_total;		// set the starting index for arr_list which holds n_total * n_total elements
	for (int i = 0; i < total; i++)
	{
		if(i % n_total == 0 || (i + 1) % n_total == 0)		// skip the elements which are at the edge of the plate
		{
		    updated_arr_list[i] = arr_list[j];
		    j ++;
		    continue;
		}
		updated_arr_list[i] = 0.25 * (arr_list[j - 1] +		// compute new values for desired elements
			arr_list[j + 1] +
			arr_list[j - n_total] +
			arr_list[j + n_total]);
        	j++;
	}
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);         // initialize MPI environment
    int numtasks, rank;             // numtasks = total number of processors
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

    int n = -1;
    int iterations = -1;

    ///////////////////////////////////////
    // Checking for commmand line arguments
    ///////////////////////////////////////
        
    if(argc != 5)
    {
        if(rank == 0)
        {
            cout << "Invalid parameters. Please check your values." << endl;
        }
        MPI_Finalize();
        return 0;
    }

    string n_str = "-N";
    string iterations_str = "-l";
    for(int i = 0; i < argc; i ++)
    {
        if(n_str.compare(argv[i]) == 0)
        {
            // if -N input is non-numeric
            if(!checkIfInt(argv[i+1]))
            {
                if(rank == 0)
                {
                    cout << "Invalid parameters. Please check your values." << endl;
                }
                MPI_Finalize();
                return 0;
            }
            n = stoi(argv[i + 1], nullptr);
        }	
        if(iterations_str.compare(argv[i]) == 0)
        {
            // if -l input is non-numeric
            if(!checkIfInt(argv[i+1]))
            {
                if(rank == 0)
                {
                    cout << "Invalid parameters. Please check your values." << endl;
                }
                MPI_Finalize();
                return 0;
            }
            iterations = stoi(argv[i + 1], nullptr);
        }
    }

    // If n or iterations is negative
    if(n <= 0 || iterations <= 0)
    {
        if(rank == 0)
        {
            cout << "Invalid parameters. Please check your values." << endl;
        }
        MPI_Finalize(); 
        return 0;
    }

    int n_total = n + 2;            // 2 edges
    int parts = n / numtasks;       // dividing the datapoints
    int rem = n % numtasks;         // remainder
    int* parts_list      = (int*)malloc(sizeof(int) * numtasks);    // holds the number of parts for each rank (starting with rank 1)
                                                                    // Note: index 0 --> rank 1
                                                                    //       index 1 --> rank 2
                                                                    //       ...
                                                                    //       index numtasks - 2 --> rank numtasks - 1
                                                                    //       index numtasks - 1 --> rank 0 (special case)
    int* cumm_parts_list = (int*)malloc(sizeof(int) * numtasks);    // holds the number of parts previous to each rank 
    cumm_parts_list[0] = 0;                                         // initializing the first element

    if(n <= numtasks)
    {
        numtasks = n;
    }

    int count = 0;
    for(int i = 0; i < numtasks; i ++)
    {
        parts_list[i] = parts;                  // first append equal number to each rank/index
        if(count < rem)
        {
            parts_list[i] += 1;                 // add 1 to "rem" number of ranks to cater to the remainder
        }
        count ++;
    }

    for(int i = 1; i < numtasks; i ++)
    {
        // compute the cummulative for each rank
        cumm_parts_list[i] = cumm_parts_list[i-1] + parts_list[i-1];    
    }

    if(rank == 0)
    {
        /////////////////////////////////////////////////
        // Rank 0 node //////////////////////////////////
        // //////////////////////////////////////////////

        // initialize array /////////////////////////
        double* updated_arr_list = (double*)malloc(sizeof(double) * n_total * n_total);
        for (int i = 0; i < n_total * n_total; i++)
		{
			if (i > (0.3 * (n_total - 1)) && i < (0.7 * (n_total - 1)))
			{
				updated_arr_list[i] = 100.0;
			}
			else
			{
				updated_arr_list[i] = 20.0;
			}
		}

        ///////////////////////////////////////////////
        // Need to run iterations number of times /////
        // ////////////////////////////////////////////
        auto start = std::chrono::high_resolution_clock::now();
	double* arr_list = (double*)malloc(sizeof(double) * n_total * parts_list[numtasks - 1]);
        for(int k = 0; k < iterations; k ++)
        {

            // send origianl array to the processors //////////////
            // (blocking) /////////////////////////////////
            for(int r = 1; r < numtasks; r ++)
            {
                MPI_Send((void*)updated_arr_list, n_total * n_total, MPI_DOUBLE, r, 101, MPI_COMM_WORLD);
            }

            // compute the value for rank 0 and store them in arr_list
            get_temperature_dist(updated_arr_list, arr_list, n_total, parts_list[numtasks - 1], numtasks, cumm_parts_list[numtasks - 1]);
            // receive computed arrays from processors /////
            // (blocking) /////////////////////////////////
            for(int r = 1; r < numtasks; r ++)
            {
                // update the original array
                MPI_Recv((void*)(updated_arr_list + n_total +  cumm_parts_list[r - 1] * n_total), n_total * parts_list[r - 1], MPI_DOUBLE, r, 201, MPI_COMM_WORLD, &status);
            }
            
            // update the original array for rank 0
            for(int i = 0; i < n_total * parts_list[numtasks - 1]; i ++)
            {
                updated_arr_list[i + n_total + cumm_parts_list[numtasks - 1] * n_total] =  arr_list[i];
            }   
        }
        auto stop = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = stop - start;
        printf("Time for %d datapoints and %d iterations is %lf (ms)\n", n, iterations, (float)elapsed.count() * 1000);

        // print array ///////////////////////////////
        print_array(updated_arr_list, n_total, n_total);
        printf("\n");

        // free memory ///////////////////////////////
        free(arr_list);
        free(updated_arr_list);
    }
    else
    {
        ////////////////////////////////////////////////////////////
        // For nodes with rank > 0 /////////////////////////////////
        // /////////////////////////////////////////////////////////
        int r = rank;

        // initialize array depending on the number of parts it holds
        double* updated_arr_list = (double*)malloc(sizeof(double) * parts_list[r - 1] * n_total);
        double* arr_list = (double*)malloc(sizeof(double) * n_total * n_total);

        // /////////////////////////////////////////////////////////
        // Need to also run iterations number of times /////////////
        for(int k = 0; k < iterations; k ++)
        {	
            // Receive array from master node //////////////////////////
            // (blocking) //////////////////////////////////////////////
            MPI_Recv((void*)arr_list, n_total * n_total, MPI_DOUBLE, 0, 101, MPI_COMM_WORLD, &status);

            // get the temperature distribution here ////////////////
            get_temperature_dist(arr_list, updated_arr_list, n_total, parts_list[r - 1], rank, cumm_parts_list[r - 1]);

            // send the computed array to rank 0 //////////////////////
            // (blocking) /////////////////////////////////////////////
            MPI_Send((void*)updated_arr_list, parts_list[r - 1] * n_total, MPI_DOUBLE, 0, 201, MPI_COMM_WORLD);
        }
        // free memory
        free(updated_arr_list);
        free(arr_list);
    }   
    free(parts_list);
    free(cumm_parts_list);
    MPI_Finalize();
    return 0;
}
