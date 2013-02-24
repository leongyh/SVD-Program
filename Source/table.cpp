#include "table.h"

ostream& print(ostream& os, const table_t& t) {
	
	bool hastitle = (t.title.length() == 0) ? false : true;
	bool hasrowhead = (t.rowhead.size() == 0) ? false : true;
	bool hascolhead = (t.colhead.size() == 0) ? false : true;
	
	const long saveflags = os.flags();
	
	const int width = (hasrowhead) ? t.table[0].size() + 1 : t.table[0].size();
	
	vector<int> colwidth(width, 0);

	if(hascolhead)
		for(int i=0; i<t.colhead.size(); i++)
			if(t.colhead[i].length() > colwidth[i]) colwidth[i] = t.colhead[i].length();
	
	int offset = (hasrowhead) ? 1 : 0;
	for(int i=0; i<t.table.size(); i++) {
		if(hasrowhead) 
			if(t.rowhead[i].length() > colwidth[0]) colwidth[0] = t.rowhead[i].length();
		for(int j=0; j<t.table[i].size(); j++)
			if(t.table[i][j].length() > colwidth[j+offset]) colwidth[j+offset] = t.table[i][j].length();
	}
			
	

	for(int j=0; j<colwidth.size(); j++) colwidth[j] += 2;


	os.setf(ios::left, ios::adjustfield);
	if(hastitle) os << t.title << endl << endl;
	os.setf(ios::right, ios::adjustfield);
	if(hascolhead) {
		for(int i=0; i<t.colhead.size(); i++) os << setw(colwidth[i]) << t.colhead[i];
		os << endl;
	}
	for(int k=0; k<t.table.size(); k++) {
		if(hasrowhead) os << setw(colwidth[0]) << t.rowhead[k];
		for(int i=0; i<t.table[k].size(); i++)
			os << setw(colwidth[offset+i]) << t.table[k][i];
		os << endl;
	}

	
	os.flags(saveflags);
	return os;
}



