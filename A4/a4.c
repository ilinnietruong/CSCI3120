/*
* File: a4.c
* Author: Linh Truong | B00708389
* Date: 07/05/2020
* Version: 1.0
* Purpose: There are two stages to in this program. The first stage creates a variable number of threads,
* which the user input in the command-line argument. If the threads are created,
* set the sv to 1 and if it is waiting for some thread, set it to 0.
* If the threads should exit, set the variable to 2.  Each thread, change the value amount of times.
* In stage 2, is a continue of stage 1. The first command-line argument you need to add -sync with
* to the other two arguments as in Stage 1. You use the mutex for this stage.
* Note: to compile the program use this command, gcc a4.c -o a4 -pthread
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>


struct thread_p
{
    int count;  /* Count the amount of the changes in the value */
    int id;   /* Thread are assigned by unique IDS*/

};

int shared_value;    /* The Shared value */

/* Mutex allows thread to share the same resource. Useful for stage 2*/
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;

/*
* Name:	thread_code()
* Purpose: Starting the thread up and indicate the amount each thread changed the value.
* Arguments: The process (p).
* Outputs: The id of each process saying it is starting up and the amount each thread changed the value.
* Modifies: the thread process linked up to the process id
* Returns:	null
* Assumptions: Process (p) is valid
* Bugs:	none
* Notes: none
*/
void *
thread_code(void * p)
{
    int count = 0;
    struct thread_p *thread_process = (struct thread_p *)p;
    printf("I am thread %d, starting up now\n",thread_process->id);

    while (shared_value < 2)
    {
        if(shared_value == 0)   /*When shared value == 0, let it sleep*/
        {
            usleep(10);
        }
            /*When  shared_value == 1, then
             a) set shared_value = 0 and b) increments its own personal count of the number of
             times it does this*/
        else
        {
            shared_value = 0;  /*Reset to 0 */
            count++;
        }
    }

    printf("I am thread %d; I changed the value %d times\n",thread_process->id,count);
    thread_process->count = count;
    return NULL;
}

/*
* Name:	thread_mutex()
* Purpose:	Similar to thread_code() above, but used mutex for stage 2
* Arguments: *p ( type of thread)
* Outputs:	Indicate the thread's id and that it is starting. Also, each thread indicate how much it changes the value.
* Modifies:	 thread_process->id.
* Returns:	null
* Assumptions: Assume that process (p) is valid.
* Bugs:	none
* Notes: none
*/

void *
thread_mutex(void * p)
{
    int count = 0;
    struct thread_p *thread_process = (struct thread_p *)p;
    printf("I am thread %d, starting up now\n", thread_process->id);

    while (shared_value < 2)  /* End the loop when shared_value is not 1 or 0*/
    {
        pthread_mutex_lock(&mutex);
        if( shared_value == 0)
        {
            pthread_mutex_unlock(&mutex);
            usleep(10);
        }

        else /* When shared_value = 1 */
        {
            shared_value = 0;
            count++;
            pthread_mutex_unlock(&mutex);
        }
    }

    printf("I am thread %d; I changed the value %d times\n", thread_process->id, count);
    thread_process->count = count;
    return NULL;
}

/*
* Name:	usage
* Purpose: The command-line where if the user mistaken forgot an information
* Arguments: message
* Outputs:	Error message to remind the user to input the command-line
* Modifies:	none
* Returns:	None
* Assumptions: message is valid
* Bugs:	None
* Notes: None
*/
void
usage(char * message)
{
    fprintf(stderr, "%s", message);
    fprintf(stderr,"Usage:  %s [-sync] <Amount of thread> <The value> \n", message);
}

int
main(int argc, char * argv[])
{
    struct thread_p *thread_argument;
    pthread_t *thread;
    bool sync = false;    /* Sync is not used */
    int thread_num;  /*unique id */
    int id,ret;
    int amountChanges;  /* Amount of changes of the thread */
    int totalChanges = 0;  /*The sum of the changes of each thread */

    if(argc==4)
    {
        if(!strcmp(argv[1],"-sync"))
        {
            sync = true;
        }
        else /*No sync */
        {
            usage(argv[0]);
            return 0;
        }

        thread_num = atoi(argv[2]);
        amountChanges = atoi(argv[3]);
        if (thread_num <= 0 || amountChanges <= 0)
        {
            usage(argv[0]);
            return 0;
        }
    }

    else if (argc == 3)
    {
        thread_num = atoi(argv[1]);
        amountChanges = atoi(argv[2]);
        if (thread_num <= 0 || amountChanges <= 0)
        {
            usage(argv[0]);
            return 0;
        }
    }

    else
    {
        usage(argv[0]);
        return 0;
    }

    thread=malloc(thread_num *sizeof(pthread_t));
    if (thread == NULL)
    {
        fprintf(stderr,"Error in malloc thread.\n");
        return -1;
    }

    thread_argument = malloc(thread_num*sizeof(struct thread_p));
    if (thread_argument == NULL)
    {
        fprintf(stderr,"Error in malloc thread_args\n");
        return -1;
    }

    if (sync) /*when sync is false*/
    {
        pthread_mutex_init(&mutex, NULL);
        for (id = 0; id < thread_num; id++)
        {
            thread_argument[id].id = id;
            ret = pthread_create(&thread[id], NULL, thread_mutex, &thread_argument[id]);
            if (ret)
            {
                fprintf(stderr, "error %d in creating thread%d\n", ret, id);
                return -1;
            }
        }
    }
    else
    {
        for (id = 0; id < thread_num; id++)
        {
            thread_argument[id].id = id;
            ret = pthread_create(&thread[id], NULL, thread_code, &thread_argument[id]);
            if (ret)
            {
                fprintf(stderr, "error %d in creating thread%d\n", ret, id);
                return -1;
            }
        }
    }
    
/* thread does not need the mutex*/
    for (int i = 0; i < amountChanges; i++)
    {
        shared_value = 1;
        while (shared_value == 1)
        {
            usleep(10);
        }
    }

    shared_value = 2;

    /*adding the total of changes per thread*/
    for(id=0; id <thread_num; id++)
    {
        ret = pthread_join(thread[id], NULL);
        if (ret)
        {
            fprintf(stderr, "error %d in pthread_join thread%d\n",ret,id);
        }
        totalChanges = totalChanges + thread_argument[id].count;
    }

    free(thread);
    free(thread_argument);

    if( shared_value == 2) {
        printf("main(): setting sv to 2 and joining the threads\n");
    }

    printf("main(): all the threads have finished\n");
    printf("main(): I set sv to 1 %d times, the threads reset it %d times.\n",
           amountChanges, totalChanges);
    return 0;

}
