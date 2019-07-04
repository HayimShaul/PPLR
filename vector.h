
template<class Number>
void draw(std::vector<Number> &v) {
	static int no_rand = 1;
	for (unsigned int i = 0; i < v.size(); ++i) {
		int r = random();
		r = no_rand++;
		v[i].from_int(r);
	}
}

template<class Number>
void operator+=(std::vector<Number> &v1, const std::vector<Number> &v2) {
	assert(v1.size() == v2.size());

	for (unsigned int i = 0; i < v1.size(); ++i)
		v1[i] += v2[i];
}


template<class Number>
std::vector<Number> operator-(const std::vector<Number> &v1, const std::vector<Number> &v2) {
	assert(v1.size() == v2.size());

	std::vector<Number> ret;
	ret.resize(v1.size());

	for (unsigned int i = 0; i < v1.size(); ++i)
		ret[i] = v1[i] - v2[i];

	return ret;
}

template<class Out, class In1, class In2>
void add(std::vector<Out> &out, const std::vector<In1> &v1, const std::vector<In2> &v2) {
	assert(v1.size() == v2.size());
	assert((void*)&out != (void*)&v1);
	assert((void*)&out != (void*)&v2);

	for (unsigned int i = 0; i < v1.size(); ++i)
		out[i] = v1[i] + v2[i];
}

template<class Out, class In1>
void add(std::vector<Out> &out, std::vector<In1> &v1) {
	assert(out.size() == v1.size());
	assert((void*)&out != (void*)&v1);

	for (unsigned int i = 0; i < v1.size(); ++i)
		out[i] += v1[i];
}

template<class Out, class In1>
void sub(std::vector<Out> &out, std::vector<In1> &v1) {
	assert(out.size() == v1.size());
	assert((void*)&out != (void*)&v1);

	for (unsigned int i = 0; i < v1.size(); ++i)
		out[i] -= v1[i];
}

template<class Number>
std::ostream &print(std::ostream &s, const std::vector<Number> &v) {
	for (unsigned int i_row = 0; i_row < v.size(); ++i_row)
		s << v[i_row].to_int() << " ";
	s << std::endl;
	return s;
}

//template<class Number>
//std::ostream &operator<<(std::ostream &s, const std::vector<Number> &v) { return print(s,v); }

