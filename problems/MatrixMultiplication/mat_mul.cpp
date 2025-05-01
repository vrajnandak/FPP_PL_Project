#include<iostream>
#include<fstream>
#include<vector>
#include<cstdlib>
#include<cstring>
#include<omp.h>

using namespace std;

//Function to write the given matrix to a file.
void write_matrix_to_file(vector<vector<int>> &mat, string &filename)
{
	ofstream out(filename);
	if(!out.is_open())
	{
		throw std::runtime_error("Failed to open file.");
	}

	int rows=mat.size();
	int cols = mat[0].size();
	out << rows << " " << cols << "\n";					//Writing the matrix size.

	for(const auto &row: mat)
	{
		for(int val: row)
		{
			out << val << " ";					//Writing the elements of the matrix.
		}
		out << "\n";
	}
	out.close();
}


//Function to read a matrix from a file.
vector<vector<int>> read_matrix(const string &filename)
{
	ifstream file(filename);
	if(!file)
	{
		cerr << "Error opening file for reading: " << filename << endl;
		exit(1);
	}

	int rows, cols;
	file >> rows >> cols;							//Reading the matrix size.

	vector<vector<int>> matrix(rows, vector<int>(cols));

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			if(!(file >> matrix[i][j]))				//Reading the elements of the matrix.
			{
				cerr << "Error reading from file " << filename << " at element " << i << ", " << j << endl;
				exit(1);
			}
		}
	}

	file.close();
	return matrix;
}

/////////////////////////// Regular sequential matrix multiplication.////////////////
vector<vector<int>> matrix_mul(vector<vector<int>> &a, vector<vector<int>> &b)
{
	int rows_a = a.size();
	int cols_b = b[0].size();
	int cols_a = a[0].size();

	vector<vector<int>> result(rows_a, vector<int>(cols_b));

	for (int i = 0; i < rows_a; ++i)
	{
		for(int j=0;j<cols_b;j++)
		{
			for(int k=0;k<cols_a;k++)
			{
				result[i][j]+=a[i][k]*b[k][j];
			}
		}
	}

	return result;
}

/////////////////////////// Regular sequential matrix multiplication - is optimized for better cache access rates.////////////////
vector<vector<int>> matrix_mul_seq_opt(vector<vector<int>> &a, vector<vector<int>> &b)
{
	int rows_a = a.size();
	int cols_b = b[0].size();
	int cols_a = a[0].size();

	vector<vector<int>> result(rows_a, vector<int>(cols_b));

	for (int i = 0; i < rows_a; ++i)
	{
		for(int k=0;k<cols_a;k++)
		{
			int tmp = a[i][k];					//Reduces memory access
			for(int j=0;j<cols_b;j++)
			{
				result[i][j]+=tmp*b[k][j];			//Reads are from closely present memory locations.
			}
		}
	}

	return result;
}

//////////////////////////////////// Parallelised matrix multiplication ///////////////////////////////////////
vector<vector<int>> matrix_mult_parallel(vector<vector<int>> &a, vector<vector<int>> &b)
{
	int rows_a=a.size();
	int cols_b=b[0].size();
	int cols_a=a[0].size();

	vector<vector<int>> result(rows_a, vector<int>(cols_b));

#pragma omp parallel for collapse(2) 						//Parallelising both i and j loops.
	for (int i = 0; i < rows_a; ++i)
	{
		for (int j = 0; j < cols_b; ++j)
		{
			int sum = 0;						//Cannot be parallelised due to race condition.
			for (int k = 0; k < cols_a; ++k)
			{
				sum += a[i][k] * b[k][j];
			}
			result[i][j] = sum;
		}
	}

	return result;
}

//////////////////////////////////// Parallelised matrix multiplication - using b transpose + parallelism ///////////////////////////////////////
vector<vector<int>> transposeMatrix(const vector<vector<int>> &matrix)
{
	int rows=matrix.size();
	int cols=matrix[0].size();
	vector<vector<int>> transposed(cols,vector<int>(rows));
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			transposed[j][i]=matrix[i][j];
		}
	}
	return transposed;
}

vector<vector<int>> matrix_mult_parallel_opt(vector<vector<int>> &a, vector<vector<int>> &b)
{
	int rows_a=a.size();
	int cols_b=b[0].size();
	int cols_a=a[0].size();
	auto b_transposed=transposeMatrix(b);
	vector<vector<int>> result(rows_a, vector<int>(cols_b));

#pragma omp parallel for collapse(2)
	for(int i=0;i<rows_a;i++)
	{
		for(int j=0;j<cols_b;j++)
		{
			int sum=0;
			for(int k=0;k<cols_a;k++)
			{
				sum += a[i][k] * b_transposed[j][k];
			}
			result[i][j] = sum;
		}
	}

	return result;
}


int main(int argc, char *argv[])
{
	auto a = read_matrix("matrix_a.txt");
	auto b = read_matrix("matrix_b.txt");

	if(argc!=3)
	{
		cerr << "Usage: " << argv[0] << " <matrix_mult_type> <validate_outputs> : matrix_mult_type can be 'seq' or 'par', validate_outputs can be 'yes' or 'no'" << endl;
		exit(1);
	}

	vector<vector<int>> result;
	string filename="imp_seq.txt";

	if(strcmp(argv[1],"seq")==0)
	{
		result = matrix_mul(a,b);
	}
	else if(strcmp(argv[1], "seq_opt")==0)
	{
		result = matrix_mul_seq_opt(a,b);
		filename="imp_seq_opt.txt";
	}
	else if(strcmp(argv[1], "par")==0)
	{
		result = matrix_mult_parallel(a,b);
		filename="imp_par.txt";
	}
	else if(strcmp(argv[1], "par_opt")==0)
	{
		result = matrix_mult_parallel_opt(a,b);
		filename="imp_par_opt.txt";
	}
	else
	{
		cerr << "Usage: " << argv[0] << " <matrix_mult_type> <validate_outputs> : matrix_mult_type can be 'seq' or 'par', validate_outputs can be 'yes' or 'no'" << endl;
		exit(1);
	}

	if(strcmp(argv[2],"yes")==0)
	{
		write_matrix_to_file(result, filename);
	}

	return 0;
}
