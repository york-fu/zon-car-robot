
#include <contiki.h>
#include <stdio.h>

PROCESS(helloworld, "helloworld");

PROCESS_THREAD(helloworld, ev, data)
{
  PROCESS_BEGIN();
  
  printf("helloworld\r\n");
  PROCESS_END();
}