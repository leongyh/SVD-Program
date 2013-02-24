#ifndef _INDEXX_H
#define _INDEXX_H

#include <vector.h>
#include "nrutil.h"
#include "getinp.h"

#define SWAP(a,b) itemp=(a);(a)=(b);(b)=itemp;
#define M 7
#define NSTACK 50

template <class T>
vector<unsigned long> indexx(const vector<T>& arr) {
	unsigned long n = arr.size();
	vector<unsigned long> indx(n,0UL);
	unsigned long i,indxt,ir=n,itemp,j,k,l=1;
	int jstack=0;
	T a;

	vector<int> istack(NSTACK, 0);
	for (j=1;j<=n;j++) indx[j-1]=j;
	for (;;) {
		if (ir-l < M) {
			for (j=l+1;j<=ir;j++) {
				indxt=indx[j-1];
				a=arr[indxt-1];
				for (i=j-1;i>=1;i--) {
					if (arr[indx[i-1]-1] <= a) break;
					indx[i+1-1]=indx[i-1];
				}
				indx[i+1-1]=indxt;
			}
			if (jstack == 0) break;
			ir=istack[jstack -1];
			jstack--;
			l=istack[jstack -1];
			jstack--;
		} else {
			k=(l+ir) >> 1;
			SWAP(indx[k-1],indx[l+1-1]);
			if (arr[indx[l+1-1]-1] > arr[indx[ir-1]-1]) {
				SWAP(indx[l+1-1],indx[ir-1])
			}
			if (arr[indx[l-1]-1] > arr[indx[ir-1]-1]) {
				SWAP(indx[l-1],indx[ir-1])
			}
			if (arr[indx[l+1-1]-1] > arr[indx[l-1]-1]) {
				SWAP(indx[l+1-1],indx[l-1])
			}
			i=l+1;
			j=ir;
			indxt=indx[l-1];
			a=arr[indxt-1];
			for (;;) {
				do i++; 
				while (arr[indx[i-1]-1] < a);
				do j--;
				while (arr[indx[j-1]-1] > a);
				if (j < i) break;
				SWAP(indx[i-1],indx[j-1])
			}
			indx[l-1]=indx[j-1];
			indx[j-1]=indxt;
			jstack += 2;
			if (jstack > NSTACK) {
				errmess("NSTACK too small in indexx.");
				return vector<unsigned long>();
			}
			if (ir-i+1 >= j-l) {
				istack[jstack-1]=ir;
				istack[jstack-1-1]=i;
				ir=j-1;
			} else {
				istack[jstack-1]=j-1;
				istack[jstack-1-1]=l;
				l=i;
			}
		}
	}
	for(i=0; i<indx.size(); i++) indx[i] -= 1;
	return indx;
}


template <class T>
vector<unsigned long> rindexx(const vector<T>& arr) {
	vector<unsigned long> ix = indexx(arr);
	if(ix.size() < 2) return ix;
		
	unsigned long last = ix.size()-1;
	unsigned long lim = ix.size()/2 - 1;
	for(unsigned long i=0; i <= lim; i++) {
		unsigned long swap = ix[i];
		ix[i] = ix[last - i];
		ix[last - i] = swap;
	}
	return ix;
}



#undef M
#undef NSTACK
#undef SWAP



#endif
