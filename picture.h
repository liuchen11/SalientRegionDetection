#ifndef __PICTURE_H__
#define __PICTURE_H__

#include <GL/glut.h>
#include <vector>
#include "base.h"
#include "libbmp.h"
using namespace std;

class picture: public libbmp{
public:
	picture(char*filename,int rate);
	picture(GLubyte* pixel, GLint width, GLint height,int rate);
	~picture();
	void loadlinks();
	void merge();
	void paint();
	void calc();
	void save();
	void resume();
private:
	RGB **saved;
	int rate;
	int num_link;
	int* zonemap;
	int* feature;
	int pixel2index(PAIR p);
	PAIR index2pixel(int index);
	bool exist(int i,int j);
	void sortlinks(int b,int e);
	LINK* links;
	vector<vector<PAIR> > zones;
};

#endif