#include<stdio.h>
#include<omp.h>

int main(int argc, char* argv[])
{
	if(argc < 3){
		printf( "Usage: %s n p \n", argv[0] );
		return -1;
	}

	int N = atoi(argv[1]);	// problem size
	int P = atoi(argv[2]);	// processors

	int i, j, k;

	// allocate memory for the required arrays
	int (*adj_mat)[N] = malloc(N * sizeof(*adj_mat));
	int (*dis)[N] = malloc(N * sizeof(*dis));

	// allocate memory for the auxiliary arrays
	int * x = (int *)malloc(sizeof(int) * N);
	int * y = (int *)malloc(sizeof(int) * N);

	// make the graph valid, edge i->j = edge j->i
	for(i=0; i<N; i++) {
		for(j=i+1; j<N; j++){
			adj_mat[i][j] = adj_mat[j][i];
		}
	}

	// initialise the dis matrix, no intermediate nodes
	// edges themselves are the optimum path
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			if(i == j){
				dis[i][j] = 0;
			} else {
				dis[i][j] = adj_mat[i][j];
			}
		}
	}

	// core algorithm
	for(k=0; k<N; k++) {

		// load the auxiliary arrays
		for(i=0; i<N; i++) {
			x[i] = dis[k][i];
			y[i] = dis[i][k];
		}

		for(i=0; i<N; i++) {
			// as discussed in the report
			// this row will not be updated so we can skip it
			if(i == k) continue;

			for(j=0; j<N; j++) {
				if(dis[i][j] > x[j] + y[i]) {
					dis[i][j] = x[j] + y[i];
				}
			}
		}	
	}

	return 0;
}
