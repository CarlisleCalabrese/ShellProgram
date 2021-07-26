//Problem 1 - 8.21
//Creator: Carlisle Calabrese.
//Date: 3/8/2021.

/* Answer:
*  Possible Outputs:
*  bac.
*  abc.
*  acbc.
*  
* Outputs I've Seen So Far: bac.
*
* Given the topological arrangements I found in my analysis of the program, there are three possibilities, those being bac, abc, and (possibly) acbc.
*
*/

#include "csapp.h"

int main() {
  
  if (fork() == 0) {
    printf("a"); fflush(stdout);
    exit(0);
  } 
  else {
    printf("b"); fflush(stdout);
    waitpid(-1, NULL, 0);
  }
  printf("c"); fflush(stdout);
  exit(0);
}