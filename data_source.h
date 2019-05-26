#ifndef ___DATA_SOURCE___
#define ___DATA_SOURCE___

template<typename Plaintext, typename Ciphertext>
class Communication;

template<class Plaintext, class Ciphertext>
class DataSource {
private:
	Matrix<Plaintext> _X;
	std::vector<Plaintext> _y;

	std::vector<Ciphertext> _Encw;

	Communication<Plaintext, Ciphertext> *_communication_channel;
public:
	DataSource() {}
	DataSource(const Matrix<Plaintext> &m, std::vector<Plaintext> &v) : _X(m), _y(v) {}

	void setCommunicationChannel(Communication<Plaintext, Ciphertext> *c) { _communication_channel = c; }

	void receive_w_from_server1(const std::vector<Ciphertext> &v) { _Encw = v; }

	void encode_data();
	void decrypt(std::vector<Plaintext> &w);
};

#endif
