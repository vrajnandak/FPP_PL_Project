#include<iostream>
#include<fstream>
#include<random>
#include<cstdlib>

using namespace std;

//Functino to randomly create a matrix of given matrix size - elements are all less than 5 having taken into consideration the matrix sizes.
void generate_matrix(const string &filename, int rows, int cols)
{
	ofstream file(filename);
	if(!file)
	{
		cerr << "Error opening file: " << filename << endl;
		exit(1);
	}

	file << rows << " " << cols << "\n";

	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
		{
			int num= 1 + rand()%5;
			file << num << " ";
		}
		file << "\n";
	}

	file.close();
}

int main(int argc, char *argv[])
{
	cout << "argc: " << argc << endl;
	if(argc!=5)
	{
		cerr << "Usage: " << argv[0] << " rows_A cols_A rows_B cols_B \n";
		exit(1);
	}

	int rows_A=atoi(argv[1]);
	int cols_A=atoi(argv[2]);
	int rows_B=atoi(argv[3]);
	int cols_B=atoi(argv[4]);

	srand(time(nullptr));

	generate_matrix("matrix_a.txt",rows_A,cols_A);
	generate_matrix("matrix_b.txt",rows_B,cols_B);

	cout << "Matrices generated: a.txt (" << rows_A << "x" << cols_A << "), " << "b.txt (" << rows_B << "x" << cols_B << ")\n";

	return 0;
}
