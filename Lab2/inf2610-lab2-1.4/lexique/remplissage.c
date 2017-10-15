/*
 * remplissage.c
 *
 *  Created on: Aug 26, 2013
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

int main(int argc, char **argv) {
    int fds[2];
    int ret;
    char c = 'x';
    int i = 0;
    /*
     * TODO: Creer un tube, puis le remplir octet par octet a l'infini
     */
    //Si la creation de la pipe echoue, afficher message d'erreur et sortir du programme
    if(pipe(fds) == -1){	
	fprintf(stderr, "pipe failed\n");
        exit(0);
    }
    do{
        ret = write(fds[WRITE], &c, sizeof(c));
	i++;
        printf("Taille de la pipe : %ld\n Octets",i*sizeof(c) );
    }while(ret>0); //Si ret = -1 => erreur
    return 0;




    
    return 0;
}
