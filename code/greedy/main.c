#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATASIZE 21
#define SAVINGLEN 190
#define MAXWEIGHT 8
#define MAXDISTANCE 50

typedef struct {
    int l;
    int r;
    int used;
    double value;
} Sort;

typedef struct {
    int route[43];
    int left;
    int right;
} Routes;

double distance[DATASIZE][DATASIZE];
Sort sort_distance[DATASIZE][DATASIZE];
Routes routes[5];
int routesSize = 0;
char citySelected[21];
double weights[] = {0, 0.1, 0.4, 1.2, 1.5, 0.8, 1.3, 1.7, 0.6, 1.2, 0.4, 0.9, 1.3, 1.3, 1.9, 1.7, 1.1, 1.5, 1.6, 1.7, 1.5};
double allLen = 0;

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

int partition (Sort *elem, int left, int right, int mode) {
    Sort pivot = elem[left];
    if (mode == 0) {
        while (left < right) {
            while (left < right && elem[right].value >= pivot.value) --right;
            elem[left] = elem[right];
            while (left < right && elem[left].value <= pivot.value) ++left;
            elem[right] = elem[left];
        }
    } else if (mode == 1) {
        while (left < right) {
            while (left < right && elem[right].value <= pivot.value) --right;
            elem[left] = elem[right];
            while (left < right && elem[left].value >= pivot.value) ++left;
            elem[right] = elem[left];
        }
    } else {
        return -1;
    }
    elem[left] = pivot;
    return left;
}

void quickSort (Sort *elem, int left, int right,int mode) {
    if (left >= right) return;
    int pivotPos = partition (elem, left, right, mode);
    quickSort (elem, left, pivotPos - 1, mode);
    quickSort (elem, pivotPos + 1, right, mode);
}

void createSortDistance () {
    read ();
    for (int i = 0; i < DATASIZE; ++i) {
        for (int j = 0; j < DATASIZE; ++j) {
            sort_distance[i][j].value = distance[i][j];
            sort_distance[i][j].l = i;
            sort_distance[i][j].r = j;
            sort_distance[i][j].used = 0;
        }
        printf("\n");
    }
    printf ("\n");

    for (int i = 0; i < DATASIZE; ++i) {
        quickSort (sort_distance[i], 0, DATASIZE - 1, 0);
    }
}

void createRoutes () {
    memset (routes, 0, sizeof (routes));
    memset (citySelected, 0, sizeof (citySelected));
    for (int i = 0; i < 5; ++i) {
        routes[i].left = 21;
        routes[i].right = 22;
    }

    int cnt = 0;
    double length = 0, weight = 0;
    int i = 0, j = 0;
    while (cnt < 20) {
        if (routes[routesSize].route[routes[routesSize].left + 1] == 0 && routes[routesSize].route[routes[routesSize].right - 1] == 0) {// head adjacent to tail
            weight = length = i = j = 0;
        }
        i = j;

        // find active city's minimum neighbor
        j = 1;
        int city_out = sort_distance[i][j].r;
        while (j < DATASIZE && (city_out == 0 || sort_distance[i][j].used == 1 || citySelected[city_out] == 1)) {
            ++j;
            city_out = sort_distance[i][j].r;
        }
        if (j >= DATASIZE) {
            routesSize++;
            allLen += length;
            printf("1: %d\n", cnt);
            continue;
        }
        int city_in = sort_distance[i][j].l;
        double temp_len = length + distance[city_in][city_out] + distance[0][city_out] - distance[0][city_in];
        double temp_weight = weight + weights[city_out];
        if (temp_len > MAXDISTANCE || temp_weight > MAXWEIGHT) {
            routesSize++;
            allLen += length;
            printf("2: %d\n", cnt);
            continue;
        }
        length = temp_len;
        weight = temp_weight;
        citySelected[city_out] = 1;
        sort_distance[i][j].used = 1;
        cnt++;
        routes[routesSize].route[routes[routesSize].right++] = city_out;
        printf("3: %d\n", cnt);
    }
    routesSize++;
}

int main () {
    createSortDistance ();
    for (int i = 0; i < DATASIZE; ++i) {
        for (int j = 0; j < DATASIZE; ++j) {
            printf ("%02d%02d: %5lf   ", sort_distance[i][j].l, sort_distance[i][j].r, sort_distance[i][j].value);
        }
        printf("\n");
    }
    printf ("\n");

    createRoutes ();
    printf("routesSize: %d, allLen: %lf\n", routesSize, allLen);
    for (int i = 0; i < routesSize; ++i) {
        for (int j = routes[i].left, k = routes[i].right; j <= k; ++j) {
            printf("%d ", routes[i].route[j]);
        }
        printf("\n");
    }

    return 0;
}
