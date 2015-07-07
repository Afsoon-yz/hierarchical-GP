#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define number_of_features  13
#define number_of_operators 5
#define tournament_size 	2
#define population_size 	100
#define max_tree_size 		20
#define max_evolution_round 100
#define fitness_stall		0.1
#define crossover_rate		0.2
#define max_nondominated	200	
#define v_portion			0.2
#define training_file			"data/training/2training.txt"
#define testing_file			"data/testing/2testing.txt"
#define accuracy_file			"H/a02t100r.txt"
#define cost_file				"H/c02t100r.txt"
#define usage_file				"H/u02t100r.txt"
#define nondom_file				"H/n02t100r.txt"

#define True  1
#define False 0




struct node
{
	int		key_type;
	double	key_value;
  	struct node *left;
  	struct node *right;
};

struct root_node
{
	int	size;
	int	age;
	double cost;
	double fitness;
	struct node *root;
};

struct 	root_node population[population_size];
struct	root_node pop [number_of_features][population_size];
double	pop_variance[number_of_features+1];
int		pop_count[number_of_features];

int		changed[population_size];
int		dominated[population_size];
int		number_of_nondominated = population_size;

char 	feature_symbol[number_of_features][3] = {"S1"   , "S2"   , "S3"   , "S4"   , "S5"   , "S6"   , "S7"   , "S8"   , "S9"   , "S10"  , "S11"  , "S12"  , "S13"  };
double 	feature_code[number_of_features]      = {1.0    , 2.0    , 3.0    , 4.0    , 5.0    , 6.0    , 7.0    , 8.0    , 9.0    , 10.0   , 11.0   , 12.0   , 13.0   };
double	feature_cost[number_of_features]	  = {0.5441 , 0.4835 , 0.2578 , 0.234  , 0.2195 , 0.1826 , 0.1508 , 0.1456 , 0.1421 , 0.1299 , 0.1112 , 0.0625 , 0.0307 };

char 	operator_symbol[number_of_operators][2] = {"-" , "+" , "-" , "*" , "/" };
double	operator_code[number_of_operators]      = {1.0 , 2.0 , 3.0 , 4.0 , 5.0 };
int  	operator_req[number_of_operators]       = { 1  ,  2  ,  2  ,  2  ,  2  };

char 	equation_text[1000];
int		used[number_of_features+1];
int		current_round = 1;

struct node *create_constant_node()
{
	struct node *n_node = (struct node *)malloc(sizeof(struct node));
	n_node -> left = NULL;
	n_node -> right = NULL;
	n_node -> key_type = 3;
	n_node -> key_value = ((double)rand() * 10) / (double)RAND_MAX;
	return n_node;
}


struct node *create_feature_node()
{
	struct node *n_node = (struct node *)malloc(sizeof(struct node));
	n_node -> left = NULL;
	n_node -> right = NULL;
	n_node -> key_type = 2;
	if (current_round < number_of_features)	n_node -> key_value = ((rand() % (number_of_features-current_round)) + current_round);
	else n_node -> key_value = current_round;
	return n_node;
}


struct node *create_operator_node()
{
	struct node *n_node = (struct node *)malloc(sizeof(struct node));
	n_node -> left = NULL;
	n_node -> right = NULL;
	n_node -> key_type = 1;
	n_node -> key_value = (rand() % number_of_operators) + 1;
	return n_node;
}


struct node *create_random_node()
{
	struct node *n_node;
	int node_type = (rand() % 3) + 1;
	int key_val = 0;

	switch(node_type)
	{
		case 1:
			n_node = create_operator_node();
			if (rand()%2 == 0)
			{
				n_node -> left = create_feature_node();
			}
			else
			{
				n_node -> left = create_constant_node();
			}

			key_val = (int)n_node -> key_value;
			if (operator_req[key_val-1]==2)
			{
				if (rand()%2 == 0)
				{
					n_node -> right = create_feature_node();
				}
				else
				{
					n_node -> right = create_constant_node();
				}
			}
			break;

		case 2:
			n_node = create_feature_node();
			break;

		case 3:
			n_node = create_constant_node();
			break;
	}
	return n_node;
}


void get_the_equation_text(struct node *tree_node)
{
	char number[15];
	if (tree_node != NULL)
	{	
		if(tree_node -> key_type == 1) strcat(equation_text , "(");
		//left
		get_the_equation_text(tree_node -> left);
		//middle
		if (tree_node -> key_type == 1) 
		{
			strcat(equation_text , operator_symbol[(int)tree_node -> key_value -1]);
		}
		else if (tree_node -> key_type == 2)
 		{
			strcat(equation_text , feature_symbol[(int)tree_node -> key_value -1]);
		}
		else if (tree_node -> key_type == 3) 
		{
			snprintf(number,15,"%f",tree_node -> key_value);
			strcat(equation_text , number);
		}
		//right
		get_the_equation_text(tree_node -> right);
		if(tree_node -> key_type == 1) strcat(equation_text , ")");
	}
}


void destroy_tree(struct node *tree_node)
{
  if( tree_node != NULL )
  {
      destroy_tree(tree_node -> left);
      destroy_tree(tree_node -> right);
      free( tree_node );
  }
}


struct node *copy_tree(struct node *tree_node)
{
	if (tree_node == NULL) return NULL;
	struct node *n_node = (struct node *)malloc(sizeof(struct node));
	n_node -> left = copy_tree(tree_node -> left);
	n_node -> right = copy_tree(tree_node -> right);
	n_node -> key_type = tree_node -> key_type;
	n_node -> key_value = tree_node -> key_value;
	return n_node;
}


int get_tree_size(struct node *tree_node)
{
	int i = 0;
	if (tree_node == NULL)	return 0;
	i= 1 + get_tree_size(tree_node -> left) + get_tree_size(tree_node -> right);
	return i;
}


struct node *find_node(int i , int *ci , struct node *tree_node)
{
	struct node *n_node = NULL;
	int current_i = *ci;
	current_i ++;
	if (i == current_i) return tree_node;
	if (tree_node -> left != NULL)	n_node = find_node(i , &current_i , tree_node -> left);
	if ((n_node == NULL) && (tree_node -> right != NULL))	n_node = find_node(i , &current_i , tree_node -> right);
	(*ci) = current_i;
	return n_node;
}


int tournament_selection()
{
	int 	k = 0;
	int 	m = 0;
	int 	i = 0;
	int 	ind = 0;
	int 	selected = 0;
	double 	best = 10000;
	int		bound = 1;
	
	if (number_of_nondominated < tournament_size) bound=number_of_nondominated;
	else	bound = tournament_size;

	while (i <= bound)
	{
		k = 0;
		m = 0;
		ind = 0;
		int	j = (rand() % number_of_nondominated) + 1;
		while(k<j && m<population_size)
		{
			if (dominated[m] == False)
			{
				k ++;
				ind = m;
			}
			m++;
		}
		if (population[ind].fitness < best)
		{
			selected = ind;
			i ++;
		}
	}
	return selected;
}


void mutate (int j)
{
	int p = tournament_selection();
	population[j].root = copy_tree(population[p].root);
	int ind = (rand() % get_tree_size(population[j].root)) + 1;

	struct node *n_node = NULL;
	struct node *e_node = NULL;
	if ( ind == 1 )
	{
		destroy_tree(population[j].root);
		population[j].root = create_random_node();
		population[j].age = 0;
	}
	else
	{
		int i = 0;
		n_node = find_node(ind,&i,population[j].root);
		e_node = create_random_node();

		destroy_tree(n_node -> left);
		destroy_tree(n_node -> right);
		n_node -> key_type = e_node -> key_type;
		n_node -> key_value = e_node -> key_value;
		n_node -> left = e_node -> left;
		n_node -> right = e_node -> right;
		free(e_node);
	}
}


void crossover (int j)
{	
	struct node *p1_node = NULL;
	struct node *p2_node = NULL;
	struct node *t1_node = NULL;

	int point1 = 0;
	int point2 = 0;
	int p1 = tournament_selection();
	int p2 = p1;
	int i = 0;

	if(number_of_nondominated > 1) while (p1 == p2)	p2 = tournament_selection();
	point1 = (rand() % population[p1].size) + 1;
	point2 = (rand() % population[p2].size) + 1;
	population[j].root = copy_tree(population[p1].root);
	p1_node = find_node(point1,&i,population[j].root);
	i = 0;
	p2_node = find_node(point2,&i,population[p2].root);
	t1_node = copy_tree(p2_node);
	destroy_tree(p1_node -> left);
	destroy_tree(p1_node -> right);
	p1_node -> key_type = t1_node -> key_type;
	p1_node -> key_value = t1_node -> key_value;
	p1_node -> left = t1_node -> left;
	p1_node -> right = t1_node -> right;
	free(t1_node);
}


double evaluate_on_data(double data[number_of_features+2] , struct node *tree_node)
{
	double l_value = 0;
	double r_value = 0;
	
	if (tree_node -> left != NULL)	l_value = evaluate_on_data(data , tree_node -> left);
	if (tree_node -> right != NULL)	r_value = evaluate_on_data(data , tree_node -> right);

	switch (tree_node -> key_type)
	{
		case 3:
			return tree_node -> key_value;
		case 2:
			used[(int)tree_node -> key_value]=1;
			return data[(int)tree_node -> key_value];
		case 1:
			switch ((int)tree_node -> key_value)
			{
				case 1:
					return -l_value;
				case 2:
					return l_value + r_value;
				case 3:
					return l_value - r_value;
				case 4:
					return l_value * r_value;
				case 5:
					if (r_value == 0) return 0;
					return l_value / r_value;
			}
	}
}


double evaluate_on_test(int j)
{
	int		i = 0;
	double	t_number = 0;
	double	d_number[number_of_features+2];
	FILE 	*data_file;
	data_file = fopen (training_file, "rt");
	
	while (fscanf(data_file,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&d_number[1],&d_number[2],&d_number[3],&d_number[4],&d_number[5],&d_number[6],&d_number[7],&d_number[8],&d_number[9],&d_number[10],&d_number[11],&d_number[12],&d_number[13],&d_number[14]) != EOF)
	{
		i++;
		t_number += fabs(d_number[number_of_features+1] - evaluate_on_data(d_number , population[j].root));
	}
	fclose(data_file);
	return t_number / i;
}


void initialize_population()
{
	int i;
	for(i=0 ; i < population_size ; i++)
	{
		population[i].root = create_random_node();
		population[i].size = get_tree_size(population[i].root);
		population[i].age = 0;
		population[i].cost = 0;
		population[i].fitness = 0;
		changed[i] = True;
		dominated[i] = False;
	}
}


double generation_individual_update()
{
	int 	i = 0;
	int		best_index = 0;
	double  best_so_far = 100000;
	for(i=0 ; i < population_size ; i++)
	{
		if (changed[i] == True) 
		{
			population[i].fitness = evaluate_on_test(i);
			population[i].size = get_tree_size(population[i].root);
			changed[i] = False;
		}
		population[i].age ++;
		if (population[i].fitness < best_so_far) 
		{
			best_so_far = population[i].fitness;
			best_index = i;
		}
	}
	return best_so_far;
}


void find_pareto_front()
{
	int i = 0;
	int j = 0;
	char text1[500];
	number_of_nondominated = population_size;
	FILE *f_results = fopen(nondom_file, "a");
	for (i=0 ; i<population_size ; i++)
	{
		dominated[i] = False;
		for (j=i+1 ; j<population_size ; j++)
			//if ((population[i].fitness >= population[j].fitness) && (population[i].size >= population[j].size) && (population[i].age >= population[j].age))
			if ((population[i].fitness >= population[j].fitness) && (population[i].size*population[i].age >= population[j].size*population[j].age))
			{
				dominated[i] = True;
				number_of_nondominated --;
				break;
			}
			else
			{
				strcpy(equation_text,"");
				get_the_equation_text(population[i].root);
				strcpy(text1,equation_text);
				strcpy(equation_text,"");
				get_the_equation_text(population[j].root);

				if(strcmp(text1,equation_text) == 0)
				{
					dominated[i] = True;
					number_of_nondominated --;
					break;
				}
			}
	}

	//fprintf(f_results,"%d\n",number_of_nondominated);
	fclose(f_results);
}
void final_pareto_front()
{
	int i = 0;
	int j = 0;
	int maxp = 0;
	double maxe = 0;
	char text1[1000];
	number_of_nondominated = population_size;
	for (i=0 ; i<population_size ; i++)	dominated[i] = False;
	for (i=0 ; i<population_size ; i++)
	{
		for (j=0 ; j<population_size ; j++)
		{
			if ((i != j)&&(dominated[i]==False)&&(dominated[j]==False)&&(population[i].fitness >= population[j].fitness) && (population[i].size >= population[j].size))
			{
				dominated[i] = True;
				number_of_nondominated --;
				break;
			}
		}
	}
	
	if (number_of_nondominated > (population_size*0.02))
		for (i=1 ; i<=(int)(number_of_nondominated-(population_size*0.02)) ; i++)	
		{	
			maxe = 0;
			maxp = 0;
			for (j=0 ; j<population_size ; j++)
				if ((dominated[j]==False)&&(population[j].fitness > maxe))
				{
					maxe = population[j].fitness;
					maxp = j;
				}
			dominated[maxp] = true;
		}

}

void fill_dominated_trees()
{
	int 	i = 0;
	double  frac = 0;
	for (i=0 ; i<population_size ; i++)
	{
		if (dominated[i] == True)
		{
			frac = (double)rand() / (double)RAND_MAX ;
			if (frac < crossover_rate)	crossover(i);
			else	mutate(i);
			changed[i] = True;
		}
	}
}


void find_a_layer()
{
	int i=0;
	int generation = 1;
	double best_fitness = 100000;
	srand( time(NULL));
	FILE *f_results;
	initialize_population();
	while ((generation < max_evolution_round) && (number_of_nondominated <= max_nondominated)) // && (best_fitness > fitness_stall)
	{
		//printf("========== ROUND %d ==========\n",generation);
		best_fitness = generation_individual_update();
		//printf(">> UPDATED\n");
		find_pareto_front();
		//printf(">> FOUND\n");
		fill_dominated_trees();
		//printf(">> FILLED\n");
		generation ++;
	}
	best_fitness = generation_individual_update();
	final_pareto_front();
}


void find_three_layer()
{
	int i=0;
	int j=0;
	int k=0;
	FILE *f_results;
	f_results = fopen(nondom_file, "a");
	for(i=1 ; i<=number_of_features ; i++)
	{
		current_round = i;
		printf("\n\n###################### ROUND %d ########################\n",i);
		find_a_layer();
		final_pareto_front();
		k=0;
		for(j=0 ; j<population_size ; j++)
		{
			if(dominated[j]==False)
			{
				pop[i][k].root = population[j].root;
				pop[i][k].fitness = population[j].fitness;
				pop[i][k].cost = population[j].cost;
				k++;
			}
			else
				destroy_tree(population[j].root);
		}
		pop_count[current_round]=k;
		fprintf(f_results,"\nthe number of solutions = %d\n",pop_count[current_round]);
	}
	fclose(f_results);
}

double find_variance(int i)
{
	int j=0;
	double mean = 0;
	double vari = 0;

	for(j=0 ; j<pop_count[i] ; j++) mean +=pop[i][j].fitness;
	mean = mean/pop_count[i];
	for(j=0 ; j<pop_count[i] ; j++) vari +=(pop[i][j].fitness-mean)*(pop[i][j].fitness-mean);
	return vari/pop_count[i];
}

void simulation_phase()
{
	int		i = 0;
	int 	j =0;
	int		k = 0;
	int		current_layer = 0;
	double	usage_per[number_of_features+2];
	double	t_number = 0;
	double	d_number[number_of_features+2];
	double	cost = 0;
	double  mean=0;
	double  partial_cost;
	double  var =0;
	double	total_cost =0;
	double	total_mean =0;
	double	calc = 0;
	FILE 	*data_file;
	FILE	*f_results;
	FILE	*af;
	FILE	*cf;
	FILE	*uf;


	data_file = fopen (testing_file, "rt");
	af = fopen(accuracy_file,"a");
	cf = fopen(cost_file,"a");
	uf = fopen(usage_file,"a");
	for (k=1 ; k<=number_of_features ; k++)	usage_per[k] = 0;
	
	while (fscanf(data_file,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&d_number[1],&d_number[2],&d_number[3],&d_number[4],&d_number[5],&d_number[6],&d_number[7],&d_number[8],&d_number[9],&d_number[10],&d_number[11],&d_number[12],&d_number[13],&d_number[14]) != EOF)
	{
		i++;
		for (k=1 ; k<=number_of_features ; k++)	used[k]=0;
		var = 10000;
		current_layer = number_of_features+1;
		pop_variance[number_of_features+1] = 0;
		while ((var > pop_variance[current_layer]) && (current_layer > 1))
		{
			current_layer --; 
			printf("layer %d\n",current_layer);
			for(j=0 ; j<pop_count[current_layer] ; j++) pop[current_layer][j].fitness = fabs(d_number[number_of_features+1] - evaluate_on_data(d_number , pop[current_layer][j].root));
			var = find_variance(current_layer)*v_portion;
		}
		mean = 0;
		for(j=0 ; j<pop_count[current_layer] ; j++) mean +=pop[current_layer][j].fitness;
		mean = mean/pop_count[current_layer];
		partial_cost = 0;
		for (k=1 ; k<=number_of_features ; k++)	
			if(used[k]==1)	
			{
				partial_cost += feature_cost[k-1];
				usage_per[k]++;
			}
		total_cost += partial_cost;
		total_mean += mean;
		printf("%dth test point\n",i);
	}
	fprintf(af,"%f\n",total_mean/i);
	fprintf(cf,"%f\n",total_cost/i);
	for (k=1 ; k<number_of_features ; k++)	fprintf(uf,"%f,",usage_per[k]/i);
	fprintf(uf,"%f\n",usage_per[number_of_features]/i);
	fclose(data_file);
	fclose(af);
	fclose(cf);
	fclose(uf);
}


double boundary_variance(int ind)
{
	int i=0;
	int j=0;
	double	t_number = 0;
	double	d_number[number_of_features+2];
	FILE 	*data_file;
	data_file = fopen (training_file, "rt");
	while (fscanf(data_file,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&d_number[1],&d_number[2],&d_number[3],&d_number[4],&d_number[5],&d_number[6],&d_number[7],&d_number[8],&d_number[9],&d_number[10],&d_number[11],&d_number[12],&d_number[13],&d_number[14]) != EOF)
	{
		i++;
		for (j=0 ; j<pop_count[ind] ; j++)
		{
			pop[ind][j].fitness = fabs(d_number[number_of_features+1] - evaluate_on_data(d_number , pop[ind][j].root));
		}
		t_number += find_variance(ind);
	}
	fclose(data_file);
	return t_number/i;
}

void main()
{
	int i=0;
	FILE *f_results;
	find_three_layer();
	for(i=1 ; i<=number_of_features ; i++) 
	{
		pop_variance[i] = boundary_variance(i);
		printf("found %dth boundary\n",i);
	}
	simulation_phase();
}






