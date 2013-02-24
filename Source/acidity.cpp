#include <iostream.h>
#include "string"
#include <vector.h>
#include "cls.h"
#include "adjust.h"
#include "getinfo.h"
#include "baseline.h"
#include "cropper.h"
#include "avespec.h"
#include "ascspc.h"
#include "spcasc.h"
#include "svdagg.h"
#include "fitspec.h"
#include "getabs.h"
#include <string.h>
#include <fstream.h>
#include "getinp.h"

// Global pathname
string path;
string prmpt = "Enter filename(s) or range (no extensions!): ";

// Local routine declarations
void menu(void);
bool doit(int, bool*);
void getpath(void);

int main(void) {
	int choice;
	bool theend = false;
	
	getpath();

	while(!theend) {
		cls();
		menu();
		if(getinpnoerr(&choice, "Choose an action: "))
			if(!doit(choice, &theend))
				errmess("\nNo such option! Try again.");
				
	}
	cout << "Exiting."  << endl;

	return 0;
}

void menu(void) {
//	***   write the menu ***
	cls();
	cout << " 1: Average spectra " << endl << endl;
	cout << " 2: Baseline correct spectra " << endl << endl;
	cout << " 3: Crop / pad spectra" << endl << endl;
	cout << " 4: SPC to ASCII translation" << endl << endl;
	cout << " 5: ASCII to SPC translation" << endl << endl;
	cout << " 6: SVD / aggregation" << endl << endl;
	cout << " 7: Spectral fitting" << endl << endl;
	cout << " 8: Get spectrum info" << endl << endl;
	cout << " 9: Get absorbances" << endl << endl;
	cout << "10: Adjust for drift" << endl << endl;
	cout << "11: End" << endl << endl;

	return;
}

bool doit(int choice, bool *stopit) {
//	*** call the subroutine from the main menu choice ***

	*stopit = false;
	
	switch(choice) {
		case 1:		avespec();
					break;
		case 2:		baseline();
					break;
		case 3:		cropper();
					break;		
		case 4:		spcasc();
					break;
		case 5:		ascspc();
					break;
		case 6:		svdagg();
					break;
		case 7:		fitspec();
					break;
		case 8:		getinfo();
					break;
		case 9:		getabs();
					break;
		case 10:	adjust();
					break;
		case 11:	*stopit = true;
					break;
		default:	return false;
	}
	return true;
}

#ifdef _USE_MAC_FILESYSTEM
#define DIRECTORY_SEPARATOR ':'
#else
#define DIRECTORY_SEPARATOR '/'
#endif

void getpath(void) {
	string s(1024, '\0');
	cout << "Enter a default directory."<< endl;
	cout << "Enter RETURN for current folder" << endl;
	cout << "or a comlete pathname for another folder." << endl;
	cout << "Folder names must be separated by " << DIRECTORY_SEPARATOR << endl << endl;
	if(getinpnoerr(&s, "Enter default directory: "))
		if(s[s.length()-1] != DIRECTORY_SEPARATOR) s += DIRECTORY_SEPARATOR;
	::path = s;
	return;
}

#undef DIRECTORY_SEPARATOR

