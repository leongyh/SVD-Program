#ifndef _CONVERT_H
#define _CONVERT_H

#include <vector.h>
#include <stdlib.h>
#include "string"

// converts false if error, true otherwise
// No data in string is treated as 0

inline bool convert(const char *s, unsigned long *i) {
	if(*s == '\0') *i = 0UL;
	else {
		char *p;
		*i = strtoul(s, &p, 10);
		if(*p != '\0') return false;
	}
	return true;
}

inline bool convert(const char *s, long *i) {
	if(*s == '\0') *i = 0L;
	else {
		char *p;
		*i = strtol(s, &p, 10);
		if(*p != '\0') return false;
	}
	return true;
}

inline int convert(const char *s, int *i) {
	long j;
	bool r = convert(s, &j);
	*i = int(j);
	return r;
}

inline int convert(const char *s, double *i) {
	if(*s == '\0') *i = 0.0;
	else {
		char *p;
		*i = strtod(s, &p);	
		if(*p != '\0') return false;
	}
	return true;
}

inline int convert(const char *s, float *i) {
	double j;
	bool r = convert(s, &j);
	*i = float(j);
	return r;
}

template <class T>
bool convert(const string& s, T *i) {	
	return convert(s.c_str(), i);
}

bool convert(unsigned long i, string *s, int len = 0, char pad = '0');
bool convert(long i, string *s, int len = 0, char pad = '0');
bool convert(double i, string *s, int precision = 0, char format = 'd', int width = 0); // format: d = default, f = fixed, s = scientific
inline bool convert(int i, string *s, int len = 0, char pad = '0') {return convert(long(i), s, len, pad);}

inline bool convert(float i, string *s, int precision = 0, char format = 'd', int width = 0) {
	return convert(double(i), s, precision, format, width);
}



inline bool convert(double x, float *y) {*y = float(x); return true;}
inline bool convert(float x, double *y) {*y = double(x); return true;}
inline bool convert(long x, int *y) {*y = int(x); return true;}
inline bool convert(int x, long *y) {*y = long(x); return true;}
inline bool convert(unsigned long x, long *y) {*y = long(x); return true;}
inline bool convert(long x, unsigned long *y) {*y = (unsigned long)(x); return true;}

template <class T, class U>
bool convert(const vector<T>& a, vector<U> *bb) {	
	vector<U> &b = *bb;
	b = vector<U>();
	for(unsigned long i=0; i<a.size(); i++) {
		U x;
		if(!convert(a[i], &x)) return false;
		b.push_back(x);
	}
	return true;
}

inline string numtostr(unsigned long i, int len = 0, char pad = '0') {
	string s;
	convert(i, &s, len, pad);
	return s;
}

inline string numtostr(long i, int len = 0, char pad = '0') {
	string s;
	convert(i, &s, len, pad);
	return s;
}

inline string numtostr(int i, int len = 0, char pad = '0') {return numtostr(long(i), len, pad);}

inline string numtostr(double i, int precision = 0, char format = 'd', int width = 0) {
	string s;
	convert(i, &s, precision, format, width);
	return s;
}

inline string numtostr(float i, int len = 0, char pad = '0') {return numtostr(double(i), len, pad);}

inline vector<double> vecftod(const vector<float>& v) {
	vector<double> r;
	convert(v, &r);
	return r;
}

inline vector<float> vecdtof(const vector<double>& v) {
	vector<float> r;
	convert(v, &r);
	return r;
}

#endif



