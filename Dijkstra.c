/* Dijkstra */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "time_util.h"

#ifdef DEBUG
# define dbg_printf(...) printf(__VA_ARGS__)
#else
# define dbg_printf(...)
#endif

#define INFINITY	200000
#define BUF_SIZE	4096
#define MIN(x, y) ((x)<(y)?(x):(y))

int Dijkstra(int ** costs, int num, int src, int dest) {
	int i, j;
	int next = src, next_tmp;
	int min_cost;
	int * flag = malloc(num * sizeof(int));
	int * result = malloc(num * sizeof(int));
	int * route = malloc(num * sizeof(int));
	memset(flag, 0, num * sizeof(int));
	/* init */
	result[src] = 0;
	flag[src] = 1;
	min_cost = INFINITY;
	route[src] = -1;
	for(i = 0; i < num; i++) {
	    if(i == src)
	    	continue;
	    result[i] = costs[src][i];
	    dbg_printf("%d:%d ", i, result[i]);
	    route[i] = src;
	    if(result[i] < min_cost) {
	        min_cost = result[i];
	        next_tmp = i;
	    }
	}
	next = next_tmp;
	dbg_printf("\n");
	for(i = 1; i < num; i++) {
		dbg_printf("next node: %d\n", next);
		if(next == dest)
			break;
		min_cost = INFINITY;
		flag[next] = 1;
		dbg_printf("%d ", result[next]);
		for(j = 0; j < num; j++) {
			if(!flag[j]) {
				if(result[next] + costs[next][j] < result[j]) {
					route[j] = next;    
					result[j] = result[next] + costs[next][j];
				}
			    if(result[j] < min_cost) {
			        min_cost = result[j];
					next_tmp = j;
			    }
			}
			dbg_printf("%d:%d,%d,%d ", j, result[j], result[next], costs[next][j]);
		}
		next = next_tmp;
		dbg_printf("\n");
	}
	if(min_cost != INFINITY) {
		dbg_printf("%d", dest);
		while(route[dest] != -1) {
			dbg_printf("<-%d", route[dest]);
			dest = route[dest];
		}
		dbg_printf("\n");
	}
	free(route);
	free(flag);
	free(result);
	return min_cost;
}

int main(int argc, char ** argv) {
	char * filename = "tiny.txt";
	int src = 4;
	int dest = 6;

	if(argc >= 4) {
		filename = argv[1];
		src = atoi(argv[2]);
		dest = atoi(argv[3]);
	}

	FILE * testfile = fopen(filename, "r");
	char * line = malloc(BUF_SIZE);
	int i, j;
	int ** costs;
	size_t n = BUF_SIZE;
	int node;
	int edge;
	int start, end;
	int cost;
	unsigned long start_time, end_time;
	if(getline(&line, &n, testfile) > 0) 
		sscanf(line, "%d", &node);
	if(getline(&line, &n, testfile) > 0) 
		sscanf(line, "%d", &edge);
	costs = malloc(node * sizeof(int *));
	dbg_printf("node size: %d, edge size: %d\n", node, edge);
	/* init */
	for(i = 0; i < node; i++) {
		costs[i] = malloc(node * sizeof(int));
		for(j = 0; j < node; j++)
			costs[i][j] = INFINITY;
	}
	/* set costs */
	for(i = 0; i < edge; i++) {
		if(getline(&line, &n, testfile) > 0) {
			if(sscanf(line, "%d %d %d", &start, &end, &cost) != 3)
				exit(1);
			costs[start][end] = cost;
		}
	}
	/* print costs */
	for(i = 0; i < node; i++) {
		for(j = 0; j < node; j++)
			dbg_printf("%d\t", costs[i][j]);
		dbg_printf("\n");
	}
	start_time = milli_time();
	cost = Dijkstra(costs, node, src, dest);
	end_time = milli_time();
	printf("\nsrc %d, dest %d, cost %d, computing time: %lu us\n",
		src, dest, cost, end_time - start_time);
	return 0;
}
