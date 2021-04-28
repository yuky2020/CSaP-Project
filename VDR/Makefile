install:
	@echo "Install VDR"
	gcc   VDR.c tools.c  util.h  -lrt -lm -lasound -ljack -pthread -lsndfile -o VDR
clean:
	@echo "Cleaning up..."
	rm VDR
