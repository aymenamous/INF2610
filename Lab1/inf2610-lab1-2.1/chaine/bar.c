/*
 * bar.c
 *
 *  Created on: 2013-08-15
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "whoami.h"

int main(int argc, char **argv) {
	increment_rank();
	whoami("bar");
    int n = atoi(argv[1]);
    //Verification du nombre de repetition. Si n>1 alors on reboucle en executant le programme baz
    if (n > 1){
        n--;
        char *repetition;
        //repetition prend le nouveau nombre de repetition
        asprintf(&repetition,"%d",n);
        execlp("baz", "baz", repetition,NULL);
    }
	return 0;
}
