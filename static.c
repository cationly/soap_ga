#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ga.h"
#include "element.h"
#include <mpi.h>


#include "soap_c_wrap.h"


void element_setup(Element *element, int randomise) {
	
	
	element->dnasize = 5;
	element->dna = (double*)calloc(element->dnasize, sizeof(double));
	
	if(randomise == 1) {
		
		for(int i=0; i<element->dnasize; i++)
			element->dna[i] = 2*((double)rand()/RAND_MAX)-1;
		
	}
	
}
void element_load(Element *element) {
	
	
}
void element_clean(Element *element) {
	
}
void element_evaluate(GA *engine, Element *element) {
	
	
	double *data = (double*)engine->data;
	element->fitness = 0;
	
	for(int i=0; i<100; i++) {
		double result = 0;
		double x = 3*(2*(double)i/100.0 - 1);
		double xx = 1;
		for(int j=0; j<element->dnasize; j++) {
			result += element->dna[j]*xx;
			xx *= x;
		}
		result = result - data[i];
		//result *= result;
		result = fabs(result);
		element->fitness -= result;
	}
	/*printf("Element[%i]: fitness %lf", element->ID, element->fitness);
	for(int j=0; j<element->dnasize; j++)
		printf("\t%lf", element->dna[j]);
	printf("\n");*/
}




int main(int argc, char **argv) {





	char filename[] = "dsgdb7ae2.xyz";
	int molecules_no = 7102;
	int train_no = 7;
	int validate_no = 3;
	void *dset = setup_soap(filename,molecules_no,train_no,validate_no);

	double *params = (double *) malloc(7*sizeof(double));
	double diag[] = {1,1,1,1,1};    //H,C,N,O,S
	int i;
	for (i=0; i<5; i++) params[i] = diag[i];
	params[5] = pow(10,-3); //lambda
	params[6] = 1;          //zeta
	double mae = run_soap(dset,params);
	free(params);
	printf("MAE: %f\n", mae);
	free_dset(dset);



	
	int mpierr = MPI_Init(&argc, &argv);

	int mpiID, mpiNP;
	mpierr = MPI_Comm_rank(MPI_COMM_WORLD, &mpiID);
	mpierr = MPI_Comm_size(MPI_COMM_WORLD, &mpiNP);
	
	GA *engine = ga_new_mpi();
	engine->element_setup = element_setup;
	engine->element_load = element_load;
	//engine->element_setup = element_setup;
	engine->element_evaluation = element_evaluate;
	
	/*engine->populationSize = 1024;
	engine->immigrationSize = 10;
	engine->mutationRate = 0.1;
	engine->mutationAmount=0.5;*/
	engine->output.binary = 0;
	engine->output.bestonly = 1;
	//engine->keepbest = 1;
	
	
	ga_readsettings(engine, "ga.in");
	
	//mpierr = MPI_Finalize();
	//return 0;
	
	
	
	double *data = (double*)malloc(sizeof(double)*100);
	for(int i=0; i<100; i++) {
		double x = 3*(2*(double)i/100.0 - 1);
		data[i] = 0.3 + 0.5*x - 0.4*x*x - 0.8*x*x*x;
	}
	engine->data = data;
	
	ga_settings_mpi(engine);
	ga_setoutput_mpi(engine, "st.out");
	
	
	ga_init_mpi(engine);
	
	
	ga_evaluate_mpi(engine);
	for(int gen=0; gen<2; gen++) {
		ga_step_mpi(engine);
		ga_evaluate_mpi(engine);
	}
	
	ga_clean(engine);
	mpierr = MPI_Finalize();
	return 0;
}

