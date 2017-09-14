/*
 * banque.c
 *
 *  Created on: 2013-08-14
 *  Modified on: 2016-01-21
 *      Author: Francis Giraldeau <francis.giraldeau@gmail.com>
 */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <pth.h>
#include <dlfcn.h>
#include <getopt.h>
#include <string.h>
#include <sys/syscall.h>

#define PROGNAME "banque"
#define VAL_VERSION "2.1"
#define DEFAULT_REPEAT 10000000
#define DEFAULT_AMOUNT 100000000
static const char *const progname = PROGNAME;

struct account {
	volatile long balance;
};

struct operation {
	struct account *account;
	int amount;
	const char *name;
};

static struct account global_account;

static struct operation ops[] = {
	{ .account = &global_account, .amount = 9,  .name = "Canada" },
	{ .account = &global_account, .amount = -5, .name = "Usa" },
	{ .account = &global_account, .amount = 4,  .name = "France" },
	{ .account = &global_account, .amount = 3,  .name = "Chine" },
	{ .account = NULL, .amount = 0, .name = NULL },
};

typedef void (*spawn_method)();
void start_serial();
void start_fork();
void start_pthread();
void start_pth();

struct opts {
	unsigned long amount;
	unsigned long repeat;
	spawn_method lib;
};

static struct opts vars = {
	.amount = DEFAULT_AMOUNT,
	.repeat = DEFAULT_REPEAT,
	.lib = start_serial,
};

__attribute__((noreturn))
static void usage(void) {
	fprintf(stderr, "Usage: %s [OPTIONS] [COMMAND]\n", progname);
	fprintf(stderr, "Execute un programme a un emplacement fixe en memoire\n");
	fprintf(stderr, "\nOptions:\n\n");
	fprintf(stderr, "--lib LIB        type de fil d'execution a utiliser [ serial | fork | pthread | pth ]\n");
	fprintf(stderr, "--amount AMOUNT  montant de depart\n");
	fprintf(stderr, "--repeat NR      nombre de transactions\n");
	fprintf(stderr, "--help           ce message d'aide\n");
	exit(EXIT_FAILURE);
}

/*
 * Décodage des paramètres en arguments
 */
static void parse_opts(int argc, char **argv) {
	int opt;

	struct option options[] = {
		{ "help",       0, 0, 'h' },
		{ "repeat",     1, 0, 'r' },
		{ "amount",     1, 0, 'a' },
		{ "lib",        1, 0, 'l' },
		{ 0, 0, 0, 0}
	};
	int idx;

	while ((opt = getopt_long(argc, argv, "hr:a:l:", options, &idx)) != -1) {
		switch (opt) {
		case 'r':
			vars.repeat = atol(optarg);
			break;
		case 'a':
			vars.amount = atol(optarg);
			break;
		case 'l':
			if (strcmp(optarg, "serial") == 0)
				vars.lib = start_serial;
			else if (strcmp(optarg, "fork") == 0)
				vars.lib = start_fork;
			else if (strcmp(optarg, "pthread") == 0)
				vars.lib = start_pthread;
			else if (strcmp(optarg, "pth") == 0)
				vars.lib = start_pth;
			else
				usage();
			break;
		case 'h':
			usage();
			break;
		default:
			usage();
			break;
		}
	}
}

/* our own implementation of gettid specific to Linux */
int gettid()
{
        return (int) syscall(SYS_gettid);
}

/*
 * Retourne le nombre d'opérations d'un tableau de struct operation.
 */
int nr_ops(struct operation *ops) {
	int nr = 0;
	if (ops == NULL)
		return nr;
	while(ops->name != NULL) {
		nr++;
		ops++;
	}
	return nr;
}

/*
 * Routine d'exécution d'un ATM. Répète le nombre de fois spécifié l'opération sur le compte
 */
void *atm(void *data) {
	unsigned long i;
	struct operation *op = (struct operation *) data;

	for (i = 0; i < vars.repeat; i++)
		op->account->balance += op->amount;
	printf("ATM %12s balance: %11ld $ (pid=%d, tid=%d)\n", op->name, op->account->balance, getpid(), gettid());
	return NULL;
}


/*
 * Exécution en série des ATM.
 */
void start_serial(struct account *ac) {
	int i;
	int max = nr_ops(ops);
	for (i = 0; i < max; i++) {
		atm(&ops[i]);
	}
}

/*
 * Exécution des ATM dans des processus
 * Démarrez tous les processus simultanément
 */
void start_fork() {
	int i;
	int max = nr_ops(ops);
	for (i = 0; i < max; i++) {
		//On cree un nouveau processus, si ce dernier est un fils, alors il execute la methode atm
		if (fork() == 0){
			atm(&ops[i]);
			//A la fin, le processus fils s'arrete
			exit(EXIT_SUCCESS);		
		}
	}
	
	// On attends que tous les processus fils finissent leur excecution pour sortir de la methode
	for (i = 0; i < max; i++) {
		wait(NULL);	
	}
}

/*
 * Exécution des ATMs dans des fils d'exécution pthread
 * Démarrez tous les fils d'exécution simultanément
 */
void start_pthread() {
    int i;
	int max = nr_ops(ops);
	//On cree un tableau de thread ayant une capacite egale au nombre d'operations
	pthread_t threads[max];
	for (i = 0; i < max; i++) {
		//On cree chaque thread et on lui ordonne d'executer la methode atm
		pthread_create(&threads[i],NULL,atm,&ops[i]);
	}

	//On attends que tous les thread terminent leur execution pour sortir de la methode
	for (i = 0; i < max; i++) {
		pthread_join(threads[i],NULL);
	}
}

/*
 * Exécution des ATMs dans des fils d'exécution en espace utilisateur pth
 * Démarrez tous les fils d'exécution simultanément
 */
void start_pth() {
	//Initialisation des pth
	pth_init();
    int i;
	int max = nr_ops(ops);
	//On cree un tableau de pth ayant une capacite egale au nombre d'operations
	pth_t pths[max];
	for (i = 0; i < max; i++) {
		//On spawn chaque pth et on lui ordonne d'executer la methode atm
		pths[i] = pth_spawn(PTH_ATTR_DEFAULT, atm, &ops[i]); 
	}
	//On attends que tous les pth terminent leur execution pour sortir de la methode
	for (i = 0; i < max; i++) {
		pth_join(pths[i],NULL);
	}
}

/*
 * Retourne le nom d'une fonction correspondant à une adresse exécutable
 */
static inline char const *symname(void *addr) {
	Dl_info sym;
	int ret = dladdr(addr, &sym);
	if (ret != 0)
		return sym.dli_sname;
	return NULL;
}

int main(int argc, char **argv) {
	int i;
	int max = nr_ops(ops);
	long expected = 0;
	parse_opts(argc, argv);
	
	// Calcule la valeur finale attendue
	expected = global_account.balance = vars.amount;
	for (i = 0; i < max; i++) {
		expected += ops[i].amount * vars.repeat;
	}

	vars.lib();
	printf("%-21s %15ld $\n", "Start balance:", vars.amount);
	printf("%-21s %15ld $\n", "End balance:", global_account.balance);
	printf("%-21s %15ld $\n", "Expected:", expected);
	return 0;
}
