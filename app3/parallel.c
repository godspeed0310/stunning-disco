#include<stdio.h>
#include<omp.h>

int main(int argc, char* argv[])
{
	if(argc < 3){
		printf( "Usage: %s n p \n", argv[0] );
		return -1;
	}

	int N=atoi(argv[1]); // problem size
	int P=atoi(argv[2]); // processors

	int i, j, k;

	// allocate memory for algorithm arrays
	int (*adj_mat)[N] = malloc(N * sizeof(*adj_mat));
	int (*dis)[N] = malloc(N * sizeof(*dis));

	// allocate memory for the auxiliary arrays
	int * x = (int *)malloc(sizeof(int) * N);
	int * xp = (int *)malloc(sizeof(int) * N);
	int * y = (int *)malloc(sizeof(int) * N);
	int * yp = (int *)malloc(sizeof(int) * N);

	for(i=0; i<N; i++) {
		for(j=i+1; j<N; j++) {
			adj_mat[i][j] = adj_mat[j][i];
		}
	}
	for(i=0; i<N; i++) {
		for(j=0; j<N; j++) {
			if(i == j) {
				dis[i][j] = 0;
			} else {
				dis[i][j] = adj_mat[i][j];
			}

			// force the load of the auxiliary
			// arrays into the cache
			if(i == 0) {
				x[j] = dis[i][j];
				xp[j] = dis[i][j];
			}
			if(j == 0) {
				y[i] = dis[i][j];
				yp[i] = dis[i][j];
			}
		}
	}

	// set the number of threads
	omp_set_num_threads(P);

	// note the indexing of k
	// done to avoid comparison with k+1
	for(k=1; k<=N; k++) {
		#pragma omp parallel for private(i,j)
		for(i=0; i<N; i++) {
			// this is the kth row
			// future auxiliary array
			if(i == k) {
				for(j=0; j<N; j++) {
					if(dis[i][j] > x[j]+y[i]){
						dis[i][j] = x[j]+y[i];
					}
					xp[j] = dis[i][j];
				}
				// we missed this in the for loop
				// but no worries, since it is in cache
				yp[k] = xp[k];
			} else {
				for(j=0; j<N; j++) {
					if(dis[i][j] > x[j]+y[i]) {
						dis[i][j] = x[j]+y[i];
					}

					// kth column
					if(j == k)
						yp[i] = dis[i][j];
				}
			}
		}

		// update the auxiliary arrays for the next iteration
		#pragma omp parallel for private(i)
		for(i=0; i<N; i++) {
			x[i] = xp[i];
			y[i] = yp[i];
		}
	}
	
	return 0;
}
