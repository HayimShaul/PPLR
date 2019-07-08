#ifndef ___PACKED_VECTOR___
#define ___PACKED_VECTOR___

template<class Number>
class PackedVector {
private:
	std::vector<Number> _ctxt;
	int _size;
public:
	PackedVector() {}

	PackedVector<Number> &operator=(const PackedVector<Number> &v) {
		_size = v._size;
		_ctxt.resize(v._ctxt.size());
		for (unsigned int i = 0; i < _ctxt.size(); ++i) {
			_ctxt[i] = v._ctxt[i];
		}
		return *this;
	}

	unsigned int size() const { return _size; }

	long to_int(int i) const {
		int simd_factor = _ctxt[0].simd_factor();
		int i_ctxt = i / simd_factor;
		std::vector<long> a = _ctxt[i_ctxt].to_vector();
		return a[i - i_ctxt * simd_factor];
	}

	template<class Out>
	void to_vector(std::vector<Out> &o) {
		o.resize(size());
		int simd_factor = _ctxt[0].simd_factor();
		int i_ctxt = -1;
		int i_slot = simd_factor;
		std::vector<long> data;

		for (unsigned int i = 0; i < size(); ++i) {
			if (i_slot == simd_factor) {
				i_slot = 0;
				++i_ctxt ;
				data = _ctxt[i_ctxt].to_vector();
			}

			o[i].from_int(data[i_slot]);
			++i;
		}
	}

	template<class In>
	void init_vector(const std::vector<In> &m) {
		Number temp;

		std::vector<long> serialVec(temp.simd_factor());
		_ctxt.resize((m.size() + temp.simd_factor() - 1) / temp.simd_factor());
		_size = m.size();

		unsigned int i_simd = 0;
		unsigned int i_ctxt = 0;
		for (unsigned int i = 0; i < m.size(); ++i) {
			serialVec[i_simd] = m[i].to_int();
			++i_simd;
			if (i_simd == temp.simd_factor()) {
				i_simd = 0;
				_ctxt[i_ctxt].from_vector(serialVec);
				++i_ctxt;
			}
		}

		if (i_simd != 0) {
			while (i_simd < temp.simd_factor()) {
				serialVec[i_simd] = 0;
				++i_simd;
			}
			_ctxt[i_ctxt].from_vector(serialVec);
		}
	}

	void operator+=(const PackedVector<Number> &m) {
		assert(_size == m._size);
		assert(_ctxt.size() == m._ctxt.size());

		for (unsigned int i = 0; i < _ctxt.size(); ++i) {
			_ctxt[i] += m._ctxt[i];
		}
	}

	template<class N>
	friend void add(PackedVector<N> &o, const PackedMatrix<N> &a, const PackedVector<N> &b);
};


template<class Number>
void add(PackedVector<Number> &o, const PackedMatrix<Number> &a, const PackedVector<Number> &b) {
	assert(b.size() == a.cols());

	o._ctxt.resize(b._ctxt.size());
	o._size = b._size;
	for (unsigned int i = 0; i < o._ctxt.size(); ++i)
		o._ctxt[i] = a._ctxt[i] + b._ctxt[i];
}

template<class Number>
std::ostream &operator<<(std::ostream &out, const PackedVector<Number> &m) {
	for (unsigned int i = 0; i < m.size(); ++i) {
		out << m.to_int(i) << " ";
	}
	return out;
}


#endif
