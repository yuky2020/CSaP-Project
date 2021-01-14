#include <stdio.h>
#include "util.h"
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
 AudioData *test=malloc(3*sizeof(AudioData));
 int i=0;
 while(getchar() != '\n'){
                printf("premi enter quando hai finito di registrare ");
                test[i]=record();
                i++;
                test=realloc(test,(i+1)*sizeof(AudioData));
          }
	  
	    	//printf("premi un tasto quando hai finito");
        //scanf("%d",&kill);
        //kill=1;
        //wait(5000);
	
 for(int j=0;j<=i;j++){
     playback(test[i]);
 }





}
