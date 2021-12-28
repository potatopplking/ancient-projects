#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>

#define COMBINATION_LVL 4
#define WORLD_SIZE 100
#define MAX_GENERATIONS 1000
#define CELL_MID (WORLD_SIZE/2-COMBINATION_LVL/2)

/* global vars */
unsigned int combinations = 1;
char combination[COMBINATION_LVL*COMBINATION_LVL];
char *cell;
char *new_cell;
unsigned char **digest;
unsigned char empty_world_digest[16];

void clear_world() {
	int i;
	for (i = 0; i != WORLD_SIZE*WORLD_SIZE; i++) cell[i] = 0;
}

int compare_hash(const char *a, const char *b) { /* returns 1 if identical; 0 otherwise */
	int i;
	for (i = 0; i != 16; i++) {
		if (a[i] != b[i]) return 0;
	}
	return 1;
}

void cp_comb_to_cell(void) {
	unsigned int i,j;
	for (i = 0; i != COMBINATION_LVL; i++) {
		for (j = 0; j != COMBINATION_LVL; j++) {
			cell[CELL_MID*WORLD_SIZE+CELL_MID+i*WORLD_SIZE+j] = combination[i*COMBINATION_LVL+j];
		}
	}
}

void print_combinations(void) {
	unsigned int i,j;

	for (i = 0; i != COMBINATION_LVL; i++) {
		for (j = 0; j != COMBINATION_LVL; j++) {
			printf("%c ",combination[i*COMBINATION_LVL+j] ? '1' : '0');
		}
		putchar('\n');
	}
}

void print_cells(void) {
	unsigned int i,j;

	for (i = 0; i != WORLD_SIZE; i++) {
		for (j = 0; j != WORLD_SIZE; j++) {
			printf("%c ",cell[i*WORLD_SIZE+j] ? '1' : '0');
		}
		putchar('\n');
	}
}

void step(void) {
	int i,j,k,l;
	int cell_count;
	char *temp;

	for (i = 0; i != WORLD_SIZE; i++) {
			for (j = 0; j != WORLD_SIZE; j++) {
				cell_count = 0;
				for (k = -1; k != 2; k++) {
					for (l = -1; l != 2; l++) {
						if (k == 0 && l == 0) continue;
						if (i+k < WORLD_SIZE && i+k >= 0 && j+l >= 0 && j+l < WORLD_SIZE) {
							if (cell[(i+k)*WORLD_SIZE+j+l]) cell_count++;
						}
					}
				}
				if (cell_count == 3 || (cell[i*WORLD_SIZE+j] && cell_count == 2)) new_cell[i*WORLD_SIZE+j] = 1;
				else new_cell[i*WORLD_SIZE+j] = 0;
			}
		}
		temp = cell;
		cell = new_cell;
		new_cell = temp;
}

void new_combination(void) {
	int i;
	int carry = 1;

	for (i = COMBINATION_LVL*COMBINATION_LVL-1; i >= 0; i--) {
		if (carry) {
			if (combination[i]) combination[i] = 0;
			else {
				carry = 0;
				combination[i] = 1;
			}
		}
		else break;
	}
}

int main(void) {
	unsigned int i,j,k;
	unsigned int start_generation;
	int found_cycle;

	cell = (char *) malloc(WORLD_SIZE*WORLD_SIZE);
	new_cell = (char *) malloc(WORLD_SIZE*WORLD_SIZE);
	for (i = 0; i != COMBINATION_LVL*COMBINATION_LVL; i++) {
		combinations *= 2;
		combination[i]= 0;
	}
	digest = (unsigned char **) malloc(sizeof(unsigned char *)*MAX_GENERATIONS);
	for (i = 0; i != MAX_GENERATIONS; i++) digest[i] = (char *) malloc(16);
	for (i = 0; i != WORLD_SIZE*WORLD_SIZE; i++) cell[i] = 0;
	printf("%dx%d: %d combinations\n",COMBINATION_LVL,COMBINATION_LVL,combinations);
	MD5((unsigned char *)cell,WORLD_SIZE*WORLD_SIZE,empty_world_digest);
	printf("empty world digest: ");
	for (i = 0; i != 16; i++) printf("%x",empty_world_digest[i]);
	putchar('\n');
	new_combination();
	for (i = 1; i != combinations; i++) {
		clear_world();
		found_cycle = 0;
		cp_comb_to_cell();
		for (j = 0; j != MAX_GENERATIONS; j++) {
			MD5((unsigned char *)cell,WORLD_SIZE*WORLD_SIZE,digest[j]);
			if (compare_hash(digest[j],empty_world_digest)) break;
			for (k = 0; k != j; k++) {
				if (compare_hash(digest[j],digest[k])) break;
			}
			if (k != j) {
			 	found_cycle = 1;
				break;
			}
			step();
		}
		if (j == MAX_GENERATIONS) {
			printf("Dosazeno maximalni generace (%d); neukonceny vyvoj, nenalezeny shody:\n",j+1);
			print_combinations();
		}
		else if (found_cycle) {
			printf("Nasel jsem cyklus v generaci %d:\n",j+1);
			print_combinations();
		}
		else if (j > 10) {
			printf("Vymizeni veskereho zivota v generaci %d:\n",j+1);
			print_combinations();
		}
		new_combination();
	}
}