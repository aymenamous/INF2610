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
        // TODO: attendre notre tour
        for (j = 0; j < e->inner; j++) {
            unsigned long idx = (i * e->inner) + j;
            statistics_add_sample(e->data, (double) idx);
        }
        // TODO: signaler le travailleur suivant
    }
    return NULL;
}

void semrelay_init(struct experiment * e) {
    int i;

    e->data = make_statistics();
    // TODO: allocation d'un tableau de sémaphores sem_t dans e->lock
    // TODO: initialisation des sémaphores
}

void semrelay_destroy(struct experiment * e) {
    int i;

    // copie finale dans e->stats
    statistics_copy(e->stats, e->data);
    free(e->data);

    // TODO: destruction du verrou
    // TODO: liberation de la memoire du verrou
}

