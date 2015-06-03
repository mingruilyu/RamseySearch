#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


#define MAXSIZE (541)
void PrintGraphNew(int *g, int gsize)
{
	int i, j;
	char file_name[100];
	sprintf(file_name, "../../file/client/new_graph");
	FILE* fd = fopen(file_name, "w");
	fprintf(fd,"%d-------------------\n",gsize);
	fprintf(stdout,"%d-------------------\n",gsize);
	for(i=0; i < gsize; i++)
	{
		for(j=0; j < gsize; j++)
		{
			fprintf(fd,"%d ",g[i*gsize+j]);
			//fprintf(stdout,"%d ",g[i*gsize+j]);
		}
		fprintf(fd,"\n");
		//fprintf(stdout,"\n");
	}
	fclose(fd);
	return;
}
/*
 * prints in the right format for the read routine
 */
void PrintGraph(int *g, int gsize)
{
	int i;
	int j;
	char file_name[100];
	sprintf(file_name, "../../file/client/cur_graph");
	FILE* fd = fopen(file_name, "w");
	fprintf(fd,"%d-------------------\n",gsize);
	fprintf(stdout,"%d-------------------\n",gsize);
	for(i=0; i < gsize; i++)
	{
		for(j=0; j < gsize; j++)
		{
			fprintf(fd,"%d ",g[i*gsize+j]);
			//fprintf(stdout,"%d ",g[i*gsize+j]);
		}
		fprintf(fd,"\n");
		//fprintf(stdout,"\n");
	}
	fclose(fd);
	return;
}

/*
 * reads a graph of the correct format from the 
 * file referenced by #fname#.  The resulting 
 * is passed back through the #g# and #gsize#
 * out parameters.  The space for the file is 
 * malloced and the routine is not thread safe.  
 * Returns 1 on success and 0 on failure.
 */
int ReadGraph(char *fname, int **g, int *gsize)
{
    int i, j, lsize, lcount;
	FILE *fd;
	int *lg;
	char line_buff[255];
	char *curr, *err, *tempc;

	fd = fopen(fname, "r");
	if(fd == 0) {
		fprintf(stderr,"ReadGraph cannot open file %s\n",fname);
		fflush(stderr);
		return(0);
	}

	fgets(line_buff, 254, fd);
	if(feof(fd)) {
		fprintf(stderr,"ReadGraph eof on size\n");
		fflush(stderr);
		fclose(fd);
		return(0);
	}	
	i = 0;
	while((i < 254) && !isdigit(line_buff[i]))
		i++;
	
	/*
	 * first line of the file must contain a size
 	 */
	if(!isdigit(line_buff[i])) {
		fprintf(stderr,
						"ReadGraph format error on size\n");
		fflush(stderr);
		fclose(fd);
		return(0);
	}

	tempc = line_buff;
	lsize = (int)strtol(tempc,&tempc,10);
	if((lsize < 0) || (lsize > MAXSIZE)) {
		fprintf(stderr,
						"ReadGraph size bad, read: %d, max: %d\n",
				lsize, MAXSIZE);
		fflush(stderr);
		fclose(fd);
		return(0);
	}

	lg = (int *)malloc(lsize * lsize * sizeof(int));
	if(lg == NULL)
	{
		fprintf(stderr,"ReadGraph: no space\n");
		fflush(stderr);
		return(0);
	}

	memset(lg, 0, lsize * lsize * sizeof(int));
  for(i=0; i < lsize; i++) {
		if(feof(fd)) break;
		err = fgets(line_buff,254,fd);
		if(err == NULL) break;
		curr = line_buff;
    for(j=0; j < lsize; j++) {
			sscanf(curr, "%d ", &(lg[i * lsize + j]));
			if((lg[i * lsize + j] != 1) 
					&& (lg[i * lsize + j] != 0)) {
				fprintf(stderr,
								"ReadGraph: non-boolean value read: %d\n", 
								lg[i * lsize + j]);
				fflush(stderr);
				fclose(fd);
				return(0);
			}
			while(isdigit(*curr)) curr++;
			while(isspace(*curr)) curr++;
		}
	}

	if(i < lsize) {
		fprintf(stderr,
						"ReadGraph file too short, lsize: %d\n",
						lsize);
		fflush(stderr);
		fclose(fd);
		return(0);
	}

	fclose(fd);

	*g = lg;
	*gsize = lsize;
	return(1);
}

void* DegradeGraph(int *old_g, int o_gsize) {
	int i, j;
	int* new_g = (int *)malloc((o_gsize - 1) * (o_gsize - 1) *sizeof(int));
	for(i = 0; i < o_gsize - 1; i ++) {
		for(j = 0; j < o_gsize - 1; j ++) {
			new_g[i * (o_gsize - 1) + j] = old_g[i * o_gsize + j];
		}
	}
	free(old_g);
	return new_g;
}

/*
 * CopyGraph 
 *
 * copys the contents of old_g to corresponding locations in new_g
 * leaving other locations in new_g alone
 * that is
 * 	new_g[i,j] = old_g[i,j]
 */
void CopyGraph(int *old_g, int o_gsize, int *new_g, int n_gsize)
{
	int i;
	int j;

	/*
	 * new g must be bigger
	 */
	if(n_gsize < o_gsize)
		return;

	for(i=0; i < o_gsize; i++)
	{
		for(j=0; j < o_gsize; j++)
		{
			new_g[i*n_gsize+j] = old_g[i*o_gsize+j];
		}
	}

	return;
}

