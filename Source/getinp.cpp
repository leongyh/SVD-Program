#include "getinp.h"
#include <iostream.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <vector.h>
#include <algo.h>
#include "string"

#define ELLIPSIS string("..")
#define WHITE_SPACE " \t\n"

void prslin(const string& line, vector<string> *input) {
	string str = line;
	
	char *p = str.c_str();
	p = strtok(p, WHITE_SPACE);
	while(p) {
		(*input).push_back(string(p));
		p = strtok(NULL, WHITE_SPACE);
	}
	
	return;
}

bool nextstr(string& str, long place) {
	if(place == -1L) return false;
	char *c = &(str[place]);

	if(*c == '9') *c = '0';
	else if(*c == 'Z') *c = 'A';
	else if(*c == 'z') *c = 'a';
	else if(isalnum(*c)) {
		(*c)++;
		return true;
	}

	return nextstr(str, place-1);
}


inline bool nextstr(string& str) {
	return nextstr(str, str.length());
}

bool chkfillfrmt(const string& a, const string& b) {
	if(a.length() != b.length()) return false;
	for(unsigned long i=0; i<a.length(); i++) {
		if(a[i] == b[i]) continue;
		const char ai = a[i];
		const char bi = b[i];
		if(isdigit(ai) && isdigit(bi)) continue;
		if(islower(ai) && islower(bi)) continue;
		if(isupper(ai) && isupper(bi)) continue;
		return false;
	}
	if(a.compare(b) < 0) return true;
	return false;
}

string substr(const string& s, unsigned long start, unsigned long stop) {
	string r("");	
	for(unsigned long i=start; i<stop; i++) r += s[i];
	r += s[stop];
	return r;
}

unsigned long calcstr(const string& a) {
	unsigned long r = 0;
	for(unsigned long i=0; i< a.length(); i++) {
		if(isdigit(a[i])) {
			r *= 10;
			r += a[i] - '0';
		} else if(isalpha(a[i])) {
			r *= 26;
			r += a[i] - ((islower(a[i])) ? 'a' : 'A');
		}
	}
	return r;
}

long chkambig(const string& a, const string& b, ostream& os, istream& is) {
	vector<long> v;
	for(long i = a.length(); i >= 0; i--) {
		if(isalnum(a[i]) && isalnum(b[i])) {
			v.push_back(i);
			if(a[i] != b[i]) break;
		}
	}
	
	if(v.size() == 0) return -1;
	if(v.size() == 1) return v[0];
		
	
	os << "Ambiguous range.  Please indicate your intention:" << endl;
	os << "0: Exit" << endl;
	for(unsigned long j=0; j<v.size(); j++) {
		const long n1 = calcstr(substr(a, 0, v[j]));
		const long n2 = calcstr(substr(b, 0, v[j]));
		const long num = n2 - n1 + 1;
		os << j+1 << ": Iterate on character " << v[j]+1 << " (" << num << " entries)" << endl;
	}
	
	int choice = -1;
	while((choice < 0) || (choice > v.size())) {
		if(!getinp(&choice, "Enter your choice: ", os, is)) choice = -1;
	}
	
	if(choice == 0) return -1;
	return v[choice-1];
}

bool fillit(const string& a, const string& b, vector<string>& v, ostream& os, istream& is) {
	v = vector<string>();
	if(!chkfillfrmt(a,b)) return false;
	long place = chkambig(a, b, os, is);
	if(place < 0) return false;
	string str = a;
	if(!nextstr(str, place)) return false;
	while(str != b) {
		v.push_back(str);
		if(!nextstr(str, place)) return false;
	}
		
	return true;
}


bool fill(vector<string>& v, ostream& os, istream& istr) {
	vector<string>::iterator it = find(v.begin(), v.end(), ELLIPSIS);
	vector<string>::iterator is = v.begin();
	
	if(it == v.begin()) return false;
	vector<string> r(v.begin(), it);
	while(it != v.end()) {
		if(it == (v.end() - 1) ) return false;
		vector<string> fillvec;
		if(!fillit(*(it-1), *(it+1), fillvec, os, istr)) return false;
		r.insert(r.end(), fillvec.begin(), fillvec.end());
		is = it + 1;
		it = find(is, v.end(), ELLIPSIS);
		r.insert(r.end(), is, it);

	}
	v= r;
	return true;
}

int getinp_aux(vector<string> *input, const char *prompt, unsigned long count, bool domore, bool dofill, ostream& os, istream& is) {

	*input = vector<string>();
	
	bool notdone = true;
	while(notdone) {
		notdone = false;
		string line("");
		int r = getinp(&line, prompt, os, is);
		switch(r) {
			case -1: 	return -1;
			case  0:	return 0;
			case  1:	break; // Fall through
		}
		prslin(line,input);
		if(domore && ((*input).back() == ELLIPSIS)) {
			(*input).pop_back();
			notdone = true;
		}
		if(dofill && !fill(*input, os, is)) return 0;
		if((*input).size() < count) notdone = true;
		if(notdone) os << endl << (*input).size() << " entries. Continue:" << endl;
	}
	return 1;
}

int getinp(string *input, const char *prompt, ostream& os, istream& is) {
	*input = string("");
	os << prompt;
	(*input).getline(is);
	if((*input).length() == 0) return -1;
	return 1;
}


int getinpvec(vector<string> *v, const char *prompt, bool dofill, unsigned long num, ostream& os, istream& is) {
	return getinp_aux(v, prompt, num, true, dofill, os, is);
}

int getinp(char *p, const char *prompt, ostream& os, istream& is, int count) {
	string inp;
	int r = getinp(&inp, prompt, os, is);
	strncpy(p, inp.c_str(), count);
	return r;
}
