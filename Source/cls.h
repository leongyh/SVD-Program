#ifndef _CLS_H
#define _CLS_H

#include <iostream.h>

// Number of lines in screen
#define SCREENLINES 24

inline void cls(void) {
	for(int i=0; i < SCREENLINES; i++) cout << endl;
	return;
}

#endif




