/* Dijkstra by adjacent list */
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

#define BUF_SIZE	4096
#define INFINITY    200000
#define SWAP(x, y, tmp)            	\
{                        			\
    tmp = x;                		\
    x = y;                			\
    y = tmp;                		\
}

struct CostNode {
    int cost;
    int node;
};

struct PriorityQueue {
    int size;
    int capacity;
    struct CostNode * front;
    int * map;
};

int enqueue(struct PriorityQueue * queue, int node, int cost) {
    struct CostNode tmp;
    int size = queue -> size;
    struct CostNode * front = queue -> front;
    int index;
    int need_swap = 1;
	dbg_printf("enqueue node %d, cost %d, map: %d\n", node, cost, queue -> map[node]);
	if(queue -> map[node] != -1) {
		index = queue -> map[node];
		if(front[index].cost <= cost) {
			return 0;
		}
		else {
			front[index].cost = cost;
		}
	}
    
    front[size].cost = cost;
    front[size].node = node;
    queue -> map[node] = size;   
    index = size;
    queue -> size = size + 1;
	while(need_swap && index > 0) {
		need_swap = 0;
		if(front[index].cost < front[(index - 1)/2].cost) {
		    SWAP(front[index], front[(index - 1)/2], tmp);
		    /* update map */
		    queue -> map[front[index].node] = index;
		    queue -> map[front[(index - 1)/2].node] = (index - 1)/2;
		    /* move to next */
		    index = (index - 1) / 2;
		    need_swap = 1;
		}
	}
	return 1;
}

struct CostNode * dequeue(struct PriorityQueue * queue) {
    struct CostNode tmp;
    int size = queue -> size;
    struct CostNode * front = queue -> front;
    int index, need_swap = 1;
    dbg_printf("dequeue size: %d\n", size);
	if(size == 0)
        return NULL;
    else {
        /* swap the first and the last */
        size--;
        SWAP(front[size], front[0], tmp);
		queue -> size = size;
		/* restore heap features, compare with 2k + 1 & 2k + 2 */
		index = 0;

		while(need_swap) {
			need_swap = 0;
			if(2 * index + 1 < size && front[index].cost > front[2 * index + 1].cost) {
			    SWAP(front[index], front[2 * index + 1], tmp);
			    /* update map */
			    queue -> map[front[index].node] = index;
		    	queue -> map[front[2 * index + 1].node] = 2 * index + 1;
				/* move to next */
			    index = 2 * index + 1;
			    need_swap = 1;
			}
			else if(2 * index + 2 < size && front[index].cost > front[2 * index + 2].cost) {
	    		SWAP(front[index], front[2 * index + 2], tmp);
	    		/* update map */
	    		queue -> map[front[index].node] = index;
		    	queue -> map[front[2 * index + 2].node] = 2 * index + 2;
	    		/* move to next */
	    		index = 2 * index + 2;
	            need_swap = 1;
			}
			if(2 * index + 1 >= size)
			    break;
		}
    }
    return &front[size];
}

int Dijkstra(struct CostNode ** costs, int num, int * sizes, int src, int dest) {
    int i, size;
	struct PriorityQueue queue;
	int now_cost = 0; /* current node's min cost */
	struct CostNode * now_node;
	int node, cost;
    int next = src;
    int * route = malloc(num * sizeof(int));
    int * flag = malloc(num * sizeof(int));
    memset(flag, 0, num * sizeof(int));

    /* init queue */
    queue.size = 0;
    queue.capacity = num;
    queue.front = malloc(queue.capacity * sizeof(struct CostNode));
    queue.map = malloc(num * sizeof(int));
    for(i = 0; i < num; i++) {
    	queue.map[i] = -1;
    	route[i] = -1;
    }
    dbg_printf("next %d, cost %d\n", next, now_cost);
    while(next != dest) {
        flag[next] = 1;
        size = sizes[next];
        for(i = 0; i < size; i++) {
            node = costs[next][i].node;
            cost = costs[next][i].cost;
            if(!flag[node]) {
                if(enqueue(&queue, node, now_cost + cost)) {
                	dbg_printf("node: %d, next: %d\n", node, next);
                	route[node] = next;
                }
            }
		}
		do {
			now_node = dequeue(&queue);
			if(now_node == NULL) {
			    return INFINITY;
			}
			now_cost = now_node -> cost;
			next = now_node -> node;
		} while (flag[next]);
		dbg_printf("next %d, cost %d\n", next, now_cost);
	}
	/* print reverse route */
	dbg_printf("%d", dest);
	while(route[dest] != -1) {
		dbg_printf("<-%d", route[dest]);
		dest = route[dest];
	}
	dbg_printf("\n");

	free(route);
	free(queue.front);
	free(flag);
	return now_cost;
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
	struct CostNode ** costs;
	int size;
	int * sizes;
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
	costs = malloc(node * sizeof(struct CostNode *));
	dbg_printf("node size: %d, edge size: %d\n", node, edge);
	/* init */
	sizes = malloc(node * sizeof(int));
	memset(sizes, 0, node * sizeof(int));

	for(i = 0; i < node; i++) {
		costs[i] = malloc(node * sizeof(struct CostNode));
	}
	/* set costs */
	for(i = 0; i < edge; i++) {
		if(getline(&line, &n, testfile) > 0) {
			if(sscanf(line, "%d %d %d", &start, &end, &cost) != 3)
				exit(1);
			size = sizes[start];
			costs[start][size].node = end;
			costs[start][size].cost = cost;
			sizes[start] = size + 1;
		}
	}
	/* print costs */
	for(i = 0; i < node; i++) {
		for(j = 0; j < sizes[i]; j++)
			dbg_printf("%d\t", costs[i][j].cost);
		dbg_printf("\n");
	}
	start_time = milli_time();
	cost = Dijkstra(costs, node, sizes, src, dest);
	end_time = milli_time();
	printf("\nsrc %d, dest %d, cost %d, computing time: %lu us\n",
		src, dest, cost, end_time - start_time);
	return 0;
}
