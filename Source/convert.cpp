#include "convert.h"
#include <strstream.h>

void pads(string *s, int len, char pad) {
	if((*s).length() < len) {
		string temp(pad, len - (*s).length());
		temp += *s;
		*s = temp;
	}
}

string numtostr_aux(unsigned long i) {
	if(i < 10UL) return string(char('0'+i));
	string r = numtostr_aux(i/10UL);
	r += char('0' + (i % 10));
	return r;
}

bool convert(unsigned long i, string *s, int len, char pad) {
	*s = numtostr_aux(i);
	pads(s, len, pad);
	return true;
}

bool convert(long i, string *s, int len, char pad) {
	*s = (i < 0) ? string('-') : string();
	*s += numtostr_aux((unsigned long)(labs(i)));	
	pads(s, len, pad);
	return true;
}

bool convert(double x, string *s, int precision, char format, int width) {
	string r('\0', 80);
	
	ostrstream ostr(r.c_str(), 80);
	
	if(precision != 0) ostr.precision(precision);
	switch(format) {
		case 'f':	ostr.setf(ios::fixed, ios::floatfield);
					break;
		case 's':	ostr.setf(ios::scientific, ios::floatfield);
					break;
		
	}

	ostr << setw(width) << x;
	*s = string(r.c_str());
	return true;
}

