#ifndef _JAK_MATRIX_H_
#define _JAK_MATRIX_H_

#include <vector.h>

template<class T>
class matrix {
public:
	typedef vector<T>::size_type size_type;
private:
	typedef vector<vector<T> > _data_type;
	_data_type data;
public:
	matrix(void) {return;}
	matrix(const matrix<T>& m) : data(m.data) {return;}
	matrix(size_type r, size_type c, T x = T()) : data(r, vector<T>(c, x)) {
		return;
	}
	matrix(size_type r, const vector<T>& v) : data(r,v) {return;}
	~matrix(void) {return;}

	matrix<T>& operator=(const matrix<T>& m) {
		if(&m != this) data = m.data;
		return *this;
	}

	size_type rows(void) const {return data.size();}
	size_type cols(void) const {
		if(rows() == 0) return 0;
		return data.back().size();
	}
	T get(size_type r, size_type c) const {
		return data[r][c];
	}
	T operator()(size_type r, size_type c) const {return get(r,c);}
	vector<T> operator[](size_type i) const {return data[i];}
	vector<T>& operator[](size_type i) {return data[i];}
	T& get(size_type r, size_type c) {
		return data[r][c];
	}
	T& operator()(size_type r, size_type c) {return get(r,c);}

private:
	void _push_row(const vector<T>& v) {
		vector<T> temp(v);
		if(temp.size() < cols())
			temp.insert(temp.end(), cols()-temp.size(), T());
		else if(temp.size() > cols())
			for(size_type j=0; j<rows(); j++)
				data[j].insert(data[j].end(), temp.size()-cols(), T());
		data.push_back(temp);
		return;
	}

	void _push_col (const vector<T>& v) {
		vector<T> temp(v);
		if(temp.size() < rows())
			temp.insert(temp.end(), rows() - temp.size(), T());
		else if(temp.size() > rows())
			data.insert(data.end(), temp.size()-rows(), vector<T>());
		for(size_type i=0; i<rows(); i++) data[i].push_back(temp[i]);
		return;
	}

	void _pop_row(void) {data.pop_back(); return;}
	void _pop_col(void) {
		for(size_type i=0; i<data.size(); i++) data[i].pop_back();
		return;
	}
	
public:
	matrix<T>& push_row(const vector<T>& v) {
		_push_row(v);
		return *this;
	}
	matrix<T>& push_col(const vector<T>& v) {
		_push_col(v);
		return *this;
	}
	matrix<T>& pop_row(void) {
		_pop_row();
		return *this;
	}
	matrix<T>& pop_col(void) {
		_pop_col();
		return *this;
	}

	matrix<T>& augment(const vector<T>& v) {return push_col(v);}
	matrix<T>& augment(const matrix<T>& m) {
		for(unsigned long i=0; i<m.cols(); i++) _push_col(m.get_col(i));
		return *this;
	}


	vector<T> get_row(size_type i) const {return data[i];}
	vector<T> get_col(size_type i) const {
		vector<T> ret;
		for(size_type j=0; j<rows(); j++) ret.push_back(data[j][i]);
		return ret;
	}

	matrix<T>& transpose(void) {
		_data_type temp(data);
		data = _data_type();
		for(unsigned long i=0; i<temp.size(); i++) _push_col(temp[i]);
		return *this;
	}

	matrix<T>& swap_row(size_type i, size_type j) {
		vector<T> swap = data[i];
		data[i] = data[j];
		data[j] = swap;
		return *this;
	}
	
	matrix<T>& swap_col(size_type i, size_type j) {
		for(size_type k=0; k < data.size(); k++) {
			T swap = data[k][i];
			data[k][i] = data[k][j];
			data[k][j] = swap;
		}
	
		return *this;
	}
};


template <class T>
matrix<T> mul(const matrix<T>& a, const matrix<T>& b) {
	matrix<T> r(a.rows(), b.cols(), 0.0);
	for(unsigned long i=0; i<a.rows(); i++) {
		for(unsigned long j=0; j<b.cols(); j++) {
			for(unsigned long k=0; k<a.cols(); k++) r[i][j] += a[i][k]*b[k][j];
		}
	}
	return r;
}

template <class T>
vector<T> mul(const matrix<T>& a, const vector<T>& b) {
	vector<T> r(a.rows(), 0.0);
	for(unsigned long i=0; i<a.rows(); i++) 
		for(unsigned long j=0; j<a.cols(); j++) r[i] += a[i][j]*b[j];
	return r;
}

template <class T>
matrix<T> add(const matrix<T>& a, const matrix<T>& b) {
	matrix<T> r(a);
	for(unsigned long i=0; i<r.rows(); i++)
		for(unsigned long j=0; j<r.cols(); j++) r[i][j] += b[i][j];
	return r;
}

template <class T>
matrix<T> sub(const matrix<T>& a, const matrix<T>& b) {
	matrix<T> r(a);
	for(unsigned long i=0; i<r.rows(); i++)
		for(unsigned long j=0; j<r.cols(); j++) r[i][j] -= b[i][j];
	return r;
}

template <class T>
matrix<T> diag(const vector<T>& a) {
	matrix<T> r(a.size(), a.size(), T());
	for(unsigned long i=0; i<a.size(); i++) r[i][i] = a[i];
	return r;
}

template <class T>
matrix<T> transpose(const matrix<T>& m) {
	matrix<T> r(m);
	r.transpose();
	return r;
}

template <class T>
ostream& print(const matrix<T>& m, ostream& os) {
	for(unsigned long i=0; i<m.rows(); i++) {
		for(unsigned long j=0; j<m.cols(); j++) os << setw(15) << m[i][j];
		os << endl;
	}
	return os;
}

template <class T>
ostream& print(const vector<T>& m, ostream& os) {
	for(unsigned long i=0; i<m.size(); i++)  os << setw(15) << m[i] << endl;
	return os;
}

template <class T>
ostream& operator<<(ostream& os, const matrix<T>& m) {return print(m, os);}

template <class T>
ostream& operator<<(ostream& os, const vector<T>& v) {return print(v, os);}

#endif





