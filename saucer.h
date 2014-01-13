/*
 * CMPUT 379,  Assignment 3, Fall 2013
 * Due Saturday, Nov 30 2013 (submit by 11:59pm)
 * (threads & synchronization & locking)
 * 
 * Submitted by: Jessica Surya
 *
*/

#pragma once

#include	<stdio.h>
#include	<curses.h>
#include	<pthread.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>

#define	MAXSAUCER	20	/* limit to number of saucers	*/
#define	MAXROCKET	20	/* limit to number of saucers	*/
#define	TUNIT   20000		/* timeunits in microseconds */

struct	propset {
	char	str[10];	/* the object to be animated */
	int 	col;
	int	row;	/* the row     */
	int	delay;  /* delay in time units */
	bool 	live;

};

pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t scr_mx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t esc_mx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rckt_mx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ufo_mx = PTHREAD_MUTEX_INITIALIZER;

struct 	propset	rcktInfo[MAXROCKET];
struct  propset ufoInfo[MAXSAUCER];

int 	score, escaped, ammo;
char * logPath;

void * animateSaucer();	

void moveHorizontal(struct propset * info, int col);

void moveVertical(struct propset * info);

void *setupUFO();

void *rocketThread();

bool detectHit(struct propset * rckt);

void buildRocket(int k, int column);

void displayInfo();

void logMSG(struct propset * info, int i, int j);