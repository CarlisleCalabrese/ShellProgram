/*
 * shell.c
 * Carlisle Calabrese
 *Honestly, I don't know what to thinks about
 * this program. On one hand, the fact I got a shell working at all given my intelligence is
 * amazing. But I also think it was me just stumbling upon solutions when trying random things to make each step work.
 * This is my finished version. I mean there are things I do want to add in the future (fg restart command and maybe a few options to look up stuff on the jobs list like dea processes, etc.), but for now, this is pretty amazing on its own.
 *It's especially amazing considering the fact this program was amazingly unstable to the point where I had to have several backups on hand in case I ruined it entirely.
 *But honestly, given how much hard work and effort I have put into it, I can definitely say that I am stil pleased with the results.
 * Based on figures 8.23-8.25, pages 754-756 in "Computer Systems: A
 * Programmer's Perspective," by Bryant and O'Hallaron.
 */

/*
 * Get csapp.h and csapp.c from http://csapp.cs.smu.edu/3e/code.html
 *
 * $ gcc shell.c csapp.c -lpthread -o shell
 *
 * (Ignore any compilation warnings related to code in csapp.c.)
 *
 * $ ./shell
 */

#include "csapp.h"

#define TRUE 1
#define FALSE 0

#define MAXARGS 128

#define MAXCHARS 64

pid_t fg_pid = 0;
int next_jid = 1;


typedef struct list_t
{
    pid_t pid;
    int jid;
    char *runstat;
    char *cmdline;
    struct list_t *next;
} list_t;

list_t *jobs_list = NULL;

void add_element(list_t **list, pid_t pid, int jid, char *runstat, char *cmdline)
{
    list_t *e;

    if (*list == NULL)  // New empty list.
    {
        *list = (list_t *) malloc(sizeof(list_t));
        (*list)->pid = pid;
        (*list)->jid = jid;
        (*list)->runstat = strndup(runstat, MAXCHARS);
        (*list)->cmdline = strndup(cmdline, MAXCHARS);
        (*list)->next = NULL;
    }
    else  // List with at least one element.
    {
        // Loop through elements, so that e is left
        // pointing to the last one in the list.
        for (e = *list; e->next != NULL; e = e->next)
            ; // (Do nothing.)

        e->next = (list_t *) malloc(sizeof(list_t));
        e = e->next;
        e->pid = pid;
        e->jid = jid;
        e->runstat = strndup(runstat, MAXCHARS);
        e->cmdline = strndup(cmdline, MAXCHARS);
        e->next = NULL;
    }
}

void get_current_name(pid_t pid, char *name, list_t **list) {
  //Returns a process's current name.
  list_t *e;

  e = *list;


  if (e->next == NULL){
       strncpy(name, e->cmdline, MAXCHARS);
    } else {

        for (;  e != NULL;  e = e->next) {

            if (pid == e->pid) {
                strncpy(name, e->cmdline, MAXCHARS);
                break;
            }
        }
  }

}

void get_running_state(pid_t pid, char *status, list_t **list) {
  //Returns the running status of a job in the list.
  list_t *e;

  e = *list;


  if (e->next == NULL){
       strncpy(status, e->runstat, MAXCHARS);
    } else {

        for (;  e != NULL;  e = e->next) {

            if (pid == e->pid) {
                strncpy(status, e->runstat, MAXCHARS);
                break;
            }
        }
  }
 
}

int jid_to_pid(int jid, list_t **list) {
  pid_t p;
  list_t *e;
  
  e = *list;

  //Returs corresponding PID of the job. 
  if (e->next == NULL){
      p = e->pid;
      return p;
    } else {

        for (;  e != NULL;  e = e->next) {
          
            if (jid == e->jid) {
                p = e->pid;
                return p;
                break;
            }
        }
    }



}

void change_job_name(list_t **list, pid_t pid, char *name) {
    //Function for changing the name of a job on the list.
  
    
    list_t *e;
    e = *list;

    char *newname;
    newname = strdup(name);
    
    
    if (e->next == NULL){
       free(e->cmdline);
       e->cmdline = newname;
    } else {

        for (;  e != NULL;  e = e->next) {
          //Changes the name of the process.
            if (pid == e->pid) {
                free(e->cmdline);
                e->cmdline = newname;
                break;
            }
        }
    }
}

void change_running_status(list_t **list, pid_t pid, char *runstat) {
  //This function has gone through tons of revisions just to get it working.
  //But I finally did, after changing the entire way the value was copied over to the list that is.
  
    
    list_t *e;
    e = *list;

    char *newrunstat;
    newrunstat = strdup(runstat);
    
    
    if (e->next == NULL){
       free(e->runstat);
       e->runstat = newrunstat;
    } else {

        for (;  e != NULL;  e = e->next) {

            if (pid == e->pid) {
                //Had to use StackOverflow to figure out how to solve a Malloc Error that occured whenever I tried to change the value of the list.
                //Apparently, you have to free the old value of the list before you can write to it (That's how most solved this issue anyway).

                free(e->runstat);
                e->runstat = newrunstat;
                break;
            }
        }
    }
}

void sigint_handler(int signal) {

    // Restore default behavior for next SIGINT (which will likely
    // come from call to raise at the end of this function).
    Signal(SIGINT, SIG_DFL);

    if (fg_pid != 0) {
      Kill(-fg_pid, SIGINT); //Exits out of the child process (- = Send to group).
      printf("Job %d has been terminated by: User Interrupt (SIGINT) \n", fg_pid);
      Signal(SIGINT, sigint_handler);
    } else {
      // Send SIGINT to self.  (This time won't be caught be handler,
      // will instead cause process to terminate.)
      raise(SIGINT); 
    }
  
    
}


void sigtstp_handler(int signal) {
  //Restores SIGSTOP to normal behavior.
  Signal(SIGTSTP, SIG_DFL);
  
  //Stops the process.
  if (fg_pid != 0) {
    kill(-fg_pid, SIGTSTP);
    printf("Job %d has been stopped by: User Stop (SIGTSTP)\n", fg_pid);
    Signal(SIGTSTP, sigtstp_handler);
  } else { 
    raise(SIGTSTP);
  }

}

void sigcont_handler_bg(int signal, pid_t pid) {
  //This is the only unusual handler n that it has
  //arguments for both the pid of the process
  //and the signal to send to it.

  //These three variables are used in changing the running status and name to show taht it is now a background process.
  char *prevname[MAXLINE];
  char *newname[MAXLINE];
  char *curstat[MAXLINE];

  //Gets current name of process.
  get_current_name(pid, prevname, &jobs_list);

  //Copies that string to the new name.
  strncpy(newname, prevname, MAXCHARS);

  //Appends th & Symbol onto it.
  strcat(newname, " &");
  
  //Changes the Job name to the new background version.
  change_job_name(&jobs_list, pid, newname);
  
  //Gets the current status and creates the new status variable.
  get_running_state(pid, curstat, &jobs_list);
  char *runstat[MAXLINE];

  //If it is stopped, it then restarts hte process in the background.
  if (strncmp(curstat, "stopped", MAXCHARS) == 0) {
    kill(pid, SIGCONT);
    strncpy(runstat, "running", MAXCHARS);
    change_running_status(&jobs_list, pid, runstat);
    
    printf("Job %d has been restarted as a Background Process by User (SIGCONT)\n", pid);


  }
}

void sigcont_handler_fg() {
  //Foreground Handler.
}


/*
 * Populate argv with pointers to places in line where arguments
 * begin (and put \0 in buf where arguments end), so that argv[0] is
 * pointer to first argument, argv[1] pointer to second, etc.
 *
 * (You should't need to make any changes to this function.)
 */
int parseline(char *line, char **argv) {
    char *cp;
    int in_arg = FALSE;
    int argc = 0;
    int bg = FALSE;

    // Go through line, one character at a time, until reaching the
    // newline character at the end.
    for (cp = line; *cp != '\n'; cp++) {
    
        if (in_arg) {
        
            // If at the end of an argument...
            if (*cp == ' ') {
                *cp = '\0'; // Mark end of argument.
                in_arg = FALSE;
            }
        } else if (*cp != ' ') { // If at beginning of new argument...
            argv[argc++] = cp;   // Set argv array element to point to
                                 // new argument.
            in_arg = TRUE;
        }
    }

    *cp = '\0';  // Mark end of last argument (which was probably
                 // followed by \n, not space).

    // If at least one argument, and last argument is &, process is
    // to be run in background.
    if (argc > 0 && *argv[argc - 1] == '&') {
        bg = TRUE;
        argv[argc - 1] = NULL; // Overwrite address of "&" to mark
                               // end of argv.
    
    } else {                   // (Process should run in foreground.)
        argv[argc] = NULL;     // Mark end of argv.
    }

    return bg;
}

/*
 * If argv[0] is a builtin command, run it and return TRUE.  If it's
 * not, return FALSE.
 */
int builtin_command(char **argv) {

    if (strcmp(argv[0], "quit") == 0) {
        // (Don't bother to return, just end the program.)
        exit(0);
    
    } else if (strcmp(argv[0], "&") == 0) {
        // (Ignore & if it isn't preceded by a command.)
        return TRUE;
    } else if (strcmp(argv[0], "jobs") == 0) {
      // Prints list of background and stopped jobs.

      list_t *e;

      char *runstat[MAXLINE];
      for (e = jobs_list; e != NULL; e = e->next) {
        strncpy(runstat, e->runstat, MAXCHARS);
        //Eventually going to add an additional argument to allow it to print different lists depending on the argument.
        
        //Prints the process only if it's currently running in the system.
        if (strncmp(e->runstat, "running", MAXCHARS) == 0) {
          printf("[%d], %d, %s, %s", e->jid, e->pid, e->runstat, e->cmdline);
        }
        
      }
        

      return TRUE;
    } else if (strcmp(argv[0], "bg") == 0) {
      pid_t runpid;

      if (strcmp(argv[1], "%") == 0) {
        runpid = jid_to_pid(atoi(argv[2]), &jobs_list);
        sigcont_handler_bg(SIGCONT, runpid);
      } else {
         runpid = atoi(argv[1]);
         sigcont_handler_bg(SIGCONT, runpid);
      }
      
      

      return TRUE;
    }

    return FALSE;
}


/*
 * Evaluate command (a line of arguments).
 */
void eval(char *cmdline, char **envp) {
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;
    int jid;
    char *runstat[MAXLINE];
    
    //Used for my current implementation of status checking.
    int status;

    // Copy cmdline to buf, use parseline to populate argv based
    // on what's in buf (and set bg based on value returned from
    // parseline).
    strcpy(buf, cmdline);
    bg = parseline(buf, argv);

    // If at least one argument, and it's not a builtin command...
    // (If it is a builtin command the builtin_command function will
    // run it too, not just check whether it's builtin.)
    if (argv[0] != NULL && !builtin_command(argv)) {
        pid = Fork();

        if (pid == 0) { // In child.
          //Added to work with child processes and groups of processes.
          pid = getpid();
          setpgid(pid, pid);

          if (execve(argv[0], argv, envp) < 0) {
            printf("%s is an invalid command.\n", argv[0]);
            exit(0);
          }   

        } else if (!bg) { // In parent, child running in foreground.
            fg_pid = pid;
            strncpy(runstat, "running", MAXCHARS);
            
            jid = next_jid++;
            //Testing Print.
            printf("[%d] %d %s %s", jid, pid, runstat, cmdline);
            add_element(&jobs_list, pid, jid, runstat, cmdline);

            
            if (waitpid(pid, &status, WUNTRACED) != 0)
            {
            if (fg_pid != 0) {
              //added check due to the first if executing down here for no reason.
              //Used for determining if a foreground process exited, terminated, or stopped.
              if (WIFEXITED(status) >= 1) {

                strncpy(runstat, "exited", MAXCHARS);
                change_running_status(&jobs_list, pid, runstat);
                printf("[%d] %d %s %s", jid, pid, runstat, cmdline);

              } else if (WIFSIGNALED(status) >= 1) {

                strncpy(runstat, "interrupted", MAXCHARS);
                change_running_status(&jobs_list, pid, runstat);
                printf("[%d] %d %s %s", jid, pid, runstat, cmdline);
              
              } else if (WIFSTOPPED(status) >= 1) {
                strncpy(runstat, "stopped", MAXCHARS);
                change_running_status(&jobs_list, pid, runstat);
                printf("[%d] %d %s %s", jid, pid, runstat, cmdline);

              }
            }
 
            }
            
            
            
          fg_pid = 0;
            
            
            
        } else {          // In parent, child running in background.
        //Implemented the whole running thing in my usual crude methods of doing so.
            strncpy(runstat, "running", MAXCHARS);
            jid = next_jid++;
            //runstat = 'Running';
            //printf("[%d] %d %s %s", jid, pid, runstat, cmdline);

            add_element(&jobs_list, pid, jid, runstat, cmdline);
        }
    }
}

int main(int argc, char **argv, char **envp) {
    char cmdline[MAXLINE];

    Signal(SIGINT, sigint_handler);
    Signal(SIGTSTP, sigtstp_handler);

    while (TRUE) {      // exit(0) will be called from builtin_command
                        // if user enters "quit" command.
        printf("> ");
        Fgets(cmdline, MAXLINE, stdin);
        eval(cmdline, envp);
    }
}