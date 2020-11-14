#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATASIZE 21
#define SAVINGLEN 190
#define MAXWEIGHT 20
#define MAXDISTANCE 50

typedef struct {
    int l;
    int r;
    double value;
} Saving;

typedef struct {
    int route[43];
    int left;
    int right;
} Routes;

double distance[DATASIZE][DATASIZE];
Saving saving_distance[SAVINGLEN];
Routes routes[5];
char citySelected[21];

void read () {
    FILE *fp = NULL;
    if ((fp = fopen ("./distance.txt", "r")) == NULL) {
        printf ("FILE ERROR!\n");
        exit (0);
    }
    for (int i = 0; i < DATASIZE; ++i) {
        for (int j = 0; j < DATASIZE; ++j) {
            fscanf (fp, "%lf", &distance[i][j]);
        }
    }
    fclose (fp);
}

void createSD () {
    int length = 0;
    for (int i = 1; i < DATASIZE; ++i) {
        for (int j = i + 1; j < DATASIZE; ++j) {
            saving_distance[length].l = i;
            saving_distance[length].r = j;
            saving_distance[length++].value = distance[0][i] + distance[0][j] - distance[i][j];
        }
    } 
}

int partition (Saving *elem, int left, int right) {
    Saving pivot = elem[left];
    while (left < right) {
        while (left < right && elem[right].value <= pivot.value) --right;
        elem[left] = elem[right];
        while (left < right && elem[left].value >= pivot.value) ++left;
        elem[right] = elem[left];
    }
    elem[left] = pivot;
    return left;
}

void quickSort (Saving *elem, int left, int right) {
    if (left >= right) return;
    int pivotPos = partition (elem, left, right);
    quickSort (elem, left, pivotPos - 1);
    quickSort (elem, pivotPos + 1, right);
}

void createRoutes () {
    memset (routes, 0, sizeof (routes));
    memset (citySelected, 0, sizeof (citySelected));


    int i = 0, j = 0, cnt = 0;
    while (cnt >= 20) {


    }
}

int main () {
    read ();
    createSD ();
    quickSort (saving_distance, 0, SAVINGLEN);
    for (int i = 0; i < SAVINGLEN; ++i) {
        printf ("%02d%02d: %5lf\n", saving_distance[i].l, saving_distance[i].r, saving_distance[i].value);
    }
    printf ("\n");
     
    for (int i = 0; i < SAVINGLEN; ++i) {
        printf ("%02d%02d: %5lf\n", saving_distance[i].l, saving_distance[i].r, saving_distance[i].value);
    }
    return 0;
}
