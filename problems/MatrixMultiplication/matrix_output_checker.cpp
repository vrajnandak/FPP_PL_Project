#include<iostream>
#include<fstream>
#include<cstdlib>
#include<vector>

using namespace std;


//Function to read matrix from a file.
vector<vector<int>> read_matrix(const string &filename)
{
	ifstream file(filename);
	if(!file)
	{
		cerr << "Error opening file for reading: " << filename << endl;
		exit(1);
	}

	int rows, cols;
	file >> rows >> cols;								//Reading the matrix size.

	vector<vector<int>> matrix(rows, vector<int>(cols));

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			if(!(file >> matrix[i][j]))					//Reading the elements of the matrix.
			{
				cerr << "Error reading from file " << filename << " at element " << i << ", " << j << endl;
				exit(1);
			}
		}
	}

	file.close();
	return matrix;
}


//Function to perform element-wise comparision of 2 matrices.
int compare_matrices(vector<vector<int>> mat1, vector<vector<int>> mat2)
{
	if(mat1.size()!=mat2.size() || mat1[0].size()!=mat2[0].size())
	{
		cout << "Mismatch in sizes of matrices" << endl;
		return -1;
	}

	for(int i=0;i<mat1.size();i++)
	{
		for(int j=0;j<mat1[0].size();j++)
		{
			if(mat1[i][j]!=mat2[i][j])
			{
				cout << "Matrices differ at (" << i << ", " << j << "): " << mat1[i][j] << " != " << mat2[i][j] << endl;
				return -1;
			}
		}
	}
	return 0;

}


//Function to perform comparisions of the resultant product of the matrix multiplication from the various algorithms used.
void compare_outputs(vector<string> filenames)
{
	//Requires the presence of all the output files.
	vector<vector<vector<int>>> matrices;
	for(int i=0;i<filenames.size();i++)
	{
		matrices.push_back(read_matrix(filenames[i]));
	}


	for(int i=1;i<filenames.size();i++)
	{
		cout << "Checking files: " << filenames[0] << " and " << filenames[i] << endl;
		auto comp_val = compare_matrices(matrices[0],matrices[i]);
		if(comp_val!=0)
		{
			cout << "Matrices 0 and " << i << " are not the same" << endl;
			exit(1);
		}
		cout << "No mismatch found" << endl;
	}
}

int main()
{
	vector<string> filenames;
	filenames.push_back("OCaml_mat_mul_partial_eval.txt");
	filenames.push_back("OCaml_mat_mul_partial_eval_parallel.txt");
	filenames.push_back("OCaml_mat_mul_partial_eval_parallel_domainslib.txt");
	filenames.push_back("OCaml_mat_mul_seq.txt");
	filenames.push_back("imp_seq.txt");
	filenames.push_back("imp_par.txt");
	filenames.push_back("imp_seq_opt.txt");
	filenames.push_back("imp_par_opt.txt");

	compare_outputs(filenames);

	return 0;
}
