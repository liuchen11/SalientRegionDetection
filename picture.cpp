#include <GL/glut.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "base.h"
#include "libbmp.h"
#include "picture.h"
using namespace std;

int search(vector<int> p,int v){
	for(int i=0;i<p.size();i++){
		if(p[i]==v)
			return i;
	}
	return -1;
}

picture::picture(char* filename,int rate):libbmp(filename){
	this->rate=rate;
	num_link=-1;
	links=NULL;
	zonemap=NULL;
	feature=NULL;
	saved=NULL;
	zones=vector<vector<PAIR> >();
}

picture::picture(GLubyte* pixel, GLint width, GLint height,int rate):libbmp(pixel,width,height){
	this->rate=rate;
	num_link=-1;
	links=NULL;
	feature=NULL;
	saved=NULL;
	zones=vector<vector<PAIR> >();
}

picture::~picture(){
	if(saved!=NULL){
		for(int i=0;i<height;i++)
			if(saved[i]!=NULL)
				delete[] saved[i];
		delete[] saved;
	}
	if(zonemap!=NULL)
		delete[] zonemap;
	if(feature!=NULL)
		delete[] feature;
	if(links!=NULL)
		delete[] links;
}

int picture::pixel2index(PAIR p){return p.i*width+p.j;}

PAIR picture::index2pixel(int index){return PAIR(index/width,index%width);}

bool picture::exist(int i,int j){
	if(i<0||i>=height)
		return false;
	if(j<0||j>=width)
		return false;
	return true;
}

void picture::sortlinks(int b,int e){
	if(b>=e)
		return;
	LINK chosen=links[b];
	int l=b,r=e;
	while(l<r){
		while(l<r&&links[r].weight>=chosen.weight)
			r--;
		links[l]=links[r];
		while(l<r&&links[l].weight<=chosen.weight)
			l++;
		links[r]=links[l];
	}
	links[l]=chosen;
	sortlinks(b,l-1);
	sortlinks(r+1,e);
}

void picture::loadlinks(){
	num_link=(width-1)*height+width*(height-1)+2*(width-1)*(height-1);
	if(links!=NULL)
		delete[] links;
	links=new LINK[num_link];

	int index=0;
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			int p1=pixel2index(PAIR(i,j));
			RGB basic=pos[i][j];
			if(exist(i,j+1)){
				int p2=pixel2index(PAIR(i,j+1));
				double w=disRGB(basic,pos[i][j+1]);
				links[index]=LINK(p1,p2,w);
				index++;
			}
			if(exist(i+1,j-1)){
				int p2=pixel2index(PAIR(i+1,j-1));
				double w=disRGB(basic,pos[i+1][j-1]);
				links[index]=LINK(p1,p2,w);
				index++;
			}
			if(exist(i+1,j)){
				int p2=pixel2index(PAIR(i+1,j));
				double w=disRGB(basic,pos[i+1][j]);
				links[index]=LINK(p1,p2,w);
				index++;
			}
			if(exist(i+1,j+1)){
				int p2=pixel2index(PAIR(i+1,j+1));
				double w=disRGB(basic,pos[i+1][j+1]);
				links[index]=LINK(p1,p2,w);
				index++;
			}
		}
	}
	num_link=index;
	sortlinks(0,num_link-1);
}

void picture::merge(){
	if(zonemap!=NULL)
		delete[] zonemap;
	zonemap=new int[width*height];
	double *zoneint=new double[width*height];
	int *zonesize=new int[width*height];
	for(int i=0;i<width*height;i++){
		zonemap[i]=i;
		zoneint[i]=0;
		zonesize[i]=1;
	}
	for(int i=0;i<num_link;i++){
		printf("%d-%d\r",i,num_link);
		LINK chosen=links[i];
		int p1=chosen.p1;
		int p2=chosen.p2;
		int zone1=zonemap[p1];
		int zone2=zonemap[p2];
		if(zone1==zone2)
			continue;
		double int1=zoneint[zone1];
		double int2=zoneint[zone2];
		double dis1=int1+rate/zonesize[zone1];
		double dis2=int2+rate/zonesize[zone2];
		double dis=dis1<dis2?dis1:dis2;
		if(dis>chosen.weight){
			for(int j=0;j<width*height;j++){
				if(zonemap[j]==zone2)
					zonemap[j]=zone1;
			}
			zoneint[zone1]=chosen.weight;
			zonesize[zone1]+=zonesize[zone2];
		}
	}

	vector<int> convert=vector<int>();
	for(int i=0;i<width*height;i++){
		int entry=search(convert,zonemap[i]);
		if(entry<0){
			convert.push_back(zonemap[i]);
			entry=zones.size();
			zones.push_back(vector<PAIR>());
		}
		zones[entry].push_back(index2pixel(i));
	}

	delete[] zoneint;
	delete[] zonesize;
	printf("\n");
}

void picture::paint(){
	int *red=new int[50];
	int *green=new int[50];
	int *blue=new int[50];
	for(int i=0;i<50;i++){
		srand(time(0)+i);
		red[i]=rand()%256;
		srand(time(0)-i*3);
		green[i]=rand()%256;
		srand(time(0)-i*i);
		blue[i]=rand()%256;
	}
	for(int i=0;i<width*height;i++){
		int zone=zonemap[i]%50;
		PAIR p=index2pixel(i);
		pos[p.i][p.j].R=red[zone];
		pos[p.i][p.j].G=green[zone];
		pos[p.i][p.j].B=blue[zone];
	}
	delete[] red;
	delete[] green;
	delete[] blue;
}

void picture::calc(){
	double *centerI=new double[zones.size()];
	double *centerJ=new double[zones.size()];
	feature=new int[zones.size()];
	for(int i=0;i<zones.size();i++){
		centerI[i]=0,centerJ[i]=0;
		for(int j=0;j<zones[i].size();j++){
			centerI[i]+=zones[i][j].i;
			centerJ[i]+=zones[i][j].j;
		}
		centerI[i]/=zones[i].size();
		centerJ[i]/=zones[i].size();
	}
	for(int i=0;i<zones.size();i++){
		feature[i]=0;
		PAIR sample1=zones[i][0];
		RGB c1=pos[sample1.i][sample1.j];
		for(int j=0;j<zones.size();j++){
			if(i!=j){
				double dis=sqrt(pow(centerI[i]-centerI[j],2)+pow(centerJ[i]-centerJ[j],2));
				PAIR sample2=zones[j][0];
				RGB c2=pos[sample2.i][sample2.j];
				feature[i]+=zones[j].size()*disRGB(c1,c2)/dis;
			}
		}
		feature[i]/=(width*height-zones[i].size());
	}
	double max=0;
	for(int i=0;i<zones.size();i++){
		if(feature[i]>max){
			max=feature[i];
		}
	}
	for(int i=0;i<zones.size();i++){
		int f=(feature[i]*255)/max;
		for(int j=0;j<zones[i].size();j++){
			PAIR p=zones[i][j];
			pos[p.i][p.j]=RGB(f,f,f);
		}
	}
	delete[] centerI;
	delete[] centerJ;
}

void picture::save(){
	saved=pos;
	pos=new RGB*[height];
	for(int i=0;i<height;i++)
		pos[i]=new RGB[width];
}

void picture::resume(){
	pos=saved;
}