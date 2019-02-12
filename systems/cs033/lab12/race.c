#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "./race.h"

#define NFIB 40
#define BUF_SIZE 1024

int someone_finished = 0;

int main() {
    race_t *race = race_init();
    create_racers(race);
    start_race(race);
    return 0;
}

race_t *race_init() {
    race_t *race = (race_t *) malloc(sizeof(race_t));

    /**
     * TODO:
     * 1. initialize barrier and race mutex
     */

    pthread_barrier_init(&(race->barrier), 0, 4);
    pthread_mutex_init(&(race->mutex), 0);

    return race;
}

int create_racers(race_t *race) {
    race->team_1 = create_team("Dory");
    race->team_2 = create_team("Nemo");
    return 0;
}

race_team_t *create_team(char *team_name) {
    race_team_t *team = (race_team_t *) malloc(sizeof(race_team_t));
    team->racer_a = create_racer(team_name);
    team->racer_b = create_racer(team_name);
    return team;
}

racer_t *create_racer(char *team_name) {
    racer_t *racer = (racer_t *) malloc(sizeof(racer_t));
    racer->finished = 0;
    racer->team_name = team_name;

    /**
     * TODO:
     * 1. initialize the racer's associated mutex and condition variable
     */

    pthread_mutex_init(&(racer->mutex), 0);
    pthread_cond_init(&(racer->done), 0);

    return racer;
}

void destroy_race(race_t *race) {
    /**
      * TODO: destroy the race barrier, race mutex, and all racer condition variables/mutexes
      */

    pthread_barrier_destroy(&race->barrier);
    pthread_mutex_destroy(&race->mutex);
    pthread_cond_destroy(&race->team_1->racer_a->done);
    pthread_cond_destroy(&race->team_1->racer_b->done);
    pthread_cond_destroy(&race->team_2->racer_a->done);
    pthread_cond_destroy(&race->team_2->racer_b->done);

    free(race->team_1->racer_a);
    free(race->team_1->racer_b);
    free(race->team_2->racer_a);
    free(race->team_2->racer_b);

    free(race->team_1);
    free(race->team_2);

    free(race);
}

void *run_racer_a(void *args) {
    /**
     * TODO:
     * 1. wait at barrier
     * 2. call calculate()
     * 3. set finished to 1
     * 4. signal condition variable
     */

    pthread_barrier_wait(&(((args_t*) args)->race->barrier));
    calculate();
    ((args_t*) args)->team->racer_a->finished = 1;
    pthread_cond_broadcast(&(((args_t*) args)->team->racer_a->done));

    return 0;
}

void *run_racer_b(void *args) {
    /**
     * TODO:
     * 1. wait at barrier
     * 2. wait on condition variable
     * 3. call handoff()
     * 4. call calculate()
     * 5. lock race mutex, call announce, unlock race mutex
     */

    pthread_barrier_wait(&(((args_t*) args)->race->barrier));
    pthread_cond_wait(&(((args_t*) args)->team->racer_a->done), &(((args_t*) args)->team->racer_b->mutex));
    handoff(((args_t*) args)->team->racer_b->team_name);
    calculate();
    pthread_mutex_lock(&(((args_t*) args)->race->mutex));
    announce(((args_t*) args)->team->racer_b->team_name);
    pthread_mutex_unlock(&(((args_t*) args)->race->mutex));

    return 0;
}

void start_race(race_t *race) {
    /**
     * TODO:
     * 1. initialize the argument wrapper
     * 2. create racer threads
     * 3. start race!
     * 4. join threads
     */

    pthread_t thr[4];
    args_t array[4];

    // racer_a
    array[0].race = race;
    array[0].team = race->team_1;

    array[1].race = race;
    array[1].team = race->team_2;

    // racer_b
    array[2].race = race;
    array[2].team = race->team_1;

    array[3].race = race;
    array[3].team = race->team_2;

    for(int i = 0; i < 2; i++) {
      args_t* ptr = &array[i];
      pthread_create(&thr[i], 0, run_racer_a, (void*) ptr);
    }

    for(int i = 2; i < 4; i++) {
      args_t* ptr = &array[i];
      pthread_create(&thr[i], 0, run_racer_b, (void*) ptr);
    }

    for(int i = 0; i < 4; i++) {
      pthread_join(thr[i], 0);
    }

    // all racers are done
    printf("All racers have finished\n");
    destroy_race(race);
}

void calculate() {
    fibonacci(NFIB);
}

int fibonacci(unsigned int a) {
    if (a < 2) {
        return 1;
    } else {
        return fibonacci(a - 1) + fibonacci(a - 2);
    }
}

void announce(char *team) {
    if (!someone_finished) {
        printf("Team %s has locked the Golden Mutex and won the race!\n", team);
    } else {
        printf("Team %s has come in second place, securing the Silver Mutex!\n",
            team);
    }

    someone_finished = 1;
}


/* printf is thread-safe, so racing threads shouldn't use it.
   this function is used so a team can announce when it
   has handed off without impeding progress of the other team. */
void *handoff_print(void *msg) {
    printf((char *)msg);
    free(msg);
    return 0;
}

void handoff(char* team) {
    pthread_t print_thread;
    char *buf = malloc(BUF_SIZE);
    snprintf(buf, BUF_SIZE, "Racer A of team %s has finished!\n", team);
    pthread_create(&print_thread, 0, handoff_print, buf);
}
