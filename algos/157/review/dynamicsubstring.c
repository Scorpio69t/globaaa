#include <string.h>
#include <stdio.h>

int max(int a, int b, int c);
void debug (int a, int b, int table[][b]);
void initialize(int a, int b, int table[][b]);
int solve(char* a, char* b, int alen, int blen, int table[][blen]);

int main(int argc, char* argv[]) {

	if (argc != 3) {
		printf("incorrect # of arguments\n");
		return 1;
	}

	char* a = argv[1];
	char* b = argv[2];
	int alen = strlen(a);
	int blen = strlen(b);

	int table[alen + 1][blen + 1];
	initialize(alen + 1, blen + 1, table);

	/* why does changing this to blen and alen and the comparisons to <= matter ?? */
	/* because blen gets passed to table in solve() */
	printf("Max substring: %d\n", solve(a, b, alen + 1, blen + 1, table));
//	debug(alen + 1, blen + 1, table);

	return 0;
}

int solve(char* a, char* b, int alen, int blen, int table[][blen]) {
	/* let's fill out table left-to-right, top-to-bottom */
        for (int i = 1; i < alen; i++) {
        	for(int j = 1; j < blen; j++) {
     	           if (a[i - 1] == b[j - 1]) {
     	              table[i][j] = max(1 + table[i-1][j-1], table[i][j-1], table[i-1][j]);
                   } else {
                      table[i][j] = max(table[i-1][j-1], table[i][j-1], table[i-1][j]);
                   }
                }
	}

	return table[alen - 1][blen - 1];
}

/* 3 way max -- what's the most efficient way to do this comparison again? */
int max(int a, int b, int c) {
	if (a >= b && a >= c) {
		return a;
	}

	if (b >= a && b >= c) {
		return b;
	}

	return c;
}

/* initialize our table with all zeros */
void initialize(int a, int b, int table[][b]) {
	for(int i = 0; i < a; i++) {
	  for(int j = 0; j < b; j++) {
	    table[i][j] = 0;
	  }
	}
}

void debug (int a, int b, int table[][b]) {
	for(int i = 0; i < a; i++) {
	  for(int j = 0; j < b; j++) {
	    printf("%d", table[i][j]);
	  }
	  printf("\n");
	}
}
