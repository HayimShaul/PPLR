#ifndef ___SERVER2___
#define ___SERVER2___

template<class Plaintext, class Ciphertext>
class Server2 {
	Matrix<Ciphertext> _EncAprime;
	PackedMatrix<Ciphertext> _EncAprime_simd;
	std::vector<Ciphertext> _Encbprime;
public:
	Server2() {}

	Communication<Plaintext, Ciphertext> *_communication_channel;

	void setCommunicationChannel(Communication<Plaintext, Ciphertext> *c) { _communication_channel = c; }

	void receive_A_and_bfrom_server1(const Matrix<Ciphertext> &A, const PackedMatrix<Ciphertext> &A_simd, const std::vector<Ciphertext> &b) { _EncAprime = A; _EncAprime_simd = A_simd; _Encbprime = b; }

	void solve();

	template<class Bits>
	void toBits(Bits &bits, const Ciphertext &c) { bits = Bits::static_from_int(c.to_int()); }
};

#endif
