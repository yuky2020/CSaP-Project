#include <stdio.h>
#include "util.h"
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
 AudioDataf *test=malloc(1*sizeof(AudioData));
 int i=0;
 while(i<=6){
               test[i]=record() ;// record for five second
                
                i++;
                test=realloc(test,(i+1)*sizeof(AudioData));
          }
	  
	    	//printf("premi un tasto quando hai finito");
        //scanf("%d",&kill);
        //kill=1;
        //wait(5000);
	
 for(int j=1;j<=i;j++){
     playback(test[j]);
 }





}
