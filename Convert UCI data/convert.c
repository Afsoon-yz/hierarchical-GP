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
	double	d_number[19];
	FILE 	*d;
	FILE 	*tr;
	FILE 	*ts;

	d = fopen("Data/dataSet10.txt", "rt");
	tr= fopen("training/10training.txt","w");
	ts= fopen("testing/10testing.txt","w");

	while (fscanf(d,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&d_number[12],&d_number[4],&d_number[10],&d_number[8],&d_number[3],&d_number[11],&d_number[5],&d_number[2],&d_number[7],&d_number[9],&d_number[1],&d_number[6]) != EOF)
	{
		i++;
		if (i%2 == 1) fprintf(tr,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",d_number[1],d_number[2],d_number[3],d_number[4],d_number[5],d_number[6],d_number[7],d_number[8],d_number[9],d_number[10],d_number[11],d_number[12]);
		else          fprintf(ts,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",d_number[1],d_number[2],d_number[3],d_number[4],d_number[5],d_number[6],d_number[7],d_number[8],d_number[9],d_number[10],d_number[11],d_number[12]);
	}
	fclose(d);
	fclose(tr);
	fclose(ts);
}
