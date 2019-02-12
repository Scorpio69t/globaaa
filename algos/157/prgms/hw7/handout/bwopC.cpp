#define PI 3.141592653589793238
#include "mex.h"
#include "math.h"

template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

double sim(double *plan) {
    double dt=.1, friction=1, gravity=.1, mass[]={30, 10, 5, 10, 5, 10}, edgel[]={.5, .5, .5, .5, .9},
            edgesp[]={160, 180, 160, 180, 160}, edgef[]={8, 8, 8, 8, 8}, anglessp[]={20, 20, 10, 10}, anglesf[]={8, 8, 4, 4};
    int edge[5][2]={{0, 1},{1, 2},{0, 3},{3, 4},{0, 5}}, angles[4][2]={{4, 0},{4, 2},{0, 1},{2, 3}};
    double v[][6]={{0,0,0,0,0,0},{0,0,0,0,0,0}}, p[][6]={{0, 0, -.25, .25, .25, .15},{1, .5, 0, .5, 0, 1.9}};
	double spin = 0, maxspin = 0, lastang = 0;
    for (int j=0;j<20;j++) {
        for (int k=0;k<10;k++) {
            double t0,t1;
            double lambda=0.05+0.1*k;
            t0=(j>0?plan[2*j-2]:.5)*(1-lambda)+plan[2*j]*lambda; 
            t1=(j>0?plan[2*j-1]:0)*(1-lambda)+plan[2*j+1]*lambda;
			int contact[6]; for (int z = 0; z < 6; z++) { if (p[1][z] <= 0) { contact[z] = 1; spin = 0; p[1][z] = 0; } else contact[z] = 0; }
            double anglesl[]={-(2.8+t0), -(2.8-t0), -(1-t1)*.9, -(1+t1)*.9};
            double disp[2][5],dist[5],dispn[2][5]; 
            for (int z=0;z<5;z++) {
                disp[0][z]=p[0][edge[z][1]]-p[0][edge[z][0]];disp[1][z]=p[1][edge[z][1]]-p[1][edge[z][0]];
                dist[z]=sqrt(disp[0][z]*disp[0][z]+disp[1][z]*disp[1][z]);
                double inv=1/dist[z];
                dispn[0][z]=disp[0][z]*inv; dispn[1][z]=disp[1][z]*inv;
            }
            double dispv[2][5],distv[5];
            for (int z=0;z<5;z++) {
                dispv[0][z]=v[0][edge[z][1]]-v[0][edge[z][0]];dispv[1][z]=v[1][edge[z][1]]-v[1][edge[z][0]];
                distv[z]=2*(disp[0][z]*dispv[0][z]+disp[1][z]*dispv[1][z]);
            }
            double forceedge[2][5];
            for (int z=0;z<5;z++) {
                double c=(edgel[z]-dist[z])*edgesp[z]-distv[z]*edgef[z];
                forceedge[0][z]=c*dispn[0][z]; forceedge[1][z]=c*dispn[1][z];
            }
            double edgeang[5],edgeangv[5];
            for (int z=0;z<5;z++) {
                edgeang[z]=atan2(disp[1][z],disp[0][z]);
                edgeangv[z]=(dispv[0][z]*disp[1][z]-dispv[1][z]*disp[0][z])/(dist[z]*dist[z]);
            }
			{double inc = edgeang[4] - lastang; if (inc < -PI) inc += 2 * PI; if (inc>PI) inc -= 2 * PI; spin += inc; double spinc = spin - .005*(k + 10 * j); if (spinc > maxspin) maxspin = spinc; lastang = edgeang[4]; }
            double angv[4];
            for (int z=0;z<4;z++) {
                angv[z]=edgeangv[angles[z][1]]-edgeangv[angles[z][0]];
            }
            double angf[4];
            for (int z=0;z<4;z++) {
                double ang=edgeang[angles[z][1]]-edgeang[angles[z][0]]-anglesl[z]; if (ang>PI) ang-=2*PI; if (ang<-PI) ang+=2*PI;//not quite all the cases
                double m0=dist[angles[z][0]]/edgel[angles[z][0]],m1=dist[angles[z][1]]/edgel[angles[z][1]];
                angf[z]=ang*anglessp[z]-angv[z]*anglesf[z]*(m0<m1?m0:m1);
            }
            double edgetorque[2][5];
            for (int z=0;z<5;z++) {
                double inv=1/(dist[z]*dist[z]);
                edgetorque[0][z]=-disp[1][z]*inv; edgetorque[1][z]=disp[0][z]*inv;
            }
            for (int z=0;z<4;z++) {
                int i0=angles[z][0],i1=angles[z][1];
                forceedge[0][i0]+=angf[z]*edgetorque[0][i0]; forceedge[1][i0]+=angf[z]*edgetorque[1][i0];
                forceedge[0][i1]-=angf[z]*edgetorque[0][i1]; forceedge[1][i1]-=angf[z]*edgetorque[1][i1];
            }
            double f[][6]={{0,0,0,0,0,0},{0,0,0,0,0,0}};
            for (int z=0;z<5;z++) {
                int i0=edge[z][0],i1=edge[z][1];
                f[0][i0]-=forceedge[0][z]; f[1][i0]-=forceedge[1][z];
                f[0][i1]+=forceedge[0][z]; f[1][i1]+=forceedge[1][z];
            }
            for (int z=0;z<6;z++) {
                f[1][z]-=gravity*mass[z];
                double invm=1/mass[z];
                v[0][z]+=f[0][z]*dt*invm; v[1][z]+=f[1][z]*dt*invm;
                if (contact[z]) {
                    double fric;
                    if (v[1][z]<0) {fric=-v[1][z]; v[1][z]=0;} else fric=0;
                    int s=sign(v[0][z]); if (v[0][z]*s<fric*friction) v[0][z]=0; else v[0][z]-=fric*friction*s;
                }
				p[0][z] += v[0][z] * dt; p[1][z] += v[1][z] * dt;
			}
			if (contact[0] || contact[5]) return -p[0][0];
        }
    }
	return -p[0][0];
}

void mexFunction(
				 int nlhs, mxArray *plhs[],
				 int nrhs, const mxArray *prhs[])
{
    int nr;
	if (nrhs < 1) mexErrMsgTxt("Function needs an input");
	if (mxGetClassID(prhs[0])!=mxDOUBLE_CLASS) mexErrMsgTxt("Input must be of type 'double'");
    {int e=mxGetNumberOfElements(prhs[0]); nr=e/40; if (e!=40) mexErrMsgTxt("Input must have 40 elements!");}
    double *plans=(double*)mxGetData(prhs[0]);
    double *out=(double*)mxGetData(plhs[0]=mxCreateDoubleMatrix(1,nr,mxREAL));
	for (int i = 0; i < nr; i++) {
		double plan[40]; for (int j = 0; j < 40; j++) {
			double v = plans[40 * i + j]; if (v < -1) v = -1; if (v>1) v = 1; plan[j] = v;
		}
		out[i] = sim(plan);
	}
}
