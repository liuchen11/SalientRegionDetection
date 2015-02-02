#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include "base.h"
#include "libbmp.h"
#include "picture.h"

int main(int argc, char *argv[])
{
	if(argc<5){
		printf("Usage ./demo <inputfile> <output1> <output2> <k>");
		return 0;
	}
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	picture p=picture(argv[1],atoi(argv[4]));
	p.load();
	p.pixel2pos();
	p.loadlinks();
	printf("begin\n");
	p.merge();
	printf("OK\n");
	p.save();
	p.paint();
	p.pos2pixel();
	p.output(argv[2]);
	p.resume();
	p.calc();
	p.pos2pixel();
	p.output(argv[3]);
	p.display();
	return 0;
}
