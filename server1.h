#ifndef ___SERVER1___
#define ___SERVER1___

#include <unsigned_word.h>

template<class Plaintext, class Ciphertext>
class Server1 {
private:
	Matrix<Plaintext> _R;
	std::vector<Plaintext> _r;

	Matrix<Ciphertext> _X;
	std::vector<Ciphertext> _y;

	std::vector<Ciphertext> _wprime;
	Matrix<Ciphertext> _Aprimeinv;

	Communication<Plaintext, Ciphertext> *_communication_channel;
public:
	Server1() {}

	void setCommunicationChannel(Communication<Plaintext, Ciphertext> *c) { _communication_channel = c; }

	void receive_X_and_y_from_data_source(const Matrix<Ciphertext> &a, const std::vector<Ciphertext> &b) { _X = a; _y = b; }
	void receive_w_and_A_from_server2(const std::vector<Ciphertext> &w, const Matrix<Ciphertext> &A) { _wprime = w; _Aprimeinv = A; }

	void mask(const Matrix<Ciphertext> &X, const std::vector<Ciphertext> &y);
	void unmask(const Matrix<Ciphertext> &X, const std::vector<Ciphertext> y, std::vector<Ciphertext> &Encw);

	void linear_regression();
	void exhaustive_sparse_linear_regression();

	template<class Bits>
	void toBits(Bits &bits, const Ciphertext &val);
};

#endif
