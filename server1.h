#ifndef ___SERVER1___
#define ___SERVER1___

#include <unsigned_word.h>
#include "packed_matrix.h"
#include "packed_vector.h"

template<class Plaintext, class Ciphertext>
class Server1 {
private:
	Matrix<Plaintext> _R;
	std::vector<Plaintext> _r;

	Matrix<Ciphertext> _X;
	std::vector<Ciphertext> _y;

	PackedMatrixSet<Ciphertext> _A_simd;
	PackedVector<Ciphertext> _b_simd;
	Matrix<Ciphertext> _A;
	std::vector<Ciphertext> _b;

	std::vector<Plaintext> _wprime;

	Communication<Plaintext, Ciphertext> *_communication_channel;
public:
	Server1() {}

	void setCommunicationChannel(Communication<Plaintext, Ciphertext> *c) { _communication_channel = c; }

	void receive_fraction_of_A_and_b(const Matrix<Ciphertext> &a, const PackedMatrixSet<Ciphertext> &a_simd, const std::vector<Ciphertext> &b, const PackedVector<Ciphertext> &b_simd);
	void receive_X_and_y_from_data_source(const Matrix<Ciphertext> &a, const std::vector<Ciphertext> &b) { _X = a; _y = b; }
	void receive_w_from_server2(const std::vector<Plaintext> &w) { _wprime = w; }

	void mask(const Matrix<Ciphertext> &X, const std::vector<Ciphertext> &y);
	void unmask(std::vector<Plaintext> &w);

	void linear_regression();
	void exhaustive_sparse_linear_regression();

	template<class Bits>
	void toBits(Bits &bits, const Ciphertext &val);
};

#endif
