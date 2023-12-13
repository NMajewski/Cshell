
#define open_f (O_RDONLY)
#define OPEN_FLAGS (O_WRONLY | O_TRUNC | O_CREAT)
#define DEF_MODE 0664

#include "executor.h"
#include "command.h"
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sysexits.h>
#include <unistd.h>

/* static void print_tree(struct tree *t); */

int execute(struct tree *t) {
  /* file I/O */
  int input = STDIN_FILENO, output = STDOUT_FILENO;
  /* wait() status */
  int status;
  /* process id for fork */
  pid_t pid;
  int pipes[2];

  if (t) {
    if (t->input) {
      if ((input = open(t->input, open_f)) < 0) {
        perror("input redirection error");
      }
    }
    if (t->output) {
      if ((output = open(t->output, OPEN_FLAGS, DEF_MODE)) < 0) {
        perror("output redirection error");
      }
    }

    /* Start by handling nodes with conjunction NONE, i.e., no pipes, no ands */
    if (t->conjunction == NONE) {
      /* implement internally the commands “cd” and “exit” */
      if (!strcmp(t->argv[0], "exit")) {
        exit(0);
      }
      if (!strcmp(t->argv[0], "cd")) {
        if (t->argv[1]) {
          /* in this case input is something like "cd ex_dir", where argv[1] is
           * not NULL */
          chdir(t->argv[1]);
        } else {
          /* in this case input is only "cd", no argv[1] */
          if ((chdir(getenv("HOME"))) < 0) {
            perror("cd failed");
            exit(EX_OSERR);
          }
        }
      } else {
        /* call fork, check if it failed, and initialize pid */
        if ((pid = fork()) < 0) {
          perror("fork failed_1");
          exit(EX_OSERR);
        } else if (pid) {
          /* parent code */
          wait(&status);
          return status;
        } else if (!pid) {
          /* child code */
          if (input != STDIN_FILENO) {
            if (dup2(input, STDIN_FILENO) < 0) {
              perror("dup2 input failed");
              exit(EX_OSERR);
            }
            close(input);
          }
          if (output != STDOUT_FILENO) {
            if (dup2(output, STDOUT_FILENO) < 0) {
              perror("dup2 output failed");
              exit(EX_OSERR);
            }
            close(output);
          }
          execvp(t->argv[0], t->argv);
          exit(-1);
          /* child code ends here */
        }
      }
    } else if (t->conjunction == AND) {
      /* conjunction && */
      if (!execute(t->left)) {
        if (!execute(t->right)) {
          return 0;
        } else {
          return -1;
        }
      } else {
        return -1;
      }

    } else if (t->conjunction == PIPE) { /* PIPE CONJUNCTION */
      /* check for amb out/in put redirect */
      if (t->left->output) {
        printf("Ambiguous output redirect.\n");
      } else {
        if (t->right->input) {
          printf("Ambiguous input redirect.\n");
        } else {
          pid_t pid_c; /* child pid */
          /* pf changs, pipe then fork */
          if (pipe(pipes) < 0) {
            perror("pipe failed");
            exit(EX_OSERR);
          }

          if ((pid_c = fork()) < 0) {
            perror("fork failed_2");
            exit(EX_OSERR);
          }

          if (pid_c) { /* parent code */
            wait(NULL);
            close(pipes[1]);

            if (dup2(pipes[0], STDIN_FILENO) < 0) {
              perror("dup2 failed");
              exit(EX_OSERR);
            }
            execute(t->right);
            close(pipes[0]);
          } else if (!pid_c) { /* child code */
            close(pipes[0]);

            if (dup2(pipes[1], STDOUT_FILENO) < 0) {
              perror("dup2 failed");
              exit(EX_OSERR);
            }
            execute(t->left);
            close(pipes[1]);
            exit(0);
          }
        }
      }
    } else if (t->conjunction == SUBSHELL) {
      pid_t pid_sub; /* child pid */

      if (t->input) {
        if ((input = open(t->input, open_f)) < 0) {
          perror("subshell input failed");
          exit(EX_OSERR);
        }
      }
      if (t->output) {
        if ((output = open(t->output, OPEN_FLAGS, DEF_MODE)) < 0) {
          perror("subshell output failed");
          exit(EX_OSERR);
        }
      }
      /* fork subshell */
      if ((pid_sub = fork()) < 0) {
        perror("fork_3 failed");
        exit(EX_OSERR);
      }
      /* reap child from parent process */
      if (pid_sub) {
        wait(NULL);
      }
      /* recursively call execute from child proc. i.e. if (!pid_sub) */
      if (!pid_sub) {
        execute(t->left);
        exit(0);
      }
    }
  }
  return 0;
}
/*
static void print_tree(struct tree *t) {
  if (t != NULL) {
    print_tree(t->left);

    if (t->conjunction == NONE) {
      printf("NONE: %s, ", t->argv[0]);
    } else {
      printf("%s, ", conj[t->conjunction]);
    }
    printf("IR: %s, ", t->input);
    printf("OR: %s\n", t->output);

    print_tree(t->right);
  }
}
*/
