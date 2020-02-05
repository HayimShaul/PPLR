#include <vector>
#include <istream>
#include <fstream>
#include <boost/tokenizer.hpp>

//#include <zp.h>
#include <helib_number.h>
#include <helib_keys.h>
#include <zp.h>

#include "matrix.h"
#include "vector.h"
#include "crt.h"

#include "times.h"

#include "data_source.h"
#include "server1.h"
#include "server2.h"

#include "communication.h"

#include "protocol.h"

typedef HelibNumber Ciphertext;
typedef ZP Plaintext;
//typedef UnsignedWord<7, Ciphertext> Bits;
//typedef int Plaintext;

#include "linear_regression.h"

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
	unsigned long p = 101;
	int primeNumber = 1;
	std::string in("");
	int dim = 4;
	int lines = 20;
	long L = 5;

	for (int argc_i = 0; argc_i < argc; ++argc_i) {
		if (memcmp(argv[argc_i], "--L=", 4) == 0)
			L = atoi(argv[argc_i] + 4);
		if (memcmp(argv[argc_i], "--p=", 4) == 0)
			p = atol(argv[argc_i] + 4);
		if (memcmp(argv[argc_i], "--n=", 4) == 0)
			primeNumber = atoi(argv[argc_i] + 4);
		if (memcmp(argv[argc_i], "--d=", 4) == 0)
			dim = atoi(argv[argc_i] + 4);
		if (memcmp(argv[argc_i], "--l=", 4) == 0)
			lines = atoi(argv[argc_i] + 4);
		if (memcmp(argv[argc_i], "--in=", 5) == 0)
			in = std::string(argv[argc_i] + 5);

		if (strcmp(argv[argc_i], "--help") == 0) {
			std::cout << "   --L=5 level of key" << std::endl;
			std::cout << "   --p=101 first prime" << std::endl;
			std::cout << "   --n=1 number of primes for CRT" << std::endl;
			std::cout << "   --in= input file (blank means random)" << std::endl;
			std::cout << "   --d=4 dimension of model. If input is specified, d cannot be bigger than dimension of input" << std::endl;
			std::cout << "   --l=4 number of lines. If input is specified, l cannot be bigger than lines of input" << std::endl;
			exit(1);
		}
	}


	Matrix<float> X;
	std::vector<float> y;
	std::vector<float> model(dim);

	if (in != std::string("")) {
		read_csv_file(in, X, y);
	} else {
		std::cout << "The real model is:";
		for (int i_col = 0; i_col < dim; ++i_col) {
			model[i_col] = myrand(0,5);
			std::cout << " " << model[i_col];
		}
		std::cout << std::endl;

		X.resize(dim, lines);
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

	unsigned long prime;
	if (p < 1000000)
		prime = Primes::find_prime_bigger_than(p-1);
	else
		prime = p;

	ThreadPool pool;

	for (int i_prime = 0; i_prime < primeNumber; ++i_prime) {
		ringSize *= prime;

		pool.submit_job(std::function<void(void)>([prime, i_prime](){
			std::cout << "prime " << i_prime << " = " << prime << std::endl;

			// generate keys
			HelibKeys keys;
			long R = 1;
			long p = prime;
			long r = 1;
			long d = 1;
			long c = 2;
			long k = 80;
			long s = 0;
			long chosen_m = 0;
			Vec<long> gens;
			Vec<long> ords;

			Times::start_phase1_step1();
			keys.initKeys(s, R, p, r, d, c, k, 64, L, chosen_m, gens, ords);

			thread_data[pthrea_self()].keys = &keys;
			thread_data[pthrea_self()].simd_factor = keys.simd_factor();
			thread_data[pthrea_self()].p = p;

			TODO: write the get_prime, get_keys functions, get_simd_factor
//			HelibNumber::set_global_keys(&keys);
//			Plaintext::set_global_p(p);
//			Plaintext::set_global_simd_factor(keys.simd_factor());
			Times::end_phase1_step1();

			try {
				std::vector<int> linRegModelInt;

				linRegModelInt = linearRegression(X, y);

				pthread_mutex_lock(&outputModel);
				for (unsigned int i_col = 0; i_col < X.cols(); ++i_col) {
					crtDigitsOfModel[i_col].push_back(CrtDigit(linRegModelInt[i_col], prime));
				}
				pthread_mutex_unlock(&outputModel);

			} catch (Matrix<Plaintext>::InverseRuntimeError &e) {
				std::cout << "prime " << prime << " gives a singular matrix. Skipping ..." << std::endl;
			}

		}));

		prime = Primes::find_prime_bigger_than(prime+1);
	}

	pool.process_jobs();

	std::cout << "computed model after CRT = ";
	std::vector<int> modelCrtDecoded(X.cols());
	for (unsigned int i_col = 0; i_col < X.cols(); ++i_col) {
		modelCrtDecoded[i_col] = crtDecode(crtDigitsOfModel[i_col]);

		std::cout << ((!i_col)?"":", ") << modelCrtDecoded[i_col];
	}
	std::cout << std::endl;

	bool ok = true;

	std::cout << "computed model after rational recontruction = ";
	for (unsigned int i = 0; i < modelCrtDecoded.size(); ++i) {
		int num, denom;
		Times::start_phase2_step3();
		rational_reconstruction(num, denom, modelCrtDecoded[i], ringSize);
		Times::end_phase2_step3();
		std::cout << ((!i)?"":", ");
		std::cout << num << "/" << denom;

		if (abs(modelCrtDecoded[i] - model[i]) > 0.3)
			ok = false;
	}
	std::cout << std::endl;


	Times::print(std::cout);

	if (ok)
		std::cout << "OK" << std::endl;
	else
		std::cout << "FAIL" << std::endl;

	return 0;
}
