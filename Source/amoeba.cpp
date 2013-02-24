#include "amoeba.h"
#include <math.h>
#include "nrutil.h"
#include "getinp.h"


double amotry(matrix<double>& p, vector<double>& y, vector<double>& psum, int ndim,
	funk_t& funk, int ihi, double fac)
{
	int j;
	double fac1,fac2,ytry;

	vector<double> ptry(ndim, 0.0);
	fac1=(1.0-fac)/ndim;
	fac2=fac1-fac;
	for (j=1;j<=ndim;j++) ptry[j-1]=psum[j-1]*fac1-p[ihi-1][j-1]*fac2;
	ytry=funk(ptry);
	if (ytry < y[ihi-1]) {
		y[ihi-1]=ytry;
		for (j=1;j<=ndim;j++) {
			psum[j-1] += ptry[j-1]-p[ihi-1][j-1];
			p[ihi-1][j-1]=ptry[j-1];
		}
	}
	return ytry;
}

#define NMAX 10000
#define GET_PSUM \
					for (j=1;j<=ndim;j++) {\
					for (sum=0.0,i=1;i<=mpts;i++) sum += p[i-1][j-1];\
					psum[j-1]=sum;}
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}

bool amoeba(matrix<double> *pt, vector<double> *yt, double ftol,
	funk_t& funk, int *nfunk)
{
	matrix<double> &p = *pt;
	vector<double> &y = *yt;
	int ndim = p.cols();
	int i,ihi,ilo,inhi,j,mpts=ndim+1;
	double rtol,sum,swap,ysave,ytry;

	vector<double> psum(ndim, 0.0);

	*nfunk=0;
	GET_PSUM
	for (;;) {
		ilo=1;
		ihi = y[0]>y[1] ? (inhi=2,1) : (inhi=1,2);
		for (i=1;i<=mpts;i++) {
			if (y[i-1] <= y[ilo-1]) ilo=i;
			if (y[i-1] > y[ihi-1]) {
				inhi=ihi;
				ihi=i;
			} else if (y[i-1] > y[inhi-1] && i != ihi) inhi=i;
		}
		rtol=2.0*fabs(y[ihi-1]-y[ilo-1])/(fabs(y[ihi-1])+fabs(y[ilo-1]));
		if ((rtol < ftol) || *nfunk >= NMAX) {
			SWAP(y[0],y[ilo-1])
			for (i=1;i<=ndim;i++) SWAP(p[1-1][i-1],p[ilo-1][i-1])
			if (rtol < ftol) break;
			if (*nfunk >= NMAX) {
				errmess("Maximum iterations exceeded in minimization.");
				return false;
			}
		}
		
		*nfunk += 2;
		ytry=amotry(p,y,psum,ndim,funk,ihi,-1.0);
		if (ytry <= y[ilo-1])
			ytry=amotry(p,y,psum,ndim,funk,ihi,2.0);
		else if (ytry >= y[inhi-1]) {
			ysave=y[ihi-1];
			ytry=amotry(p,y,psum,ndim,funk,ihi,0.5);
			if (ytry >= ysave) {
				for (i=1;i<=mpts;i++) {
					if (i != ilo) {
						for (j=1;j<=ndim;j++)
							p[i-1][j-1]=psum[j-1]=0.5*(p[i-1][j-1]+p[ilo-1][j-1]);
						y[i-1]=funk(psum);
					}
				}
				*nfunk += ndim;
				GET_PSUM
			}
		} else --(*nfunk);
	}
	return true;
}
#undef SWAP
#undef GET_PSUM
#undef NMAX
