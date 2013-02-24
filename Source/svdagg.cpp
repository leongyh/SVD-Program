#include "svdagg.h"
#include <iostream.h>
#include "svdspec.h"
#include "aggreg.h"
#include "cls.h"
#include "getinp.h"

void svdagg(void) {
	int choice;
	static spcsvdcmp_t svd;
	
	cls();
	
	cout << " Choose an option:" << endl << endl;
	cout << " 0: Return to main menu " << endl << endl;
	cout << " 1: SVD " << endl << endl;
	cout << " 2: Compute aggregate spectra " << endl << endl;
	
	bool inperr = true;
	while(inperr) {
		inperr = false;
		if(!getinpnoerr(&choice, "Choose an action: ")) return;
		switch (choice) {
			case 0: 	return;
			case 1: 	svd = svdspec();
						break;
			case 2: 	if(svd.svd.s.empty()) svd = svdspec();
						if(svd.svd.s.empty()) break;
						aggreg(svd);
						break;
			default:	inperr = true;
		}
	}

	return;
}
