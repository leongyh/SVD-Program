#include "dsvdcmp.h"
#include <math.h>
#include "nrutil.h"
#include <algo.h>
#include "getinp.h"

double dpythag(double a, double b)
{
	double absa,absb;
	absa=fabs(a);
	absb=fabs(b);
	if (absa > absb) return absa*sqrt(1.0+DSQR(absb/absa));
	else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+DSQR(absa/absb)));
}


svd_t dsvdcmp(const matrix<double>& mx) {
	matrix<double> a = mx;
	int m = a.rows();
	int n = a.cols();
	vector<double> rv1(n, 0.0);
	matrix<double> v(n, n, 0.0);
	vector<double> w(n, 0.0);
	
	int i, its, l, k, j, jj, nm, flag;
	double c,f,h,s,x,y,z;
	double g(0.0), scale(0.0), anorm(0.0);

	for (i=1;i<=n;i++) {
		l=i+1;
		rv1[i-1]=scale*g;
		g = 0.0;
		s = 0.0;
		scale = 0.0;
		if (i <= m) {
			for (k=i;k<=m;k++) scale += fabs(a[k-1][i-1]);
			if (scale) {
				for (k=i;k<=m;k++) {
					a[k-1][i-1] /= scale;
					s += a[k-1][i-1]*a[k-1][i-1];
				}
				f=a[i-1][i-1];
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				a[i-1][i-1]=f-g;
				for (j=l;j<=n;j++) {
					for (s=0.0,k=i;k<=m;k++) s += a[k-1][i-1]*a[k-1][j-1];
					f=s/h;
					for (k=i;k<=m;k++) a[k-1][j-1] += f*a[k-1][i-1];
				}
				for (k=i;k<=m;k++) a[k-1][i-1] *= scale;
			}
		}
		w[i-1]=scale *g;
		g=s=scale=0.0;
		if (i <= m && i != n) {
			for (k=l;k<=n;k++) scale += fabs(a[i-1][k-1]);
			if (scale) {
				for (k=l;k<=n;k++) {
					a[i-1][k-1] /= scale;
					s += a[i-1][k-1]*a[i-1][k-1];
				}
				f=a[i-1][l-1];
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				a[i-1][l-1]=f-g;
				for (k=l;k<=n;k++) rv1[k-1]=a[i-1][k-1]/h;
				for (j=l;j<=m;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[j-1][k-1]*a[i-1][k-1];
					for (k=l;k<=n;k++) a[j-1][k-1] += s*rv1[k-1];
				}
				for (k=l;k<=n;k++) a[i-1][k-1] *= scale;
			}
		}
		anorm=DMAX(anorm,(fabs(w[i-1])+fabs(rv1[i-1])));
	}
	for (i=n;i>=1;i--) {
		if (i < n) {
			if (g) {
				for (j=l;j<=n;j++) v[j-1][i-1]=(a[i-1][j-1]/a[i-1][l-1])/g;
				for (j=l;j<=n;j++) {
					s = 0.0;
					for (k=l;k<=n;k++) s += a[i-1][k-1]*v[k-1][j-1];
					for (k=l;k<=n;k++) v[k-1][j-1] += s*v[k-1][i-1];
				}
			}
			for (j=l;j<=n;j++) v[i-1][j-1]=v[j-1][i-1]=0.0;
		}
		v[i-1][i-1]=1.0;
		g=rv1[i-1];
		l=i;
	}
	for (i=IMIN(m,n);i>=1;i--) {
		l=i+1;
		g=w[i-1];
		for (j=l;j<=n;j++) a[i-1][j-1]=0.0;
		if (g) {
			g=1.0/g;
			for (j=l;j<=n;j++) {
				for (s=0.0,k=l;k<=m;k++) s += a[k-1][i-1]*a[k-1][j-1];
				f=(s/a[i-1][i-1])*g;
				for (k=i;k<=m;k++) a[k-1][j-1] += f*a[k-1][i-1];
			}
			for (j=i;j<=m;j++) a[j-1][i-1] *= g;
		} else for (j=i;j<=m;j++) a[j-1][i-1]=0.0;
		++a[i-1][i-1];
	}
	for (k=n;k>=1;k--) {
		for (its=1;its<=30;its++) {
			flag=1;
			for (l=k;l>=1;l--) {
				nm=l-1;
				if ((double)(fabs(rv1[l-1])+anorm) == anorm) {
					flag=0;
					break;
				}
				if ((double)(fabs(w[nm-1])+anorm) == anorm) break;
			}
			if (flag) {
				c=0.0;
				s=1.0;
				for (i=l;i<=k;i++) {
					f=s*rv1[i-1];
					rv1[i-1]=c*rv1[i-1];
					if ((double)(fabs(f)+anorm) == anorm) break;
					g=w[i-1];
					h=dpythag(f,g);
					w[i-1]=h;
					h=1.0/h;
					c=g*h;
					s = -f*h;
					for (j=1;j<=m;j++) {
						y=a[j-1][nm-1];
						z=a[j-1][i-1];
						a[j-1][nm-1]=y*c+z*s;
						a[j-1][i-1]=z*c-y*s;
					}
				}
			}
			z=w[k-1];
			if (l == k) {
				if (z < 0.0) {
					w[k-1] = -z;
					for (j=1;j<=n;j++) v[j-1][k-1] = -v[j-1][k-1];
				}
				break;
			}
			if (its == 30) {
				errmess("No convergence in 30 dsvdcmp iterations.");
				return svd_t();
			}
			x=w[l-1];
			nm=k-1;
			y=w[nm-1];
			g=rv1[nm-1];
			h=rv1[k-1];
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
			g=dpythag(f,1.0);
			f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
			c=s=1.0;
			for (j=l;j<=nm;j++) {
				i=j+1;
				g=rv1[i-1];
				y=w[i-1];
				h=s*g;
				g=c*g;
				z=dpythag(f,h);
				rv1[j-1]=z;
				c=f/z;
				s=h/z;
				f=x*c+g*s;
				g = g*c-x*s;
				h=y*s;
				y *= c;
				for (jj=1;jj<=n;jj++) {
					x=v[jj-1][j-1];
					z=v[jj-1][i-1];
					v[jj-1][j-1]=x*c+z*s;
					v[jj-1][i-1]=z*c-x*s;
				}
				z=dpythag(f,h);
				w[j-1]=z;
				if (z) {
					z=1.0/z;
					c=f*z;
					s=h*z;
				}
				f=c*g+s*y;
				x=c*y-s*g;
				for (jj=1;jj<=m;jj++) {
					y=a[jj-1][j-1];
					z=a[jj-1][i-1];
					a[jj-1][j-1]=y*c+z*s;
					a[jj-1][i-1]=z*c-y*s;
				}
			}
			rv1[l-1]=0.0;
			rv1[k-1]=f;
			w[k-1]=x;
		}
	}
	
	
	svd_t r;
	r.u = a;
	r.v = v;
	r.s = w;
	
	
	return r;
}

void svdcmp_inplace(svd_t& svd) {

	matrix<double> &a = svd.u;
	matrix<double> &v = svd.v;
	vector<double> &w = svd.s;
	
	int m = a.rows();
	int n = a.cols();
	vector<double> rv1(n, 0.0);
	v = matrix<double>(n, n, 0.0);
	w = vector<double>(n, 0.0);
	
	int i, its, l, k, j, jj, nm, flag;
	double c,f,h,s,x,y,z;
	double g(0.0), scale(0.0), anorm(0.0);

	for (i=1;i<=n;i++) {
		l=i+1;
		rv1[i-1]=scale*g;
		g = 0.0;
		s = 0.0;
		scale = 0.0;
		if (i <= m) {
			for (k=i;k<=m;k++) scale += fabs(a[k-1][i-1]);
			if (scale) {
				for (k=i;k<=m;k++) {
					a[k-1][i-1] /= scale;
					s += a[k-1][i-1]*a[k-1][i-1];
				}
				f=a[i-1][i-1];
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				a[i-1][i-1]=f-g;
				for (j=l;j<=n;j++) {
					for (s=0.0,k=i;k<=m;k++) s += a[k-1][i-1]*a[k-1][j-1];
					f=s/h;
					for (k=i;k<=m;k++) a[k-1][j-1] += f*a[k-1][i-1];
				}
				for (k=i;k<=m;k++) a[k-1][i-1] *= scale;
			}
		}
		w[i-1]=scale *g;
		g=s=scale=0.0;
		if (i <= m && i != n) {
			for (k=l;k<=n;k++) scale += fabs(a[i-1][k-1]);
			if (scale) {
				for (k=l;k<=n;k++) {
					a[i-1][k-1] /= scale;
					s += a[i-1][k-1]*a[i-1][k-1];
				}
				f=a[i-1][l-1];
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				a[i-1][l-1]=f-g;
				for (k=l;k<=n;k++) rv1[k-1]=a[i-1][k-1]/h;
				for (j=l;j<=m;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[j-1][k-1]*a[i-1][k-1];
					for (k=l;k<=n;k++) a[j-1][k-1] += s*rv1[k-1];
				}
				for (k=l;k<=n;k++) a[i-1][k-1] *= scale;
			}
		}
		anorm=DMAX(anorm,(fabs(w[i-1])+fabs(rv1[i-1])));
	}
	for (i=n;i>=1;i--) {
		if (i < n) {
			if (g) {
				for (j=l;j<=n;j++) v[j-1][i-1]=(a[i-1][j-1]/a[i-1][l-1])/g;
				for (j=l;j<=n;j++) {
					s = 0.0;
					for (k=l;k<=n;k++) s += a[i-1][k-1]*v[k-1][j-1];
					for (k=l;k<=n;k++) v[k-1][j-1] += s*v[k-1][i-1];
				}
			}
			for (j=l;j<=n;j++) v[i-1][j-1]=v[j-1][i-1]=0.0;
		}
		v[i-1][i-1]=1.0;
		g=rv1[i-1];
		l=i;
	}
	for (i=IMIN(m,n);i>=1;i--) {
		l=i+1;
		g=w[i-1];
		for (j=l;j<=n;j++) a[i-1][j-1]=0.0;
		if (g) {
			g=1.0/g;
			for (j=l;j<=n;j++) {
				for (s=0.0,k=l;k<=m;k++) s += a[k-1][i-1]*a[k-1][j-1];
				f=(s/a[i-1][i-1])*g;
				for (k=i;k<=m;k++) a[k-1][j-1] += f*a[k-1][i-1];
			}
			for (j=i;j<=m;j++) a[j-1][i-1] *= g;
		} else for (j=i;j<=m;j++) a[j-1][i-1]=0.0;
		++a[i-1][i-1];
	}
	for (k=n;k>=1;k--) {
		for (its=1;its<=30;its++) {
			flag=1;
			for (l=k;l>=1;l--) {
				nm=l-1;
				if ((double)(fabs(rv1[l-1])+anorm) == anorm) {
					flag=0;
					break;
				}
				if ((double)(fabs(w[nm-1])+anorm) == anorm) break;
			}
			if (flag) {
				c=0.0;
				s=1.0;
				for (i=l;i<=k;i++) {
					f=s*rv1[i-1];
					rv1[i-1]=c*rv1[i-1];
					if ((double)(fabs(f)+anorm) == anorm) break;
					g=w[i-1];
					h=dpythag(f,g);
					w[i-1]=h;
					h=1.0/h;
					c=g*h;
					s = -f*h;
					for (j=1;j<=m;j++) {
						y=a[j-1][nm-1];
						z=a[j-1][i-1];
						a[j-1][nm-1]=y*c+z*s;
						a[j-1][i-1]=z*c-y*s;
					}
				}
			}
			z=w[k-1];
			if (l == k) {
				if (z < 0.0) {
					w[k-1] = -z;
					for (j=1;j<=n;j++) v[j-1][k-1] = -v[j-1][k-1];
				}
				break;
			}
			if (its == 30) {
				errmess("No convergence in 30 dsvdcmp iterations.");
				return;
			}
			x=w[l-1];
			nm=k-1;
			y=w[nm-1];
			g=rv1[nm-1];
			h=rv1[k-1];
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
			g=dpythag(f,1.0);
			f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
			c=s=1.0;
			for (j=l;j<=nm;j++) {
				i=j+1;
				g=rv1[i-1];
				y=w[i-1];
				h=s*g;
				g=c*g;
				z=dpythag(f,h);
				rv1[j-1]=z;
				c=f/z;
				s=h/z;
				f=x*c+g*s;
				g = g*c-x*s;
				h=y*s;
				y *= c;
				for (jj=1;jj<=n;jj++) {
					x=v[jj-1][j-1];
					z=v[jj-1][i-1];
					v[jj-1][j-1]=x*c+z*s;
					v[jj-1][i-1]=z*c-x*s;
				}
				z=dpythag(f,h);
				w[j-1]=z;
				if (z) {
					z=1.0/z;
					c=f*z;
					s=h*z;
				}
				f=c*g+s*y;
				x=c*y-s*g;
				for (jj=1;jj<=m;jj++) {
					y=a[jj-1][j-1];
					z=a[jj-1][i-1];
					a[jj-1][j-1]=y*c+z*s;
					a[jj-1][i-1]=z*c-y*s;
				}
			}
			rv1[l-1]=0.0;
			rv1[k-1]=f;
			w[k-1]=x;
		}
	}

	return;
}


vector<double> dsvbksb(const svd_t& svd, const vector<double>& b) {
	const matrix<double> &u = svd.u, &v = svd.v;
	const vector<double> &w = svd.s;
	
	int m = u.rows();
	int n = u.cols();
	int jj,j,i;
	double s;
	
	
	vector<double> tmp(n, 0.0);
	vector<double> x(n, 0.0);
	for (j=1;j<=n;j++) {
		s=0.0;
		if (w[j-1]) {
			for (i=1;i<=m;i++) s += u[i-1][j-1]*b[i-1];
			s /= w[j-1];
		}
		tmp[j-1]=s;
	}
	for (j=1;j<=n;j++) {
		s=0.0;
		for (jj=1;jj<=n;jj++) s += v[j-1][jj-1]*tmp[jj-1];
		x[j-1]=s;
	}
	
	return x;
}

