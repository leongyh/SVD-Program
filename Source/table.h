#ifndef _TABLE_H
#define _TABLE_H

#include <iostream.h>
#include "string"
#include <vector.h>

struct table_t {
	string title;
	vector<string> colhead;
	vector<string> rowhead;
	vector<vector<string> > table;
};

typedef struct table_t table_t;

ostream& print(ostream&, const table_t&);

inline ostream& operator<<(ostream& os, const table_t& t) {return print(os, t);}



#endif


