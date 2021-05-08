/*File: a2.c
 * Author: Linh Truong B00708389
 * Date: 2020/05/30
 * Version 1.0
 * Purpose: Creating a Shell in C
 * Reference:  http://people.cs.pitt.edu/~khalifa/cs449/spr07/Assigns/Assign4/myshell.c
 */

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


int main(void)
{
    static char line[1024];
    char *args[51];
    char string[100];
    int i ;
    int status;
    int pipe[2];
    pid_t pid;
    pid_t childP1;
    pid_t childP2;


    while (1)
    {
        printf("$ ");
        fflush(stdout);
        scanf("%[^\n]%*c", string);
        i = 0; /*reset to 0. */
        args[i] = strtok(string, " ");

        while (args[i] != NULL)
        {
            i++;
            args[i] = strtok(NULL, " "); /*blank space */
        }

        if (strcmp(args[0], "exit") == 0)  /*When the user enter "exit", the shell will end program */
        {
            break;
        }

        if (strcmp(args[i - 1], "&") != 0)
        {
            pid = fork();

            if ( pid > 0)
            {
                wait(NULL);
            }

            else if (pid == 0)
            {
                execvp(args[0], args);
                for (int j = 0; j < i; j++)
                {
                    args[j] = NULL;
                }
            }

            else /* when pid <0 */
            {
                fprintf(stderr, "FORK failed. Please try again..\n");
                return 1;
            }
        }

        else
        {
           pid = fork();

            /*piping procedure*/
            if (pid == 0)
            {
                args[i - 1] = NULL;
                execvp(args[0], args);
                childP1 = fork();

                if (childP1 == 0)
                {
                    dup2(pipe[1], 1); /*copy the read file */
                    close(pipe[0]); /* 0 is read. */
                    close(pipe[1]); /* 1 is write. */
                    execlp(args[1], NULL);
                }
                else
                {
                    childP2 = fork();
                    if (childP2 == 0)
                    {
                        dup2(pipe[0], 0); /*copy the write file */
                        close(pipe[0]);
                        close(pipe[1]);
                        execlp(args[2], NULL);
                    }
                }

                close(pipe[0]);
                close(pipe[1]);
                waitpid(childP1, &status, 0); /*waiting for first child process to be done/ */
            }
        }
    }
    return 0;
}
