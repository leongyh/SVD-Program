#ifndef _SPECTRUM_H
#define _SPECTRUM_H

#include <pair.h>
#include <vector.h>
#include <math.h>

typedef pair<float, float> wavelength_absorbance_pair_t;
typedef vector<wavelength_absorbance_pair_t> ascii_spectrum_t;
typedef vector<unsigned char> spc_spectrum_t;


#ifdef _CONVERT_IBM_NUMERICS

#include "string"

// This function converts the IBM 4-byte floating point representation
// to the native representation.  It is to be used to convert IBM binary files to the non-IBM
// native representation.
// The IBM representation, for bits 0 (least significant) to 31 (most significant), is
// Sign bit = 31  1 = negative, 0 = positive
// Exponent = bits 24 to 30, with bias 64, base 4
// Bit 23 is also an exponential bit, base 2
// A factor of two is also assumed
// Mantissa = bits 0 to 23, with a leading 1 assumed
// Let e = exponent, base 4; t = exponent, base 2; s = sign bit; m = mantissa
// f = (1 + m) * 2**(2*(e-64)+t+1) * (-1)**s 
// Also note that the IBM representation uses the first byte for the lease significant bits, 
// last byte for the most significant bits
// In the function below, the argument must point to the first byte of the 4-byte representation
// The function assumes machine architecture of 8-bit chars, and unsigned long at least 4 bytes, with leading most-significant byte
float ibm_4byte_float_to_native(const unsigned char *);

// This function forms the IBM 4-byte floating point representation from native float.
// The output is put into the unsigned char string pointed to by p.
// Machine architecture assumed as above
string native_float_to_ibm_4byte(float);

// The IBM int translators simply reverse the byte order
// No assumptions about the length of an unsigned int
unsigned int ibm_2byte_unsigned_int_to_native(const unsigned char *);
string native_unsigned_int_to_ibm_2byte(unsigned int);



#else

inline float ibm_4byte_float_to_native(const unsigned char *p) {
	const float *x = (float *)p;
	return *x;
}
inline float native_float_to_ibm_4byte(float x) {return x;}
inline unsigned int ibm_2byte_unsigned_int_to_native(const unsigned char *p) {
	unsigned int x = *((const unsigned int *)p) & 0xffff;
	return x;
}
inline unsigned int native_unsigned_int_to_ibm_2byte(unsigned int x) {return x;}

#endif


ascii_spectrum_t spctoasc(const spc_spectrum_t&);
spc_spectrum_t asctospc(ascii_spectrum_t&);

class spectrum {
private:
	float _longw;
	float _shortw;
	
	vector<float> _ascspec;
	spc_spectrum_t _spcspec;
	
	void _spctoasc(void);
	void _asctospc(void);

	vector<float> _asciix(const ascii_spectrum_t& asc) {
		vector<float> _asc;
		for(vector<float>::size_type i=0; i<asc.size(); i++) 						_asc.push_back(asc[i].second);
		return _asc;
	}

public:
	class info_t{
	public:
		float longw; 
		float shortw; 
		float incr; 
		unsigned long size;
		
		bool operator==(const info_t& x) const {return ((longw == x.longw) && (shortw == x.shortw) && (size == x.size));}
		bool operator!=(const info_t& x) const {return !operator==(x);}
	
		info_t& operator=(const info_t& i) {
			longw = i.longw;
			shortw = i.shortw;
			incr = i.incr;
			size = i.size;
			return *this;
		}
	
		info_t(float lw, float sw, float inc, unsigned long sz) : longw(lw), shortw(sw), incr(inc), size(sz) {return;}
		info_t(void) : longw(0.0), shortw(0.0), incr(0.0), size(0) {return;}
		~info_t(void) {return;}
	};
	
	spectrum(void) {return;}
	spectrum(const spc_spectrum_t& spc) : _spcspec(spc) {
		_spctoasc();
		return;
	}
	spectrum(const ascii_spectrum_t& asc) : _ascspec(_asciix(asc)), _longw(asc[0].first), _shortw(asc.back().first) {
		_asctospc();
		return;
	}
	
	spectrum(const vector<float>& v, float longw, float shortw) : _longw(longw), _shortw(shortw), _ascspec(v) {
		_asctospc();
		return;
	}

	
	~spectrum(void) {return;}
	
	spectrum& operator=(const spectrum& s) {
		if(&s != this) {
			_longw = s._longw;
			_shortw = s._shortw;
	
			_ascspec = s._ascspec;
			_spcspec = s._spcspec;
		}
		return *this;
	}
		
	
	void put(const ascii_spectrum_t& asc) {
		_ascspec = _asciix(asc);
		_asctospc();
		return;
	}
	
	void put(const vector<float>& v, float longw, float shortw) {
		_longw = longw;
		_shortw = shortw;
		_ascspec = v;
		_asctospc();
		return;
	}

	void put(const spc_spectrum_t& spc) {_spcspec = spc; _spctoasc(); return;}
	void put(unsigned long i, float x) {
		_ascspec[i] = x;
		_asctospc();
		return;
	}
	
	
	
	// Return number of data points
	ascii_spectrum_t::size_type size(void) const {return _ascspec.size();}
	
	// Return long wavelength of data range
	float shortw(void) const {return _shortw;}
	
	// Return short wavelength of data range
	float longw(void) const {return _longw;}
	
	// Return wavelength increment between data points
	float incr(void) const {return (_longw - _shortw)/(_ascspec.size() - 1);}

	info_t info(void) const {return info_t(longw(), shortw(), incr(), size());}

	// Return the ascii formatted spectrum
	ascii_spectrum_t get_asc(void) const {
		ascii_spectrum_t r;
		float wlen = _longw;
		float inc = incr();
		for(vector<float>::size_type i=0; i<_ascspec.size(); i++) {
			r.push_back(wavelength_absorbance_pair_t(wlen, _ascspec[i]));
			wlen -= inc;
		}
		return r;
	}
	
	// Return the spc formatted spectrum
	spc_spectrum_t get_spc(void) const {return _spcspec;}

	// Return the vector of absorbance values
	vector<float> get_abs(void) const {return _ascspec;}
	
	// Returns index of wavelength x (rounded to closest data point)
	long which(float x) const {
		float t = (_longw - x)/incr();
		long w = long(floor(t));
		if((t - w) > 0.5) w += 1;
		return w;
	}

	// Return absorbance of index i
	float absorb(long i) const {return _ascspec[i];}
	
	// Return absorbance at wavelength x
	float absorb(float x) const {return absorb(which(x));}
	
	
	// Return wavelength of index i
	float wavelength(long i) const {return _longw - i*incr();}
	
	// Return nearest wavelength data
	float wavelength(float x) const {return wavelength(which(x));}

	wavelength_absorbance_pair_t waveabs(ascii_spectrum_t::size_type i) const {
		return wavelength_absorbance_pair_t(wavelength(long(i)), _ascspec[i]);
	}

	// Return ith absorbance point
	float operator[](ascii_spectrum_t::size_type i) const {return absorb(long(i));}
	float operator[](long i) const {return absorb(i);}
	float operator[](int i) const {return absorb(long(i));}
	
	// Return absorbance at wavelength x (rounded to closest data point)
	float operator[](float x) const {return absorb(x);}

	bool inrange(float x) const {
		long t = which(x);
		return ((t >= 0) && (t < _ascspec.size()));
	}

	// Simple arithmetic operations
	void add(float x) {
		for(ascii_spectrum_t::size_type i=0; i < _ascspec.size(); i++) _ascspec[i] += x;
		_asctospc();
		return;
	}
	
	void add(const spectrum& spec) {
		if(info() != spec.info()) return;
		for(ascii_spectrum_t::size_type i=0; i< _ascspec.size(); i++) _ascspec[i] += spec._ascspec[i];
		_asctospc();
		return;
	}
		
	void sub(float x) {add(-x); return;}
	
	void sub(const spectrum& spec) {
		if(info() != spec.info()) return;
		for(ascii_spectrum_t::size_type i=0; i< _ascspec.size(); i++) _ascspec[i] -= spec._ascspec[i];
		_asctospc();
		return;
	}
	
	void mul(float x) {
		for(ascii_spectrum_t::size_type i=0; i < _ascspec.size(); i++) _ascspec[i] *= x;
		_asctospc();
		return;
	}
	
	void div(float x) {
		for(ascii_spectrum_t::size_type i=0; i < _ascspec.size(); i++) _ascspec[i] /= x;
		_asctospc();
		return;
	}
	
	spectrum& operator+=(const spectrum& s) {
		add(s);
		return *this;
	}
	
	spectrum& operator-=(const spectrum& s) {
		sub(s);
		return *this;
	}
	
	spectrum& operator+=(float s) {
		add(s);
		return *this;
	}
	
	spectrum& operator-=(float s) {
		sub(s);
		return *this;
	}
	
	spectrum& operator*=(float s) {
		mul(s);
		return *this;
	}
	
	spectrum& operator/=(float s) {
		div(s);
		return *this;
	}
	
	void shift(float x) {
		_longw += x;
		_shortw += x;
		return;
	}
	
	void pad_short(float x, float xpad = 0.0) {
		long w = which(x);
		if(w < _ascspec.size()) return;
		_shortw = wavelength(w);
		_ascspec.insert(_ascspec.end(), w - _ascspec.size() + 1, xpad);
		_asctospc();
		return;
	}
	
	void pad_long(float x, float xpad = 0.0) {
		long w = which(x);
		if(w >= 0) return;
		_longw = wavelength(w);
		vector<float> asc(-w, xpad);
		asc.insert(asc.end(), _ascspec.begin(), _ascspec.end());
		_ascspec = asc;
		_asctospc();
		return;
	}
	
	void crop_short(float x) {
		long w = which(x);
		if((w < 0) || (w >= _ascspec.size()-1)) return;
		_shortw = wavelength(w);
		vector<float> asc(_ascspec.begin(), _ascspec.begin() + w + 1);
		_ascspec = asc;
		_asctospc();
		return;
	}
	
	void crop_long(float x, float xpad = 0.0) {
		long w = which(x);
		if((w <= 0) || (w > _ascspec.size()-1)) return;
		_longw = wavelength(w);
		vector<float> asc(_ascspec.begin()+w, _ascspec.end());
		_ascspec = asc;
		_asctospc();
		return;
	}
	
	void range(float longw, float shortw, float pad = 0.0) {
		if(longw > _longw) pad_long(longw);
		else crop_long(longw);
		if(shortw < _shortw) pad_short(shortw);
		else crop_short(shortw);

		return;
	}
};

spectrum readspc(istream&);
spectrum readasc(istream&);
ostream& writespc(const spectrum&, ostream&);
ostream& writeasc(const spectrum&, ostream&);

bool chkinfo(const spectrum::info_t&, const spectrum::info_t&);
bool chkinfo(const spectrum&, const spectrum&);
bool chkinfo(const spectrum&, const spectrum::info_t&);
bool chkinfo(const spectrum::info_t&, const spectrum&);

#endif





