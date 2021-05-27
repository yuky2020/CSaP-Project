install:
	@echo "Install CLIENT"
	gcc  ../lib/recorder.c  client.c ../lib/tools.c  ../lib/audioutil.c ../lib/util.h ../lib/libportaudio.a -lrt -lm -lasound -ljack -pthread -lsndfile -o CLIENT   
clean:
	@echo "Cleaning up..."
	rm CLIENT
