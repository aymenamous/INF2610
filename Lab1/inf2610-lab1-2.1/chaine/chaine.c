/*
 * chaine.c
 *
 *  Created on: 2013-08-15
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include "whoami.h"

/* Astuce:
 * Conversion de string à nombre: atoi()
 * Conversion de nombre à string: asprintf()
 */

int main(int argc, char **argv) {
    //Par defaut nombre de repetition 1 => afficher une seule fois baz, foo et bar
	int n = 1;
	if (argc == 2) {
	    n = atoi(argv[1]);
	}

	// ajoute le répertoire courant dans $PATH
	add_pwd_to_path(argv[0]);

	increment_rank();
	whoami("chaine");
    //Variable qui contient le nombre de repetition
    char *repetition;
    //Conversion de n en repetition
    asprintf(&repetition,"%d",n);
	// Exécution de n cycles foo bar baz
      execlp("baz", "baz", repetition,NULL);
	return 0;
}
