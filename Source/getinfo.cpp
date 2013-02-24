#include "getinfo.h"
#include <fstream.h>
#include "spectrum.h"
#include "cls.h"
#include "fileops.h"
#include "getinp.h"
#include "table.h"
#include "convert.h"

extern const string prmpt;

void getinfo(void) {
	cls();	

	cout << endl << "Get spectrum info" << endl << endl;

	vector<string> fnames;
	//if(!getinpvecnoerr(&fnames, prmpt.c_str())) return;
	if(!getfiles(&fnames, prmpt.c_str(), ".spc")) return;
	table_t table;

	for(vector<string>::size_type i=0; i<fnames.size(); i++) {
		vector<string> row;

		ifstream ifile((fnames[i]+".spc").c_str());
		//if(!ifileopen(fnames[i], ".spc", ifile)) return;
		
		spectrum spec = readspc(ifile);
		spectrum::info_t info = spec.info();
		row.push_back(fnames[i]);
		row.push_back(numtostr(info.longw, 3, 'f'));
		row.push_back(numtostr(info.shortw, 3, 'f'));
		row.push_back(numtostr(info.size));
		row.push_back(numtostr(info.incr, 4, 'f'));
		table.table.push_back(row);
	}
	table.title = string("Spectrum info");
	table.colhead.push_back(string("Spectrum"));
	table.colhead.push_back(string("Long (nm)"));
	table.colhead.push_back(string("Short (nm)"));
	table.colhead.push_back(string("Data Pts"));
	table.colhead.push_back(string("Increment (nm)"));

	savedisk(table);
	cout << endl << table << endl;
	errmess("");
	return;
}


