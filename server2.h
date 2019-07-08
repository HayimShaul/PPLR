#ifndef ___SERVER2___
#define ___SERVER2___

template<class Plaintext, class Ciphertext>
class Server2 {
	PackedMatrix<Ciphertext> _EncAprime;
	PackedVector<Ciphertext> _Encbprime;
public:
	Server2() {}

	Communication<Plaintext, Ciphertext> *_communication_channel;

	void setCommunicationChannel(Communication<Plaintext, Ciphertext> *c) { _communication_channel = c; }

	void receive_A_and_bfrom_server1(const PackedMatrix<Ciphertext> &A, const PackedVector<Ciphertext> b) { _EncAprime = A; _Encbprime = b; }

	void solve();

	template<class Bits>
	void toBits(Bits &bits, const Ciphertext &c) { bits = Bits::static_from_int(c.to_int()); }
};

#endif
