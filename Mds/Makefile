install:
	@echo "Install MDS"
	cc Mds.c ../lib/tools.c  ../lib/util.h -lrt -lm -lasound -ljack -pthread -lsndfile -o MDS
clean:
	@echo "Cleaning up..."
	rm MDS
	@echo "Removing also poisx sem starting with sem"
	rm /dev/shm/sem.*
