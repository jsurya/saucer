/*
 * CMPUT 379,  Assignment 3, Fall 2013
 * Due Saturday, Nov 30 2013 (submit by 11:59pm)
 * (threads & synchronization & locking)
 * 
 * Submitted by: Jessica Surya
 *
*/

 #include "saucer.h"

int main(int ac, char *av[]) {

	/* set up curses */
	initscr(); // init curses
	crmode(); // disable required \r after each key press
	noecho(); // don't print out the character the key is bound to
	clear();

	int	       	c;		/* user input		*/
	pthread_t 	maker;
	int 		i = 0;
	int 		column = COLS/2;
	ammo = MAXROCKET+10;
	escaped = 0;

	pthread_create(&maker, NULL, setupUFO, NULL);
	pthread_mutex_init(&mx, NULL);
	pthread_mutex_init(&scr_mx, NULL);
	pthread_mutex_init(&esc_mx, NULL);
	pthread_mutex_init(&rckt_mx, NULL);
	pthread_mutex_init(&ufo_mx, NULL);
	
	struct propset launchSite = { "|", COLS, LINES-3, 0, 0}; 
	displayInfo();
	moveHorizontal(&launchSite, column);

	while(1) {
		displayInfo();
		if (ammo < 0 || escaped >= 3) {
			break;
		}

		c = getchar();

		if ( c == 'Q' || c == 'q' ) { // Q quits the program
			break;
		} 	
		if ( c == ' ' ) { // check if space button was pressed
			int k = i % MAXROCKET;
			buildRocket(k, column);
			i++;
			pthread_mutex_lock(&mx);
			ammo--;
			pthread_mutex_unlock(&mx);
			/* refresh info */
			displayInfo();
		}

		if ( c == ',') {
			if (column > 0) {
				column--;
				moveHorizontal(&launchSite, column);
			}
		}
		if ( c == '.') {
			if (column+1 < COLS-1) {
				column++;
				moveHorizontal(&launchSite, column);
			}	
		}
	}
	pthread_cancel(maker);
	endwin();
	return 0;
}

void displayInfo() {
	mvprintw(LINES-1,0,"%s %s %d %s %d %s %02d", 
	"'Q' to quit, ',' or '.'", 
	"to move launch site, score: ", score, 
	"escaped: ", escaped, "rockets: ", ammo);
}

void buildRocket(int k, int column) {
	pthread_t 	rckt[MAXROCKET];

	strcpy(rcktInfo[k].str, "^");
	rcktInfo[k].col =  column+1, 
	rcktInfo[k].row = LINES-4;
	rcktInfo[k].delay = 3;
	rcktInfo[k].live = true;
	pthread_create(&rckt[k], NULL, rocketThread, &rcktInfo[k]);
	pthread_detach(rckt[k]);
}

void *setupUFO() {
	char* string = "<--->";
	pthread_t      thrds[MAXSAUCER];

	int i = 0;
	int j;
	
	/* assign rows and velocities to each saucer */
	srand(getpid());
	while(1) {
		usleep(40*TUNIT);
		if (rand()%9 >= 7) {
			j = i % MAXSAUCER;
			if (ufoInfo[j].live == false && 
				pthread_mutex_lock(&ufo_mx) == 0) {
				strcpy(ufoInfo[j].str, string);	/* the saucer*/
				ufoInfo[j].col = 0;
				ufoInfo[j].row = (rand()%3);	/* the row */
				ufoInfo[j].delay = 5+(rand()%8); /* a speed */
				ufoInfo[j].live = true;
				logMSG(&ufoInfo[j], i, j);
				pthread_create(&thrds[j], NULL, 
				animateSaucer, &ufoInfo[j]);
				pthread_mutex_unlock(&ufo_mx);
			}
		}
		i++;
	}
}

void *animateSaucer(void *arg) {
	struct propset *info = arg;		/* point to info block	*/
	int	len = strlen(info->str)+2;	/* +2 for padding	*/
	int	col = 0;

	while(col+len < COLS && info->live == true) {
		usleep(info->delay*TUNIT);
		pthread_mutex_lock(&ufo_mx);
		moveHorizontal(info, col);
		info->col = col;
		pthread_mutex_unlock(&ufo_mx);
		col++;

	}
	pthread_mutex_lock(&esc_mx);
	if(info->live == true && !strcmp(info->str, "<--->")) {
		escaped++;
		displayInfo();
	}
	pthread_mutex_unlock(&esc_mx);
	info->live = false;
	strcpy(info->str, "     ");
	moveHorizontal(info, col);
	if (escaped >= 3) {
		displayInfo();
		endwin();
		exit(0);
	}
}

void *rocketThread(void *arg) {
	struct propset *rckt = arg;		/* point to info block	*/
	int	row = rckt->row;

	while(row > 0) {
		usleep(TUNIT*rckt->delay); // sleep for microseconds
		pthread_mutex_lock(&ufo_mx);	/* only one thread	*/
		moveVertical(rckt);
		if (detectHit(rckt)) {
			strcpy(rckt->str, " ");
			rckt->live = false;
		}
		pthread_mutex_unlock(&ufo_mx);	/* done with curses	*/
		row--;
	}
	pthread_mutex_lock(&rckt_mx);
	rckt->live = false;
	pthread_mutex_unlock(&rckt_mx);
	strcpy(rckt->str, " ");
	moveVertical(rckt);
}

void moveHorizontal(struct propset * info, int col) {
	move(info->row, col);	
	addch(' ');			
	addstr(info->str);		
	addch(' ');			
	move(LINES-1,COLS-1);	/* park extreme bottom right of the screen */
		/* to hide accidental key presses and the cursor itself  */
		/* from users */
	refresh();			/* and show it		*/
}

void moveVertical(struct propset * info) {
	move(info->row-1, info->col);	
	addstr(info->str);
	move(info->row, info->col);
	addch(' ');
	pthread_mutex_lock(&rckt_mx);
	info->row--;		
	pthread_mutex_unlock(&rckt_mx);
	move(LINES-1,COLS-1);	
	refresh();			/* and show it		*/
}

bool detectHit(struct propset * rckt) {
	int i;
	for (i = 0; i < MAXSAUCER; i++) {
		if (ufoInfo[i].live == true && rckt->live == true &&
			ufoInfo[i].row == rckt->row && 
			(rckt->col >= ufoInfo[i].col && 
				rckt->col <= 
				ufoInfo[i].col+strlen(ufoInfo[i].str))) {
			pthread_mutex_lock(&scr_mx);
			strcpy(ufoInfo[i].str, "     ");
			ufoInfo[i].live = false;
			score++;
			ammo+= 3;
			displayInfo();
			pthread_mutex_unlock(&scr_mx);
			return true;
		}

	}
	return false;
}

void logMSG(struct propset * info, int i, int j) {
	FILE * logFile = fopen("/cshome/jsurya/Dropbox/school/CMPUT_379/asn3j/saucer_log.txt", "a");
	fprintf(logFile,"index = %d\t %s\tlive = %d\tpos = %d\n", i, info->str, info->live,
	 j);
	fclose(logFile);
}
