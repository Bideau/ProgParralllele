#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include "gnuplot_i.h"
#define SLEEP_LGTH 10

// Draw all the data
int getCoord(int id,int nbCluster,int nbElem){
	//File info cluster
	char* filename;
	char* dirname;
	char* file_contents;
	char* line;
	char* token;
	struct stat filestatus,dirstatus;
	FILE* fp;
	int i,j,k,file_size;
	size_t len;
	ssize_t read=0;
	double* yvals;
	double* xvals;

	//Plot Stat
	gnuplot_ctrl* myPlot;
	myPlot = gnuplot_init();

	// Select Directory
	dirname = malloc(255*sizeof(char));
	sprintf(dirname,"./result/ClusterResult%d",nbCluster);
	if (stat(dirname,&dirstatus)==-1){
		mkdir(dirname,0700);
	}
	filename = malloc(355*sizeof(char));
	// Plot Centroid
	sprintf(filename,"%s/Centroid.tmp",dirname);
	//gnuplot_cmd(myPlot,"set style tc rgb 'black'");
	gnuplot_setstyle(myPlot, "dots") ;
	toPlotFile(filename,myPlot);
	gnuplot_cmd(myPlot,"unset style");

	gnuplot_setstyle(myPlot, "points") ;
	// Select File
	// Plot Cluster
	for( i=0; i<nbCluster; i++){

		sprintf(filename,"%s/Cluster%d.tmp",dirname,i);
		printf("Dir %d : %s\n",i,filename);
		
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
		line = malloc(255*sizeof(char));
		// Value init
		xvals = malloc(nbElem*sizeof(double));
		if(xvals == NULL){
			fprintf(stderr, "Unable to alloc memory\n");
			return 1;
		}
		yvals = malloc(nbElem*sizeof(double));
		if(yvals == NULL){
			fprintf(stderr, "Unable to alloc memory\n");
			return 1;
		}
		
		// Draw all the Data from one thread
		toPlotFile(filename,myPlot);
		fclose(fp);
	}

	printf("File Past\n");
	sleep(SLEEP_LGTH) ;
	gnuplot_close(myPlot);
	return 0;
	
}

// Plot from an array of double
int toPlot(double* xvals,double* yvals,int numPoint){
	char * commandsForGnuplot[] = {"set title \"Steam Data Traitement\"","set xlabel 'owners'","set ylabel 'p2weeks'","set logscale xy", "plot 'data.temp'"};
	
	/*Opens an interface that one can use to send commands as if they were typing into the
	*     gnuplot command line.  "The -persistent" keeps the plot open even after your
	*     C program terminates.
	*/
	gnuplot_ctrl* myPlot;
	myPlot = gnuplot_init();
	gnuplot_cmd(myPlot,"set logscale xy");
	gnuplot_setstyle(myPlot, "points") ;
	gnuplot_plot_xy(myPlot, xvals, yvals, numPoint, "Steam Data") ;

	sleep(SLEEP_LGTH) ;
	gnuplot_close(myPlot);
	return 0;
}

// Plot form a file
int toPlotFile(char* filename,gnuplot_ctrl* myPlot){
	
	gnuplot_cmd(myPlot,"set logscale xy");
	gnuplot_setstyle(myPlot, "points");
	gnuplot_plot_atmpfile(myPlot,filename,"Toto");

	return 0;
}
