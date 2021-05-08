/*
 * File:	a3.c
 * Author:	Jim Diamond and Linh Truong
 * Date:	2020-05-17
 * Version:	1.0
 *
 * Purpose:	Simulation of the three scheduling (FCFS, SJF, RR) , as per of CSCI 3120 A3.
*/


#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include   <inttypes.h>
#include   <math.h>
#include   <time.h>
#include   <stdbool.h>



#ifdef DEBUG
#define D_PRNT(...) fprintf(stderr, __VA_ARGS__)
#else
#define D_PRNT(...)
#endif


#define     DEFAULT_INIT_JOBS	    5
#define     DEFAULT_TOTAL_JOBS	    100
#define     DEFAULT_LAMBDA	    ((double)1.0)
#define     DEFAULT_SCHED_TIME	    10		    // usec
#define     DEFAULT_CONT_SWTCH_TIME 50		    // usec
#define     DEFAULT_TICK_TIME	    10		    // msec
#define     DEFAULT_PROB_NEW_JOB    ((double)0.15)
#define     DEFAULT_RANDOMIZE	    false

enum sched_alg_T { UNDEFINED, RR, SJF, FCFS };
char * alg_names[] = { "UNDEFINED", "RR", "SJF", "FCFS" };

struct simulation_params
{
    enum sched_alg_T sched_alg;
    int init_jobs;
    int total_jobs;
    double lambda;
    int sched_time;
    int cont_swtch_time;
    int tick_time;
    double prob_new_job;
    bool randomize;
};

char * progname;

void
usage(const char * message)
{
    fprintf(stderr, "%s", message);
    fprintf(stderr, "Usage: %s <arguments>\nwhere the arguments are:\n",
            progname);
    fprintf(stderr, "\t-alg [rr|sjf|fcfs]\n"
                    "\t[-init_jobs <n1 (int)>\n"
                    "\t[-total_jobs <n2 (int)>]\n"
                    "\t[-prob_comp_time <lambda (double)>]\n"
                    "\t[-sched_time <ts (int, microseconds)>]\n"
                    "\t[-cs_time <cs (int, microseconds)>]\n"
                    "\t[-tick_time <cs (int, milliseconds)>]\n"
                    "\t[-prob_new_job <pnj (double)>]\n"
                    "\t[-randomize]\n");
}



/*
 * Name:	process_args()
 * Purpose:	Process the arguments, checking for validity where possible.
 * Arguments:	argc, argv and a struct to hold the values in.
 * Outputs:	Error messages only.
 * Modifies:	The struct argument.
 * Returns:	0 on success, non-0 otherwise.
 * Assumptions:	The pointers are valid.
 * Bugs:	This is way too long.  Maybe I should reconsider how
 *		much I dislike getopt().
 * Notes:	Sets the struct to the default values before
 *		processing the args.
 */

int
process_args(int argc, char * argv[], struct simulation_params * sps) {
    // Process the command-line arguments.
    // The only one which doesn't have a default (and thus must be
    // specified) is the choice of scheduling algorithm.

    char c;
    int i;

    for (i = 1; i < argc - 1; i++) {
        if (!strcmp(argv[i], "-alg")) {
            i++;
            if (!strcmp(argv[i], "rr"))
                sps->sched_alg = RR;
            else if (!strcmp(argv[i], "sjf"))
                sps->sched_alg = SJF;
            else if (!strcmp(argv[i], "fcfs"))
                sps->sched_alg = FCFS;
            else {
                usage("Error: invalid scheduling algorithm (-alg).\n");
                return 1;
            }
        } else if (!strcmp(argv[i], "-init_jobs")) {
            i++;
            if (sscanf(argv[i], "%d%c", &sps->init_jobs, &c) != 1
                || sps->init_jobs < 0) {
                usage("Error: invalid argument to -init_jobs\n");
                return 1;
            }
        }
    }

/// MORE CODE HERE!!
    return 0;
}

double
rand_exp(double lambda)
{
    int64_t divisor = (int64_t)RAND_MAX + 1;
    double u_0_to_almost_1;
    double raw_value;
    u_0_to_almost_1 = (double)random() / divisor;
    raw_value = log(1 - u_0_to_almost_1) / -lambda;
    return round(raw_value * 1000000.) / 1000000.;
}

int
main(int argc, char *argv[]) {
    progname = argv[0];
    struct simulation_params sim_params = {
          .sched_alg = UNDEFINED,
          .init_jobs = DEFAULT_INIT_JOBS,
          .total_jobs = DEFAULT_TOTAL_JOBS,
          .lambda = DEFAULT_LAMBDA,
          .sched_time = DEFAULT_SCHED_TIME,
          .cont_swtch_time = DEFAULT_CONT_SWTCH_TIME,
          .tick_time = DEFAULT_TICK_TIME,
          .prob_new_job = DEFAULT_PROB_NEW_JOB,
          .randomize = DEFAULT_RANDOMIZE
    };
    srandom(time(NULL));
    int t = 0; //the time
    int processes [100000] = {};
    int bt [100000] = {};
    int WaitTime[10000] = {};
    int turnaround [1000000] = {};
    int count = 0;
   double average_waiting_time = 0 ;
   double average_turnaround_time = 0;
   int  responseTime[10000] = {};
   double average_response_time = 0;
    int smallest=0,temp=0;
    while (t < sim_params.sched_time)
    {
        for (int j = 0; j < sim_params.init_jobs; j++)
        {
            //amount of processes into the queue
            processes[j] = j + 1;
            count++;
        }

        for (int m = 0; m < sim_params.init_jobs; m++)
        {
            double burstTime = rand_exp(sim_params.lambda);
            bt[m] = burstTime;
        }
        t++;
    }

    for(int i=0; i < 4 ; i++) {
        if (strcmp(argv[i], "fcfs") == 0) {

            WaitTime[0] = 0;
            turnaround[0] = bt[0];
            for (int i = 1; i < count; i++) {
                turnaround[i] = WaitTime[i] + bt[i];
                WaitTime[i] = WaitTime[i - 1] + bt[i - 1];
                responseTime[i] = WaitTime[i - 1] + bt[i - 1];

            }

            for (int i = 0; i < count; i++) {
                average_waiting_time += WaitTime[i];
                average_turnaround_time += turnaround[i];
                average_response_time += responseTime[i];
            }
            average_turnaround_time = average_turnaround_time / count;
            average_waiting_time = average_waiting_time / count;
            average_response_time = average_response_time / count;
        } else if (strcmp(argv[i], "sjf") == 0) {

            //sorting the burst time to lowest to highest

            for (int i = 0; i < count; i++) {
                smallest = i;
                for (int j = i + 1; j < count; j++) {
                    if (bt[smallest] > bt[j] ) {
                        smallest = j;
                    }
                }

                temp = bt[i];

                bt[i] = bt[smallest];
                bt[smallest] = temp;

            }
            //the first wait time is 0 because the first processor doesnt need to wait
            WaitTime[0] = 0;

            //calculating the wait time and average
            for (int i = 1; i < count; i++)
            {
                WaitTime[i] = 0;
                for (int j = 0; j < i; j++)
                {
                    WaitTime[i] += bt[j];
                }
                average_waiting_time += WaitTime[i];
            }
            //calculating the turnaround, response time and their average
            for (int i = 0; i < count; i++) {
                turnaround[i] = bt[i] + WaitTime[i];
                average_turnaround_time += turnaround[i];
                responseTime[i] = t + bt[i]; //the first time entering + burst time
                average_response_time += responseTime[i];
            }

            average_waiting_time = (float) average_waiting_time / count;      //average waiting time
            average_turnaround_time = (float) average_turnaround_time / count;
            average_response_time = (float) average_response_time / count;

        } else if (strcmp(argv[i], "rr") == 0) //else if alg is rr
        {
            int q = sim_params.tick_time; // q: quantum
            t = 0;

            for (int i = 0; i < count; i++) {

                if (bt[i] < q ) //when the brust time is less than the quantum number
                {
                    t += bt[i];
                    WaitTime[i] = t - bt[i];
                }
                //when burst time is greater than the quantum
                else{
                    t += q; //add time + the quantum together
                    bt[i] -= q; //subtract each burst time by the quantum
                }
            }



            //the first wait time is 0 because the first processor doesnt need to wait
            WaitTime[0]=0;

            //calculating the wait time and average
            for(int i=1;i < count;i++)
            {
                WaitTime[i]=0;
                for(int j=0; j < i;j++){
                    WaitTime[i] += bt[j];
                }
                average_waiting_time+=WaitTime[i];
            }
            //calculating the turnaround, response time and their average
            for(int i=0;i<count;i++) {
                turnaround[i] = bt[i] + WaitTime[i];
                average_turnaround_time += turnaround[i];
                responseTime[i] = t + bt[i]; //the first time entering + burst time
                average_response_time += responseTime[i];
            }

            average_waiting_time=(float)average_waiting_time/count;      //average waiting time
            average_turnaround_time = (float) average_turnaround_time/count;
            average_response_time = (float) average_response_time/count;
        }
    }



    if (process_args(argc, argv, &sim_params) != 0)
        return EXIT_FAILURE;

    // RUN THE SIMULATION HERE

    printf("For a simulation using the %s scheduling algorithm\n",
            alg_names[sim_params.sched_alg]);
    printf("with the following parameters:\n");
    printf("    init jobs           = %d\n", sim_params.init_jobs);
    printf("    total jobs          = %d\n", sim_params.total_jobs);
    printf("    lambda              = %.6f\n", sim_params.lambda);
    printf("    sched time          = %d\n", sim_params.sched_time);
    printf("    context switch time = %d\n", sim_params.cont_swtch_time);
    printf("    tick time           = %d\n", sim_params.tick_time);
    printf("    prob of new job     = %.6f\n", sim_params.prob_new_job);
    printf("    randomize           = %s\n",
            sim_params.randomize ? "true" : "false");
    printf("the following results were obtained:\n");
    printf("    Average response time:   %10.6lf\n", average_response_time);
    printf("    Average turnaround time: %10.6lf\n", average_turnaround_time);
    printf("    Average waiting time:    %10.6lf\n", average_waiting_time);

    return EXIT_SUCCESS;
}
