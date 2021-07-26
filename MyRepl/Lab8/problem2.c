//Problem 2 - 8.17
//Creator: Carlisle Calabrese.
//Date: 3/8/2021.

/* Answer:
* Output It Gave Me:
* Hello
* 0
* 1
* Bye
* 2
* Bye
*
* Possible Outputs:
*
* Possibility 1:
* Hello
* 0
* 1
* Bye
* 2
* Bye
*
*Possibility 2 (Theoretical based on my analysis):
* Hello
* 1
* 0
* Bye
* 2
* Bye
*
*Possibility 3 (Theoretical based on my analysis):
* Hello
* 1
* Bye
* 0
* 2
* Bye
*/

#include "csapp.h"


int main() {
  int status;
  pid_t pid;

  printf("Hello\n");
  
  pid = fork();

  printf("%d\n", !pid);
  if (pid != 0) {

    if (waitpid(-1, &status, 0) > 0) {
     
      if (WIFEXITED(status) != 0) {
        printf("%d\n", WEXITSTATUS(status));
      }
    }
   

  }

  printf("Bye\n");
  exit(2);
}