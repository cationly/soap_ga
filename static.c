#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ga.h"
#include "element.h"
#include <mpi.h>
#include "soap_c_wrap.h"

void *dataset;

void element_setup(Element *element, int randomise) {
	
	
	element->dnasize = 7;
	element->dna = (double*)calloc(element->dnasize, sizeof(double));
	
	if(randomise == 1) {

		// similarity kernel diagonal for H,C,N,O,S
		for(int i=0; i<5; i++)
			element->dna[i] = 99.f*((double)rand()/RAND_MAX)+1.f;	//range: 1 : 100
		// lambda
		element->dna[5] = pow(10.f,-20.f*((double)rand()/RAND_MAX));	//range: 10^-20 : 10^0
		// zeta
		element->dna[6] = 9.9*((double)rand()/RAND_MAX)+0.1;		//range: 0.1 : 10
		
	}
	
}
void element_load(Element *element) {
	
	
}
void element_mutate(Element *element, int amount) {
	
	double r;
	
	for(int i=0; i<amount; i++) {
		int idx = rand() % element->dnasize;
		r = (double)rand()/RAND_MAX;
		if (idx < 5) {		/* similarity kernel diagonal */
			element->dna[idx] = 99*r+1;		//range: 1 : 100
		} else if (idx == 5) {	/* lambda */	
			element->dna[idx] = pow(10,-20*r);	//range: 10^-20 : 10^0
		} else if (idx == 6) {	/* zeta */	
			element->dna[idx] = 9.9*r+0.1;		//range: 0.1 : 10
		}
	}
	
}
void element_clean(Element *element) {
	
}
void element_evaluate(GA *engine, Element *element) {

	double *params = element->dna;
	double mae = run_soap(dataset,params);
	element->fitness = -mae;

}
int main(int argc, char **argv) {

	char filename[] = "dsgdb7ae2.xyz";
	int molecules_no = 7102;
	int train_no = 40;	//at least 43 to cover all small molecules (below 5 non-h atoms)
	int validate_no = 10;
	dataset = setup_soap(filename,molecules_no,train_no,validate_no);
	
	int mpierr = MPI_Init(&argc, &argv);

	int mpiID, mpiNP;
	mpierr = MPI_Comm_rank(MPI_COMM_WORLD, &mpiID);
	mpierr = MPI_Comm_size(MPI_COMM_WORLD, &mpiNP);
	
	GA *engine = ga_new_mpi();
	engine->element_setup = element_setup;
	engine->element_load = element_load;
	engine->element_mutate = element_mutate;
	engine->element_clean = element_clean;
	engine->element_evaluation = element_evaluate;
	
	engine->output.binary = 0;
	engine->output.bestonly = 1;	
	
	ga_readsettings(engine, "ga.in");

	
	ga_settings_mpi(engine);
	ga_setoutput_mpi(engine, "st.out");
	
	
	ga_init_mpi(engine);
	
	
	ga_evaluate_mpi(engine);
	for(int gen=0; gen<10; gen++) {
		ga_step_mpi(engine);
		ga_evaluate_mpi(engine);
	}

	
	ga_clean(engine);
	mpierr = MPI_Finalize();
	free_dset(dataset);
	return 0;
}

