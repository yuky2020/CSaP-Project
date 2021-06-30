#include <stdio.h>
#include "util.h"
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
 AudioDataf test=recordP();
     playback(test);
 }

