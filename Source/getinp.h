#ifndef _GETINP_H
#define _GETINP_H

#include "string"
#include <vector.h>
#include "convert.h"
#include <string.h>

// getinps return -1 if no data, 0 if error, 1 otherwise

int getinpvec(vector<string> *v, const char *prompt = "", bool dofill = true, unsigned long limit = 0, ostream& os = cout, istream& is = cin);

inline int getinpvec(vector<string> *v, const string& prompt = "", bool dofill = true, unsigned long limit = 0, 
	ostream& os = cout, istream& is = cin) {return getinpvec(v, prompt.c_str(), dofill, limit, os, is);}

template <class T>
int getinpvec(vector<T> *v, const char *prompt = "", bool dofill = true, unsigned long limit = 0, ostream& os = cout, istream& is = cin) {
	vector<string> vs;
	int r = getinpvec(&vs, prompt, dofill, limit, os, is);
	switch(r) {
		case -1:	*v = vector<T>();
					return -1;
		case  0:	return 0;
		case  1:	break;
	}
	if(convert(vs, v)) return 1;
	return 0;
}

int getinp(string *input, const char *prompt = "", ostream& os = cout, istream& is = cin);

int getinp(char *p, const char *prompt = "", ostream& os = cout, istream& is = cin, int count = 1);

template <class T>
int getinp(T *i, const char *prompt = "", ostream& os = cout, istream& is = cin) {
	string str;
	int t = getinp(&str, prompt, os, is);
	switch(t) {
		case -1:	*i = T();
					return -1;
		case 0:		return 0;
		case 1:		break;
	}
	if(convert(str, i)) return 1;
	return 0;
}

inline void errmess(const char *s, ostream& os = cout, istream& is = cin) {
	string t;
	os << s << endl;
	getinp(&t, "Press RETURN to continue...\n", os, is);
	return;
}

// getnoerrs loop until good data are input, return true if data, return false if no data

#define _ERRMESS_ "\n*** Error on input ***"

template <class T>
bool getinpvecnoerr(vector<T> *input, const char *prompt = "", bool dofill = true, 
	unsigned long limit = 0, const char *mess = _ERRMESS_, ostream& os = cout, istream& is = cin) {
	int r = 0;
	while(r == 0) {
		r = getinpvec(input, prompt, dofill, limit, os, is);
		if(r == 0) errmess(mess);
	}
	return (r == 1) ? true : false;
}

template <class T>
bool getinpnoerr(T *input, const char *prompt = "", const char *mess = _ERRMESS_, ostream& os = cout, istream& is = cin) {
	int r = 0;
	while(r == 0) {
		r = getinp(input, prompt, os, is);
		if(r == 0) errmess(mess);
	}
	return (r == 1) ? true : false;
}

inline bool ynquery(const char *prompt, const char *mess= _ERRMESS_, ostream& os = cout, istream& is = cin) {
	char c = '\0';
	while((c != 'y') && (c != 'n')) getinpnoerr(&c, prompt, mess, os, is);
	return (c == 'y') ? true : false;
}


#undef _ERRMESS_

#endif


