#include <cmath>
#include "base.h"

RGB::RGB(){R=0,G=0,B=0;}

RGB::RGB(int b,int g,int r){R=r,G=g,B=b;}

double disRGB(RGB c1,RGB c2){
	double square=pow((c1.R-c2.R),2)+pow((c1.G-c2.G),2)+pow((c1.B-c2.B),2);
	return sqrt(square);
}

PAIR::PAIR(int i,int j){this->i=i,this->j=j;}

PAIR::PAIR(){i=-1,j=-1;}

bool PAIR::operator==(PAIR tocompare){return i==tocompare.i&&j==tocompare.j;}

PAIR plusPAIR(PAIR v1,PAIR v2){return PAIR(v1.i+v2.i,v1.j+v2.j);}

LINK::LINK(){p1=-1,p2=-1,weight=-1;}

LINK::LINK(int p1,int p2,double weight){
	this->p1=p1;
	this->p2=p2;
	this->weight=weight;
}