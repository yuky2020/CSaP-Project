install:
	@echo "Install VDR"
	cc   VDR.c ../lib/tools.c  ../lib/util.h  -lrt -lm -lasound -ljack -pthread -lsndfile -o VDR
clean:
	@echo "Cleaning up..."
	rm VDR
