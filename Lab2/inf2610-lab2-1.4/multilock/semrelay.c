/*
 * semrelay.c
 *
 *  Created on: 2013-08-19
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <semaphore.h>

#include "semrelay.h"
#include "statistics.h"
#include "multilock.h"
#include "utils.h"

void *semrelay_main(void * data) {
    unsigned long i, j;
    struct experiment * e = data;

    for (i = 0; i < e->outer; i++) {
    
        // from : https://www.mkssoftware.com/docs/man3/sem_wait.3.asp
        // TODO: attendre notre tour
        sem_wait(&(e->lock[i])); 
        
        for (j = 0; j < e->inner; j++) {
            unsigned long idx = (i * e->inner) + j;
            statistics_add_sample(e->data, (double) idx);
        }
        
        // from : https://www.mkssoftware.com/docs/man3/sem_post.3.asp
        // TODO: signaler le travailleur suivant
        sem_post(&(e->lock[i+1])); 
    }
    return NULL;
}

void semrelay_init(struct experiment * e) {
    int i;
    e->data = make_statistics();
    
    // from : https://stackoverflow.com/questions/26753957/how-to-dynamically-allocateinitialize-a-pthread-array
    // TODO: allocation d'un tableau de sémaphores sem_t dans e->lock
    e->lock = malloc(sizeof(sem_t));
    
    // from : https://www.mkssoftware.com/docs/man3/sem_init.3.asp
    // TODO: initialisation des sémaphores
    // ?? not sure
       // if i initialize all with 0 --> loops forever
       // if i initialize all in 1 --> incorrect result
    for (i = 0; i < e->nr_threads; i++)
       sem_init(&(e->lock[i]), 0, 1);
    
}

void semrelay_destroy(struct experiment * e) {
    int i;

    // copie finale dans e->stats
    statistics_copy(e->stats, e->data);
    free(e->data);
       
    // from : https://www.mkssoftware.com/docs/man3/sem_destroy.3.asp
    // TODO: destruction du verrou
    for (i = 0; i < e->nr_thread; i++)
       sem_destroy(&(e->lock[i])); 
    
    // TODO: liberation de la memoire du verrou
    free(e->lock);
}

