//Problem 1 - 8.16
//Creator: Carlisle Calabrese.
//Date: 3/8/2021.

/*Answer:
* Output: counter = 2.
*Reason: When the program executes, while the child branch does execute and operate on the value with the Parent Process still running in the background, it then exits right after subtracting the value from its address for the Counter Variable. Upon the child returning, the Parent prints its value which is from a different address than the one used for the child's version of the same variable. This means that, in other words, the value displayed is 2, as the operations the child did to its version of the counter variable do not carry over to the operations performed on the parent's.
*/

#include "csapp.h"

int counter = 1;

int main() {
  if (fork() == 0) {
    counter--;
    exit(0);
    

  } else {
    wait(NULL);
    printf("counter = %d\n", ++counter);
  }
  exit(0);
}