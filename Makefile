default:
		(gcc time_util.c Dijkstra.c -o Dijkstra; gcc time_util.c Dijkstra_list.c -o Dijkstra_list; ./Dijkstra medium.txt 4 200 >> D.out; ./Dijkstra_list medium.txt 4 200 >> Dlist.out)
		
clean:
		rm -f *.o *~
