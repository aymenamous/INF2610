/*
 * baz.c
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
	whoami("baz");
    //Execute le programme suivant en passant l'argument argv[1] qui contient le nombre de repetition
    execlp("foo", "foo", argv[1],NULL);
	return 0;
}
