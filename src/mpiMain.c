
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <mpi.h>

#include "myK.h"
#include "json.h"
#include "graph4.h"


#define ROOT 0
#define KMIN 1
#define KMAX 25

Game* pop;
int nbElem;

// Function for JSON Parsing
static void process_value(json_value* value, int depth);

// Function for JSON Parsing
static void process_object(json_value* value, int depth){
	int length, x;
	char*  key;
	Game tmp;
	int verify=1;
	if (value == NULL) {
		return;
	}
	length = value->u.object.length;
	// Get every Data from JSON to build struct
	for (x = 0; x < length; x++) {
		key=value->u.object.values[x].name;
		if (strcmp(key, "appid")==0){
			// Break if data empty (Specify by SteamSpy appid=999999 nodata)
			if(value->u.object.values[x].value->u.integer == 999999){
				break;
			}else{
				tmp.appid = value->u.object.values[x].value->u.integer;
			}
		}else if (strcmp(key,"owners")==0){
			tmp.owners=value->u.object.values[x].value->u.integer;
		}else if (strcmp(key,"players_2weeks")==0){
			tmp.p2weeks=value->u.object.values[x].value->u.integer;
		}else if (strcmp(key,"name")==0){
			strcpy(tmp.name, value->u.object.values[x].value->u.string.ptr);
		}
		process_value(value->u.object.values[x].value, depth+1);
		verify=0;
	}

	//Test to elimate bug value that appear at the end
	if(tmp.appid==4207711){
		printf("object[%d].name = %s\n", x, value->u.object.values[x].name);
		exit(1);
	}
	if(verify ==0){
		pop[nbElem]=tmp;
		nbElem++;
	}
}

// Function for JSON Parsing
static void process_array(json_value* value, int depth){
	int length, x;
	if (value == NULL) {
		return;
	}
	length = value->u.array.length;
	printf("array\n");
	
	for (x = 0; x < length; x++) {
		process_value(value->u.array.values[x], depth);
	}
}

// Function for JSON Parsing
static void process_value(json_value* value, int depth){
	int j;
	if (value == NULL) {
		return;
	}
	switch (value->type) {
		case json_none:
			printf("none\n");
			break;
		case json_object:
			process_object(value, depth+1);
			break;
		case json_array:
			process_array(value, depth+1);
			break;
	}
}



int main(int argc, char** argv){
	char* filename;
	char* dirname;
	char* file_contents;
	FILE *fp;
	struct stat filestatus, dirstatus;
	int file_size;
	int i,j,ierr,my_id;
	int nbCluster;
	json_char* json;
	json_value* value;
	MPI_Status status;

	/**************  Mpi Start *****************/
	ierr= MPI_Init(&argc,&argv);
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	
	// Create a type for struct 
	const int nitems=4;
	int blocklengths[4] = {1,1,1,200};
	MPI_Datatype types[4] = {MPI_INT,MPI_INT, MPI_INT,MPI_CHAR};
	MPI_Datatype mpi_game_type;
	MPI_Aint     offsets[4];

	offsets[0] = offsetof(Game, appid);
	offsets[1] = offsetof(Game, owners);
	offsets[2] = offsetof(Game, p2weeks);
	offsets[3] = offsetof(Game, name);

	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_game_type);
	MPI_Type_commit(&mpi_game_type);
	
	// Init Game Array
	nbElem=0;
	pop = malloc(10000*sizeof(Game));
	if(pop == NULL){
		fprintf(stderr, "Unable to alloc memory\n");
		exit(1);
	}


	// Separating Root from other
	if(my_id==ROOT){
	
		// Reading entry from args
		if (argc != 2) {
			fprintf(stderr, "%s <file_json>\n", argv[0]);
			return 1;
		}
		filename=argv[1];
		// Verify file
		if ( stat(filename, &filestatus) != 0) {
			fprintf(stderr, "File %s not found\n", filename);
			return 1;
		}
		
		// Open file
		fp = fopen(filename, "rt");
		if (fp == NULL) {
			fprintf(stderr, "Unable to open %s\n", filename);
			fclose(fp);
			return 1;
		}
		file_size = filestatus.st_size;
		file_contents = (char*)malloc(filestatus.st_size);
		if ( file_contents == NULL) {
			fprintf(stderr, "Memory error: unable to allocate %d bytes\n", file_size);
			return 1;
		}
		// Read File
		if ( fread(file_contents, file_size, 1, fp) != 1 ) {
			fprintf(stderr, "Unable t read content of %s\n", filename);
			fclose(fp);
			free(file_contents);
			return 1;
		}

		fclose(fp);
		printf("--------------------------------\n\n");
	

		// JSON reading Part
		json = (json_char*)file_contents;
		value = json_parse(json,file_size);
		if (value == NULL) {
			fprintf(stderr, "Unable to parse data\n");
			free(file_contents);
			exit(1);
		}

		process_value(value, 0);
		json_value_free(value);
		free(file_contents);
		
		//Suppression bug Elem
		nbElem=nbElem-1;	
		printf(" Nbelement : %d \n",nbElem);
		// Send Data
		// Send Array Size
		MPI_Bcast(&nbElem, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
		printf(" Array Size : %d\n",nbElem);
		// Send Array
		MPI_Bcast(pop, nbElem, mpi_game_type, ROOT, MPI_COMM_WORLD);
		// Send number of cluster
		nbCluster=12;
		MPI_Bcast(&nbCluster, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
		nbCluster=4;

	}else{
		// Wait for receive Data
		// Receive Array Size
		MPI_Bcast(&nbElem, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
		printf(" Array Size : %d\n",nbElem);
		// Receive Array
		MPI_Bcast(pop,nbElem,mpi_game_type,ROOT,MPI_COMM_WORLD);
		// Receive number of cluster
		MPI_Bcast(&nbCluster,1,MPI_INT,ROOT,MPI_COMM_WORLD);
	}
	for (nbCluster=my_id*4; nbCluster<(my_id*4)+4; nbCluster++){
		int* c;
		Game** clusterOrder;
		double ** centroids;
		// Si nbCluster < 4 bug pour le k-means
		if(nbCluster<4){
			nbCluster=4;
		}
		printf("Cluster %d\n",nbCluster);

		/* K-mean Part */
		printf("Start Rank %d\n",my_id);
		centroids= malloc(nbCluster* sizeof(double*));
		if(centroids == NULL){
			fprintf(stderr, "Unable to alloc memory\n");
			return 1;
		}
		// Allocation centroids
		for ( i = 0; i < nbCluster; i++) {
			centroids[i] = (double*)malloc( 2*sizeof(double));
			if(centroids[i] == NULL){
				fprintf(stderr, "Unable to alloc memory\n");
				return 1;
			}
			centroids[i][0]=0.0;
			centroids[i][1]=0.0;
		}
		
		c = k_means(pop, nbElem, 2, nbCluster, 1e-4, centroids);
		/* K-mean End */
		printf("Rank %d\n",my_id);
		printf("Cluster %d\n",nbCluster);

		/* Ordering Data Part */
		int sizeCluster[nbCluster];
		clusterOrder = malloc(nbCluster*sizeof(Game*));
		if(clusterOrder == NULL){
			fprintf(stderr, "Unable to alloc memory\n");
			return 1;
		}
		printf("Reading Past\n");
		// Allocate Memory
		for( i=0; i< nbCluster; i++){
			sizeCluster[i]=0;
			clusterOrder[i] = malloc(7000*sizeof(Game));
			if(clusterOrder[i] == NULL){
				fprintf(stderr, "Unable to alloc memory\n");
				return 1;
			}
		}
		printf("Malloc Past\n");

		// Ordering Data
		for (i = 0; i < nbElem; i++) {
			clusterOrder[c[i]][sizeCluster[c[i]]]=pop[i];
			sizeCluster[c[i]]++;
		}
		printf("Ordering Past\n");
	
		// Create Directory for Cluster information
		printf("Directory Start\n");
		dirname = malloc(255*sizeof(char));
		sprintf(dirname,"./result/ClusterResult%d",nbCluster);
		if (stat(dirname,&dirstatus)==-1){
			mkdir(dirname,0700);
		}
		printf("Directory Past\n");
		/* Ordering Data End */
		
		/* Writting Part */
		// File info cluster
		printf("File Start\n");
		filename = malloc(255*sizeof(char));
		for( i=0; i<nbCluster; i++){
			sprintf(filename,"%s/Cluster%d.tmp",dirname,i);
			fp = fopen(filename, "w+");
			if (fp == NULL) {
				fprintf(stderr, "Unable to open %s\n", filename);
				fclose(fp);
				return 1;
			}
			printf("Cluster : %d\n",i);
			for (j=0;j<sizeCluster[i];j++){
				fprintf(fp,"%d %d \n",clusterOrder[i][j].owners,clusterOrder[i][j].p2weeks);
			}
			fclose(fp);
		}
		printf("Centroid Start\n");

		// Write centroid point in file
		sprintf(filename,"%s/Centroid.tmp",dirname);
		fp = fopen(filename, "w+");
		if (fp == NULL) {
			fprintf(stderr, "Unable to open %s\n", filename);
			fclose(fp);
			return 1;
		}
		for( i=0; i<nbCluster; i++){
			fprintf(fp,"%e %e \n",centroids[i][0],centroids[i][1]);
		}
		fclose(fp);
		/* Writting End */

		printf("File Past\n");
	
		/* Draw Part */
		printf("Graph Start\n");
		getCoord(my_id,nbCluster,nbElem);
		/* Draw End */

		// Writting File Verif
		sprintf(filename,"./result/FileResult%d.tmp",nbCluster);
		fp = fopen(filename, "w+");
		if (fp == NULL) {
			fprintf(stderr, "Unable to open %s\n", filename);
			fclose(fp);
			return 1;
		}
		// Create a file for controle data
		for (i = 0; i < nbElem; i++) {
			fprintf(fp,"Data point %s is in cluster %d. Coordonate %d and %d\n", pop[i].name, c[i],pop[i].owners,pop[i].p2weeks);
		}

		/* HouseKeeping Part */
		fclose(fp);
		free(c);
		free(clusterOrder);
		free(centroids);
	}
	free(filename);
	free(pop);
	ierr = MPI_Finalize();
	/* HouseKeeping Part End */
	/**************  Mpi End  *******************/

	return 0;
}

