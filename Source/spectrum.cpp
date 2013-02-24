#include "spectrum.h"
#include <stdlib.h>
#include "string"
#include <string.h>
#include <strstream.h>
#include "getinp.h"

#ifdef _CONVERT_IBM_NUMERICS
void _push(float x, unsigned char *p) {
	string s = native_float_to_ibm_4byte(x);
	for(int i=0; i<4; i++) p[i] = s[i];
	return;
}

void _push(unsigned int x, unsigned char *p) {
	string s = native_unsigned_int_to_ibm_2byte(x);
	for(int i=0; i<2; i++) p[i] = s[i];
	return;
}

#else

void _push(float x, unsigned char *p) {
	const unsigned char *s = (unsigned char *)(&x);
	for(int i=0; i<4; i++) p[i] = s[i];
	return;
}

void _push(unsigned int x, unsigned char *p) {
	const unsigned char *s = (unsigned char *)(&x);	
	for(int i=0; i<2; i++) p[i] = s[i];
	return;
}

#endif

#ifdef _CONVERT_IBM_NUMERICS

// See description of IBM format in spectrum.h
// In the function below, the argument contains the IBM 4-byte float format, with byte order already reversed
// The function assumes 8-bit chars, and unsigned long at least 4 bytes
float ibm_4byte_float_to_native(unsigned long ibm_float) {
	// Assume native machine unsigned long format is most significant byte first, 
	// and that unsigned long is at least 4 bytes.
	// On input, the ibm-formatted 4-byte float is to be stuffed into the 4 least significant bytes
	// of the function argument, and is already byte-order reversed.
	// Do computations in double precision, then cast to single precision.


	const int mantissa_bits = 23;
	const int ibm_bias = 64;
	
	if(!ibm_float) return 0.0;
	unsigned long temp = ibm_float;		// Working space

	// Accumulate the mantissa
	unsigned long mask = 1UL;
	int shift = 0;
	double rr = 0.0;
	for(int i=0; i<mantissa_bits; i++) {
		rr += (mask & temp) >> shift;
		rr /= 2;
		mask <<= 1;
		shift += 1;
	}
	rr += 1;							// Implied leading 1
	
	// All done with the mantissa, now shift it off
	temp >>= mantissa_bits;
	
	unsigned int t = temp & 1UL;		// Get the base 2 exponent, which is now the last bit...
	temp >>= 1;							// ...and shift it off
	
	// Compute the exponent, which is now the least significant byte of working space
	int expon = (temp & 0x7f) - ibm_bias;		// Set the leading bit (sign bit) to 0, and subtract the bias 64
	expon *= 2;									// Now change to base 2
	expon += t;									// Get the last exponent bit
	expon += 1;									// Get the implied factor of two
	
	rr *= pow(2, expon);						// Now have absolute value
												
												// Tricks to get the proper sign, without a branch
	rr *= 1 - int((temp >> 6) & 2);				// Put the sign in the second bit, mask off the first bit, and subtract from 1

	return float(rr);
}
float ibm_4byte_float_to_native(const unsigned char *p) {
	// Reverse the byte order, and stuff it into an unsigned long
	// Call the above function for the calculation
	unsigned long t = 0UL;
	unsigned char *pt = (unsigned char *)(&t) + sizeof(unsigned long) - 1;
	for(int i=0; i<4; i++) *pt-- = p[i];
	return ibm_4byte_float_to_native(t);
}
string native_float_to_ibm_4byte(float x) {
	string p(4, '\0');
	double ax = fabs(x);
	const int ibm_bias = 64;
	if(ax == 0.0) return p;
	int et;
	double m = frexp(ax, &et);		// 0.5 <= m < 1,  ax = m * 2**expon (definition of the library function frexp)
	m *= 2;							// 1 <= m < 2
	m -= 1;							// Now have mantissa
	et -= 2;						// et = 2*(e-64)+t   odd if t = 1, even if t = 0	
	int t = abs(et % 2);			// get t, but modulus can be negative!  Have to account fot that.
							
	et -= t;						// now get e
	et /= 2;
	et += ibm_bias;
	et &= 0x7f;						// Clear the sign bit (and higher bits); now have et = e
	if(x < 0.0) et |= 0x80;			// Set the sign bit if necessary
	p[3] = et;						// Most significant byte of output is complete, so stuff it


	// Now construct the output mantissa
	unsigned long temp = 0UL;
	unsigned long mask = 0x40000000;
	
	while(m > 0.0) {
		m = frexp(m, &et);
		mask >>= -et;
		temp |= mask;
		m -= 0.5;
	}
	unsigned char *pt = (unsigned char *)(&temp);
	p[2] = pt[0];
	p[1] = pt[1];
	p[0] = pt[2];
		
	p[2] |= t << 7;			// Put t in the correct bit
		
	return p; 
}


unsigned int ibm_2byte_unsigned_int_to_native(const unsigned char * p) {
	unsigned int r = p[1];
	r <<= 8;
	r |= p[0];
	return r;
}

string native_unsigned_int_to_ibm_2byte(unsigned int x) {
	string p(2, '\0');
	unsigned char c = x & 0xff;
	p[0] = c;
	c = (x & 0xff00) >> 8;
	p[1] = c;
	return p;
}

#endif


void _spcx(const spc_spectrum_t& spc, float *longw, float *shortw, vector<float> *asc) {
	*longw = ibm_4byte_float_to_native(&spc[10]);
	*shortw = ibm_4byte_float_to_native(&spc[14]);
	const int numpts = ibm_2byte_unsigned_int_to_native(&spc[118]);	

	*asc = vector<float>();
	const unsigned char *p = &spc[120];		// Point to beginning of array of absorbances
	for(int i=0; i< numpts; i++) {
		float absval = ibm_4byte_float_to_native(p);
		(*asc).push_back(absval);
		p += 4;
	}
	return;
}

spc_spectrum_t _spchead(void) {
	const unsigned char *spchead =
		(unsigned char *)	"\000\000\002\000\002\000\006\000\004\000"  \
							"\000\000\341\103\000\000\252\103\000\000"  \
							"\000\100\012\327\243\274\001\000\005\000"  \
							"\130\061\060\070\103\000\000\000\000\000"  \
							"\000\000\000\000\000\000\000\000\000\000"  \
							"\000\000\000\000\000\000\000\000\000\000"  \
							"\000\000\000\000\000\000\000\000\000\000"  \
							"\000\000\000\000\000\000\000\000\000\000"  \
							"\000\000\000\000\000\000\000\000\000\000"  \
							"\000\000\000\000\000\000\000\000\000\000"  \
							"\062\060\072\063\067\000\064\064\000\060"  \
							"\067\057\062\060\057\071\062\000\000\000";			// Length is 120 bytes
	spc_spectrum_t spc;
	for(int i=0; i<120; i++) spc.push_back(spchead[i]);
	return spc;
}

void _ascx(const vector<float>& asc, float longw, float shortw, spc_spectrum_t *spc) {
	*spc = _spchead();
	const unsigned int numpts = asc.size();

	(*spc).insert((*spc).end(), 4*numpts, '\0');
	
	unsigned char *p = &(*spc)[120] - 4;		// Points to 4 bytes in front of array of absorbance values
	for(int ii=0; ii<numpts; ii++) {
		p += 4;
		_push(asc[ii], p);
	}

	_push(longw, &(*spc)[10]);
	_push(shortw, &(*spc)[14]);
	_push(numpts, &(*spc)[118]);

	return;
}

void spectrum::_spctoasc(void) {
	_spcx(_spcspec, &_longw, &_shortw, &_ascspec); 
	return;
}

void spectrum::_asctospc(void) {
	_ascx(_ascspec, _longw, _shortw, &_spcspec); 
	return;
}

ascii_spectrum_t spctoasc(const spc_spectrum_t& spc) {
	float longw, shortw;
	vector<float> _asc;
	
	_spcx(spc, &longw, &shortw, &_asc);
	const int numpts = _asc.size();
	
	const float incr = (longw - shortw)/(numpts - 1);
	float wlen = longw;
	ascii_spectrum_t asc;
	for(int i=0; i< numpts; i++) {
		asc.push_back(wavelength_absorbance_pair_t(wlen, _asc[i]));
		wlen -= incr;
	}
	
	return asc;
}

spc_spectrum_t asctospc(ascii_spectrum_t& asc) {
	spc_spectrum_t spc;
	vector<float> _asc;
	for(ascii_spectrum_t::size_type i=0; i < asc.size(); i++) _asc.push_back(asc[i].second);
	float longw = asc[0].first;
	float shortw = asc.back().first;
	_ascx(_asc, longw, shortw, &spc);
	return spc;
}

spectrum readspc(istream& is) {
	spc_spectrum_t spc;
	while(!is.eof()) {
		unsigned char c = is.get();
		spc.push_back(c);
	}
	return spectrum(spc);
}
spectrum readasc(istream& is) {
	ascii_spectrum_t asc;
	string str;
	while(!is.eof()) {
		getline(is, str);
		char *xp = str.c_str();
		xp = strtok(xp, " ,\n\t");
		vector<float> v;
		while(xp) {
			float x = atof(xp);
			v.push_back(x);
			xp = strtok(NULL, " ,\n\t");
		}
		for(vector<float>::size_type i=0; i<v.size(); i += 2) {
			float x = v[i];
			float y = v[i+1];
			asc.push_back(wavelength_absorbance_pair_t(x,y));
		}
		v = vector<float>();
	}
	return spectrum(asc);
}


ostream& writespc(const spectrum& spec, ostream& os) {
	spc_spectrum_t spc = spec.get_spc();
	
	for(spc_spectrum_t::size_type i=0; i<spc.size(); i++) os << spc[i];

	return os;
}
ostream& writeasc(const spectrum& spec, ostream& os) {
	ascii_spectrum_t asc = spec.get_asc();
	
	for(ascii_spectrum_t::size_type i=0; i<asc.size(); i++) os << asc[i].first << "\t" << asc[i].second << endl;

	return os;
}

bool chkinfo(const spectrum::info_t& a, const spectrum::info_t& b) {
	if(a != b) {
		char s[1024];
		ostrstream os(s, 1024);
		os <<	"Wavelength ranges of spectra are not identical!" << endl <<
				"Had:         Long wavelength = " << a.longw << endl <<
				"            Short wavelength = " << a.shortw << endl <<
				"       Number of data points = " << a.size << endl <<
				"and:         Long wavelength = " << b.longw << endl <<
				"            Short wavelength = " << b.shortw << endl <<
				"       Number of data points = " << b.size << endl << flush;
					
		errmess(s);
		return false;
	}
	return true;
}

bool chkinfo(const spectrum& a, const spectrum& b) {
	return chkinfo(a.info(), b.info());
}

bool chkinfo(const spectrum& a, const spectrum::info_t& b) {
	return chkinfo(a.info(), b);
}

bool chkinfo(const spectrum::info_t& a, const spectrum& b) {
	return chkinfo(a, b.info());
}



