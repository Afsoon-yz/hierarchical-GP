#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void main()
{
	srand( time(NULL));
	FILE *f;
	int i = 0;
	int j = 150;
	int k = 50;

	double a = ((double)rand()/(double)RAND_MAX);
	double b = ((double)rand()/(double)RAND_MAX);
	double c = ((double)rand()/(double)RAND_MAX);
	double v = 0;

	double s1 = 0;
	double s2 = 0;
	double s3 = 0;
	f = fopen("trainnon.txt", "w");
	for (i=0 ; i<j ; i++)
	{
		v = ((double)rand()/(double)RAND_MAX);
		s1 = v;
		s2 = a+(b*v);
		s3 = a+(b*v)+(c*v*v);
		fprintf(f,"%f,%f,%f,%f\n",s1,s2,s3,v);
	}
	fclose(f);

	f = fopen("testnon.txt", "w");
	for (i=0 ; i<k ; i++)
	{
		v = ((double)rand()/(double)RAND_MAX);
		s1 = v;
		s2 = a+(b*v);
		s3 = a+(b*v)+(c*v*v);
		fprintf(f,"%f,%f,%f,%f\n",s1,s2,s3,v);
	}
	fclose(f);
}
