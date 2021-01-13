#include <stdio.h>
#include "util.h"
#include <stdlib.h>
int main(int argc, char *argv[])
{
 AudioData *test=malloc(3*sizeof(AudioData));
 
 int kill=0;
 int i=0;
 switch (fork()) {
	    case 0:
          while(!kill){
              
              test[i]=record();
              i++;
              test=realloc(test,(i+1)*sizeof(AudioData));
          }
	  
	    case -1:
	    	perror("fork");
	    	break;
	    
      default:
	    	printf("premi un tasto quando hai finito");
        scanf("%d",&kill);
        kill=1;
        wait(5);
	}
 for(int j=0;j<=i;j++){
     playback(test[i]);
 }





}
