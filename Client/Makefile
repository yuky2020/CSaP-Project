install:
	@echo "Install CLIENT"
	gcc  recorder.c  client.c tools.c  audioutil.c util.h libportaudio.a -lrt -lm -lasound -ljack -pthread -lsndfile -o CLIENT   
clean:
	@echo "Cleaning up..."
	rm CLIENT
