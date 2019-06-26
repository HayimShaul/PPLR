#include <vector>
#include <istream>
#include <fstream>
#include <algorithm>
#include <boost/tokenizer.hpp>

#include <zp.h>

#include "matrix.h"
#include "vector.h"
#include "crt.h"

#include "data_source.h"
#include "server1.h"
#include "server2.h"

#include "communication.h"

#include "protocol.h"

typedef ZP<1> Ciphertext;
typedef ZP<1> Plaintext;
typedef UnsignedWord<7, Ciphertext> Bits;
//typedef int Plaintext;

#include "linear_regression.h"

//int main2(int, char**) {
//	Ciphertext::set_global_p(101);
//
//	Matrix<Plaintext> X;
//	std::vector<Plaintext> y;
//
//	DataSource<Plaintext, Ciphertext> dataSource(X, y);
//	Server1<Plaintext, Ciphertext> server1;
//	Server2<Plaintext, Ciphertext> server2;
//
//	Communication<Plaintext, Ciphertext> communication(dataSource, server1, server2);
//
//	Bits bits;
//	Ciphertext c(50);
//
//	server1.toBits(bits, c);
//
//	for (int i = 5; i >= 0; --i) {
//		std::cout << bits[i].to_int() << " ";
//	}
//	std::cout << std::endl;
//	return 0;
//}

unsigned int countLines(const std::string &fname) {
	std::ifstream inFile(fname); 

	if (!inFile)
		throw std::runtime_error(std::string("can't open file ") + fname);
//	std::string line;
//	int count = 0;
//	while (getline(inFile, line))
//		++count;
//
//	return count;
	return std::count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n');
}

std::vector<float> parseCSVLine(std::string line){
	using namespace boost;

	std::vector<float> vec;

	// Tokenizes the input string
	tokenizer<escaped_list_separator<char> > tk(line, escaped_list_separator<char> ('\\', ',', '\"'));
	for (auto i = tk.begin();  i!=tk.end();  ++i)
		vec.push_back(std::stof(*i));

	return vec;
}


// read a csv file given as x,y,c   where c=0,1 is the class of a point and (x,y) is the coordinates
template<class Num>
void read_csv_file(const std::string &fname, Matrix<Num> &m, std::vector<Num> &v) {
	std::istream *in;

	if (fname == "-") {
		in = &(std::cin);
	} else {
		in = new std::ifstream(fname);
	}

	std::string line;
	std::vector<float> l;

	int nlines = countLines(fname);
	std::getline(*in, line);	// first line is headers
	std::getline(*in, line);
	l = parseCSVLine(line);
	unsigned int dim = l.size() - 1;
	m.resize(dim, nlines-1);
	v.resize(nlines-1);
	int i_line = 0;
	while (l.size() > 0) {
		if (l.size() != dim+1) {
			throw std::runtime_error(std::string("Error while reading CSV file. Lines have different length"));
		}
		for (unsigned int col = 0; col < dim; ++col)
			m(col, i_line) = Num(int(l[col]));
		v[i_line] = Num(int(l[dim]));
		std::getline(*in, line);
		l = parseCSVLine(line);
		++i_line;
	}

	if (in != &(std::cin))
		delete in;
}


int myrand(int min, int max) {
	return min + (random() % (max - min));
}


int main(int argc, char **argv) {
	int p = 101;
	int primeNumber = 1;
	std::string in("");

	for (int argc_i = 0; argc_i < argc; ++argc_i) {
		if (memcmp(argv[argc_i], "--p=", 4) == 0)
			p = atoi(argv[argc_i] + 4);
		if (memcmp(argv[argc_i], "--n=", 4) == 0)
			primeNumber = atoi(argv[argc_i] + 4);
		if (memcmp(argv[argc_i], "--in=", 5) == 0)
			in = std::string(argv[argc_i] + 5);

		if (strcmp(argv[argc_i], "--help") == 0) {
			std::cout << "   --p=101 first prime" << std::endl;
			std::cout << "   --n=1 number of primes for CRT" << std::endl;
			std::cout << "   --in= input file (blank means random)" << std::endl;
		}
	}


	Matrix<float> X;
	std::vector<float> y;

	if (in != std::string("")) {
		read_csv_file(in, X, y);
	} else {
		int lines = 20;
		int cols = 3;
		std::vector<float> model(cols);
		std::cout << "The real model is:";
		for (int i_col = 0; i_col < cols; ++i_col) {
			model[i_col] = myrand(0,5);
			std::cout << " " << model[i_col];
		}
		std::cout << std::endl;

		X.resize(cols, lines);
		for (unsigned int i = 0; i < X.cols(); ++i) {
			for (unsigned int j = 0; j < X.rows(); ++j) {
				X(i,j) = myrand(0,10);
			}
		}

		y.resize(lines);
		for (unsigned int j = 0; j < X.rows(); ++j) {
			y[j] = 0;
			for (unsigned int i = 0; i < X.cols(); ++i) {
				y[j] += X(i,j) * model[i];
			}
		}
	}


	std::vector<std::vector<CrtDigit> > crtDigitsOfModel(X.cols());
	int ringSize = 1;

	int prime = Primes::find_prime_bigger_than(p-1);
	for (int i_prime = 0; i_prime < primeNumber; ++i_prime) {
		std::cout << "prime " << i_prime << " = " << prime << std::endl;
		ringSize *= prime;
		Ciphertext::set_global_p(prime);

		try {
			std::vector<int> linRegModelInt;
			linRegModelInt = linearRegression(X, y);

			for (unsigned int i_col = 0; i_col < X.cols(); ++i_col) {
				crtDigitsOfModel[i_col].push_back(CrtDigit(linRegModelInt[i_col], prime));
			}
		} catch (Matrix<Plaintext>::InverseRuntimeError &e) {
			std::cout << "prime " << prime << " gives a singular matrix. Skipping ..." << std::endl;
		}

		prime = Primes::find_prime_bigger_than(prime+1);
	}

	std::cout << "computed model after CRT = ";
	std::vector<int> modelCrtDecoded(X.cols());
	for (unsigned int i_col = 0; i_col < X.cols(); ++i_col) {
		modelCrtDecoded[i_col] = crtDecode(crtDigitsOfModel[i_col]);

		std::cout << ((!i_col)?"":", ") << modelCrtDecoded[i_col];
	}
	std::cout << std::endl;

	std::cout << "computed model after rational recontruction = ";
	for (unsigned int i = 0; i < modelCrtDecoded.size(); ++i) {
		int num, denom;
		rational_reconstruction(num, denom, modelCrtDecoded[i], ringSize);
		std::cout << ((!i)?"":", ");
		std::cout << num << "/" << denom;
	}
	std::cout << std::endl;

	return 0;
}
