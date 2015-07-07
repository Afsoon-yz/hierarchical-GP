#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void main()
{
	int		i = 0;
	double	t_number = 0;
	double	d_number[22];
	double	total = 0;
	FILE 	*d;
	FILE 	*tr;

	d = fopen("aa.txt", "rt");
	tr= fopen("a.txt","a");
	
	while (fscanf(d,"%lf,%lf,%lf,%lf",&d_number[1],&d_number[2],&d_number[3],&d_number[4]) != EOF)
	{
		i++;
		total += d_number[4];
	}

	fprintf(tr,"%lf\n",total/i);
	fclose(d);
	fclose(tr);
}
