#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define OPERATOR	1
#define FEATURE		2
#define	CONSTANT	3
#define COMPARISON	4

#define number_of_features 		7
#define number_of_operators 	6
#define number_of_comparisons 	6
#define tournament_size 		2
#define population_size 		100
#define max_tree_size 			20
#define max_evolution_round 	3500
#define fitness_stall			4.3
#define crossover_rate			0.2
#define max_nondominated		100	
#define training_file			"data/training/1training.txt"
#define testing_file			"data/testing/1testing.txt"
#define accuracy_file			"NH/a1nh500.txt"
#define cost_file				"NH/c1nh500.txt"
#define usage_file				"NH/u1nh500.txt"
#define nondom_file				"NH/n1nh500.txt"
#define True  1
#define False 0

struct node
{
	int		key_type;
	double	key_value;
  	struct node *left;
  	struct node *right;
	struct node *middle;
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
struct	root_node pop[population_size];
int		pop_count;
int		changed[population_size];
int		dominated[population_size];
int		number_of_nondominated = population_size;
    
char 	feature_symbol[number_of_features][3] = {"S1"   , "S2"   , "S3"   , "S4"   , "S5"   , "S6"   , "S7"  };
double 	feature_code[number_of_features]      = {1.0    , 2.0    , 3.0    , 4.0    , 5.0    , 6.0    , 7.0   };
double	feature_cost[number_of_features]	  = {0.6918 , 0.6467 , 0.6012 , 0.5951 , 0.3356 , 0.3175 , 0.1766};

char 	operator_symbol[number_of_operators][3] = {"-" , "+" , "-" , "*" , "/" , "if"};
double	operator_code[number_of_operators]      = {1.0 , 2.0 , 3.0 , 4.0 , 5.0 ,  6.0};
int  	operator_req[number_of_operators]       = { 1  ,  2  ,  2  ,  2  ,  2  ,   2 };

char 	comparison_symbol[number_of_operators][3] = {"==" , "!=" , ">" , "<" , ">=" , "<="};
double	comparison_code[number_of_operators]      = {1.0  , 2.0  , 3.0 , 4.0 , 5.0  ,  6.0};
int  	comparison_req[number_of_operators]       = { 2   ,  2   ,  2  ,  2  ,  2   ,   2};

char 	equation_text[500];
int		used[number_of_features];

struct node *create_constant_node()
{
	struct node *n_node = (struct node *)malloc(sizeof(struct node));
	n_node -> left = NULL;
	n_node -> right = NULL;
	n_node -> middle = NULL;
	n_node -> key_type = 3;
	n_node -> key_value = ((double)rand()/(double)RAND_MAX);
	//n_node -> key_value = ((double)rand() * 10) / (double)RAND_MAX;
	return n_node;
}


struct node *create_feature_node()
{
	struct node *n_node = (struct node *)malloc(sizeof(struct node));
	n_node -> left = NULL;
	n_node -> right = NULL;
	n_node -> middle = NULL;
	n_node -> key_type = 2;
	n_node -> key_value = (rand() % number_of_features) + 1;
	return n_node;
}

struct node *create_comparison_node()
{
	struct node *n_node = (struct node *)malloc(sizeof(struct node));
	n_node -> left = NULL;
	n_node -> right = NULL;
	n_node -> middle = NULL;
	n_node -> key_type = 4;
	n_node -> key_value = (rand() % number_of_comparisons) + 1;
	
	if (rand()%2 == 0)
	{
		n_node -> left = create_feature_node();
	}
	else
	{
		n_node -> left = create_constant_node();
	}

	if (comparison_req[(int) n_node -> key_value - 1] == 2)
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
	return n_node;
}


struct node *create_operator_node()
{
	struct node *n_node = (struct node *)malloc(sizeof(struct node));
	n_node -> left = NULL;
	n_node -> right = NULL;
	n_node -> middle = NULL;
	n_node -> key_type = 1;
	n_node -> key_value = (rand() % number_of_operators) + 1;
	if (n_node -> key_value == 6) n_node -> middle = create_comparison_node();
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
		switch (tree_node -> key_type)
		{
			case 1: 
				strcat(equation_text , "(");
				if(tree_node -> key_value == 1) strcat(equation_text , operator_symbol[(int)tree_node -> key_value -1]);
				if(tree_node -> key_value == 6) 
				{
					strcat(equation_text , operator_symbol[(int)tree_node -> key_value -1]);
					get_the_equation_text(tree_node -> middle);
					strcat(equation_text , " then ");
				}
				get_the_equation_text(tree_node -> left);
				if(tree_node -> key_value == 1) strcat(equation_text , "");
				else if(tree_node -> key_value == 6) strcat(equation_text , " else ");
				else strcat(equation_text , operator_symbol[(int)tree_node -> key_value -1]);
				get_the_equation_text(tree_node -> right);
				strcat(equation_text , ")");
				break;
			case 2: 
				strcat(equation_text , feature_symbol[(int)tree_node -> key_value -1]);
				break;
			case 3: 
				snprintf(number,15,"%f",tree_node -> key_value);
				strcat(equation_text , number);
				break;
			case 4: 
				strcat(equation_text , "(");
				get_the_equation_text(tree_node -> left);
				strcat(equation_text , comparison_symbol[(int)tree_node -> key_value -1]);
				get_the_equation_text(tree_node -> right);
				strcat(equation_text , ")");
				break;
		}
	}
}


void destroy_tree(struct node *tree_node)
{
	if( tree_node != NULL )
	{
		destroy_tree(tree_node -> left);
		destroy_tree(tree_node -> right);
		destroy_tree(tree_node -> middle);
		free( tree_node );
  }
}


struct node *copy_tree(struct node *tree_node)
{
	if (tree_node == NULL) return NULL;
	struct node *n_node = (struct node *)malloc(sizeof(struct node));
	n_node -> left = copy_tree(tree_node -> left);
	n_node -> right = copy_tree(tree_node -> right);
	n_node -> middle = copy_tree(tree_node -> middle);
	n_node -> key_type = tree_node -> key_type;
	n_node -> key_value = tree_node -> key_value;
	return n_node;
}


int get_tree_size(struct node *tree_node)
{
	int i = 0;
	if (tree_node == NULL)	return 0;
	i= 1 + get_tree_size(tree_node -> left) + get_tree_size(tree_node -> right) + get_tree_size(tree_node -> middle);
	return i;
}


struct node *find_node(int i , int *ci , struct node *tree_node)
{
	struct node *n_node = NULL;
	int current_i = *ci;
	current_i ++;
	if (i == current_i) return tree_node;
	if (tree_node -> middle != NULL) 						n_node = find_node(i , &current_i , tree_node -> middle);
	if ((n_node == NULL) && (tree_node -> left != NULL))	n_node = find_node(i , &current_i , tree_node -> left);
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

	while (i <= tournament_size)
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
		destroy_tree(n_node -> middle);
		n_node -> key_type = e_node -> key_type;
		n_node -> key_value = e_node -> key_value;
		n_node -> left = e_node -> left;
		n_node -> right = e_node -> right;
		n_node -> middle = e_node -> middle;
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

	while (p1 == p2)	p2 = tournament_selection();
	point1 = (rand() % population[p1].size) + 1;
	point2 = (rand() % population[p2].size) + 1;
	population[j].root = copy_tree(population[p1].root);
	p1_node = find_node(point1,&i,population[j].root);
	i = 0;
	p2_node = find_node(point2,&i,population[p2].root);
	t1_node = copy_tree(p2_node);
	destroy_tree(p1_node -> left);
	destroy_tree(p1_node -> right);
	destroy_tree(p1_node -> middle);
	p1_node -> key_type = t1_node -> key_type;
	p1_node -> key_value = t1_node -> key_value;
	p1_node -> left = t1_node -> left;
	p1_node -> right = t1_node -> right;
	p1_node -> middle = t1_node -> middle;
	free(t1_node);
}


double evaluate_on_data(double data[number_of_features+2] , struct node *tree_node)
{
	double l_value = 0;
	double r_value = 0;
	double m_value = 0;

	switch (tree_node -> key_type)
	{
		case 4:
			if (tree_node -> left   != NULL)	l_value = evaluate_on_data(data , tree_node -> left);
			if (tree_node -> right  != NULL)	r_value = evaluate_on_data(data , tree_node -> right);
			switch((int)tree_node -> key_value)
			{
				case 1:
					return l_value == r_value;
				case 2:
					return l_value != r_value;
				case 3:
					return l_value > r_value;
				case 4:
					return l_value < r_value;
				case 5:
					return l_value >= r_value;
				case 6:
					return l_value <= r_value;
			}
		case 3:
			return tree_node -> key_value;
		case 2:
			used[(int)tree_node -> key_value] = 1;
			return data[(int)tree_node -> key_value];
		case 1:
			switch ((int)(tree_node -> key_value-1))
			{
				case 1:
					if (tree_node -> left   != NULL)	l_value = evaluate_on_data(data , tree_node -> left);
					return -l_value;
				case 2:
					if (tree_node -> left   != NULL)	l_value = evaluate_on_data(data , tree_node -> left);
					if (tree_node -> right  != NULL)	r_value = evaluate_on_data(data , tree_node -> right);
					return l_value + r_value;
				case 3:
					if (tree_node -> left   != NULL)	l_value = evaluate_on_data(data , tree_node -> left);
					if (tree_node -> right  != NULL)	r_value = evaluate_on_data(data , tree_node -> right);
					return l_value - r_value;
				case 4:
					if (tree_node -> left   != NULL)	l_value = evaluate_on_data(data , tree_node -> left);
					if (tree_node -> right  != NULL)	r_value = evaluate_on_data(data , tree_node -> right);
					return l_value * r_value;
				case 5:
					if (tree_node -> left   != NULL)	l_value = evaluate_on_data(data , tree_node -> left);
					if (tree_node -> right  != NULL)	r_value = evaluate_on_data(data , tree_node -> right);
					if (r_value == 0) return 0;
					return l_value / r_value;
				case 6:
					if (tree_node -> middle != NULL)	m_value = evaluate_on_data(data , tree_node -> middle);
					if (m_value > 0) 
					{
						if (tree_node -> left   != NULL)	l_value = evaluate_on_data(data , tree_node -> left);
						return l_value;
					}
					else
					{
						if (tree_node -> right  != NULL)	r_value = evaluate_on_data(data , tree_node -> right);
						return r_value;
					}
			}
	}
}


double evaluate_on_test(int j)
{
	int		i = 0;
	int		k = 0;
	double	t_number = 0;
	double	d_number[number_of_features+2];
	double	tree_cost = 0;
	FILE 	*data_file;
	data_file = fopen (training_file, "rt");
	while (fscanf(data_file,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&d_number[1],&d_number[2],&d_number[3],&d_number[4],&d_number[5],&d_number[6],&d_number[7],&d_number[8]) != EOF)
	{
		for (k=0 ; k<number_of_features ; k++)	used[k]=0;
		i++;
		t_number += fabs(d_number[number_of_features+1] - evaluate_on_data(d_number , population[j].root));
		for (k=1 ; k<=number_of_features ; k++)	
			if(used[k]==1)
				tree_cost += feature_cost[k-1];
	}
	fclose(data_file);
	population[j].cost = tree_cost / i;
	return t_number / i;
}

/*
void prune(struct node *n_node)
{
	struct node *nl_node = NULL;
	struct node *nr_node = NULL;
	struct node *nm_node = NULL;

	if (n_node -> left   != NULL) prune(n_node -> left);
	if (n_node -> right  != NULL) prune(n_node -> right);
	if (n_node -> middle != NULL) prune(n_node -> middle);

	switch (n_node -> key_type)
	{
		case 1:
			switch ((int)n_node -> key_value)
			{
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
			}
		case 4:
			switch ((int)n_node -> key_value)
			{
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
			}
	}
}
*/

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
	strcpy(equation_text,"");
	get_the_equation_text(population[best_index].root);
	printf("Best Solution: %s\n",equation_text);
	printf("The Fitness  : %f\n",best_so_far);

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
			//if ((population[i].fitness >= population[j].fitness) && (population[i].size >= population[j].size) && (population[i].age >= population[j].age) && (population[i].cost >= population[j].cost))
			//if((population[i].fitness >= population[j].fitness) && (population[i].cost >= population[j].cost) && (population[i].size*population[i].age >= population[j].size*population[j].age))
			if((population[i].fitness >= population[j].fitness) && (population[i].cost*population[i].size*population[i].age >= population[j].cost*population[j].size*population[j].age))
			{
				strcpy(equation_text,"");
				get_the_equation_text(population[i].root);
				strcpy(text1,equation_text);
				strcpy(equation_text,"");
				get_the_equation_text(population[j].root);
				
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
	printf("Nondominate count %d\n",number_of_nondominated);
	fprintf(f_results,"%d\n",number_of_nondominated);
	fclose(f_results);
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

void final_pareto_front()
{
	int i = 0;
	int j = 0;
	int k = 0;
	char text1[500];
	number_of_nondominated = population_size;
	for (i=0 ; i<population_size ; i++)	dominated[i] = False;
	for (i=0 ; i<population_size ; i++)
	{
		for (j=0 ; j<population_size ; j++)
		{
			if ((i != j)&&(dominated[i]==False)&&(dominated[j]==False)&&(population[i].fitness >= population[j].fitness) && (population[i].cost >= population[j].cost))
			{
				dominated[i] = True;
				number_of_nondominated --;
				break;
			}
		}
	}
	printf("number is %d\n",number_of_nondominated);
	k=0;
	for(j=0 ; j<population_size ; j++)
	{
		if(dominated[j]==False)
		{
			pop[k].root = population[j].root;
			pop[k].fitness = population[j].fitness;
			pop[k].cost = population[j].cost;
			strcpy(equation_text,"");
			get_the_equation_text(pop[k].root);
			printf("%f ---- %s\n",pop[k].fitness,equation_text);
			k++;
		}
		else
			destroy_tree(population[j].root);
	}
	pop_count = k;
}


int find_final_solution()
{
	int 	i = 0;
	int 	best_i = 0;
	double 	best_d = 10000000;
	
	for(i=0 ; i<population_size ; i++)
	{
		if((dominated[i] == False) && ((population[i].fitness+population[i].cost)<best_d))
		{
			best_d = population[i].fitness+population[i].cost;
			best_i = i;
		}
	}
	return best_i;
}

void Simulation_phase()
{
	int		i = 0;
	int		k = 0;
	int		l = 0;
	int		j = 0;
	int		s_count[number_of_features+2];
	double	t_number = 0;
	double	d_number[number_of_features+2];
	double	tree_cost = 0;
	double	partial_cost = 0;
	double	partial_number = 0;
	FILE	*data_file;
	FILE	*cf;
	FILE	*af;
	FILE	*uf;
	cf = fopen(cost_file,"a");
	af = fopen(accuracy_file,"a");
	uf = fopen(usage_file,"a");
	data_file = fopen(testing_file,"rt");

	for (j=0 ; j<number_of_features+2 ; j++) s_count[j] = 0;
	while (fscanf(data_file,"%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",&d_number[1],&d_number[2],&d_number[3],&d_number[4],&d_number[5],&d_number[6],&d_number[7],&d_number[8]) != EOF)
	{
		partial_cost = 0;
		for (k=0 ; k<number_of_features ; k++)	used[k]=0;
		i++;
		printf("test on %ith point\n",i);
		partial_number = 0;
		for (l=0 ; l<pop_count ; l++)	partial_number += fabs(d_number[number_of_features+1] - evaluate_on_data(d_number , pop[l].root));
		partial_number /= pop_count;
		for (k=1 ; k<=number_of_features ; k++)	
			if(used[k]==1)
			{
				partial_cost += feature_cost[k-1];
				s_count[k] ++;
			}		
		tree_cost +=partial_cost;
		t_number +=partial_number;
	}
	fprintf(af,"%f\n",t_number / i);
	fprintf(cf,"%f\n",tree_cost / i);
	for (j=1 ; j<number_of_features ; j++) fprintf(uf,"%f,",(double)s_count[j] / i);
	fprintf(uf,"%f\n",(double)s_count[number_of_features] / i);
	fclose(cf);
	fclose(af);
	fclose(uf);
}


void main()
{
	int i;
	int generation = 1;
	double best_fitness = 100000;
	srand( time(NULL));
	FILE *f_results;
	FILE *f_fit;
	FILE *f_cost;
	initialize_population();
	while ((generation < max_evolution_round) && (number_of_nondominated <= max_nondominated)) //&& (best_fitness > fitness_stall)
	{
		printf("========== ROUND %d ==========\n",generation);
		best_fitness = generation_individual_update();
		printf(">> UPDATED\n");
		find_pareto_front();
		printf(">> FOUND\n");
		fill_dominated_trees();
		printf(">> FILLED\n");
		generation ++;
	}
	printf("========== ENDED IN %dth GENERATION ==========\n",generation);
	best_fitness = generation_individual_update();
	final_pareto_front();
	Simulation_phase();
}










