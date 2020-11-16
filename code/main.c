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
} Saving;

typedef struct {
    int route[43];
    int left;
    int right;
} Routes;

double distance[DATASIZE][DATASIZE];
Saving saving_distance[SAVINGLEN];
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

void createSD () {
    int length = 0;
    for (int i = 1; i < DATASIZE; ++i) {
        for (int j = i + 1; j < DATASIZE; ++j) {
            saving_distance[length].l = i;
            saving_distance[length].r = j;
            saving_distance[length].used = 0;
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
    for (int i = 0; i < 5; ++i) {
        routes[i].left = 21;
        routes[i].right = 22;
    }

    int cnt = 0;
    double length = 0, weight = 0;
    while (cnt < 20) {
        for (int i = 0; i < SAVINGLEN; ++i) {
            // 假设不会出现只送一个城市就超出里程或者载重量的情形
            // case 1: 路径生成初始时刻
            if (routes[routesSize].route[routes[routesSize].left + 1] == 0 && routes[routesSize].route[routes[routesSize].right - 1] == 0) {// head adjacent to tail
                if (cnt == 19) {
                    int k = 0;
                    while (citySelected[k] != 0) ++k;
                    citySelected[k] = 2;
                    routes[routesSize].route[routes[routesSize].right++] = k;
                    routesSize++;
                    allLen += 2 * distance[0][k];
                    return;
                }
                while (i < SAVINGLEN && saving_distance[i].used == 1) ++i;
                // 防止数组越界，虽然这里不会，但是加上异常处理是一个好习惯
                if (i >= SAVINGLEN) {
                    printf("saving overflow 1\n");
                    return;
                }
                int c1 = saving_distance[i].l, c2 = saving_distance[i].r;
                // 这里, 所剩城市 >= 2, 所以必能跳出 while
                while (citySelected[c1] != 0 || citySelected[c2] != 0) {
                    ++i;
                    while (i < SAVINGLEN && saving_distance[i].used == 1) ++i;
                    if (i >= SAVINGLEN) {
                        printf("saving overflow 2\n");
                        return;
                    }
                    c1 = saving_distance[i].l;
                    c2 = saving_distance[i].r;
                }
                length = distance[0][c1] + distance[0][c2] + distance[c1][c2];
                weight = weights[c1] + weights[c2];
                routes[routesSize].route[routes[routesSize].right++] = c1;
                routes[routesSize].route[routes[routesSize].right++] = c2;
                citySelected[c1] = citySelected[c2] = 1;
                saving_distance[i].used = 1;
                cnt += 2;
                continue;
            }


            // case 2: 路径中已有城市
            while (i < SAVINGLEN && saving_distance[i].used == 1) ++i;
            if (i >= SAVINGLEN) {
                for (int i = routes[routesSize].left + 1, j = routes[routesSize].right; i < j; ++i) {
                    citySelected[routes[routesSize].route[i]] = 2;
                }
                routesSize++;
                allLen += length;
                break;
            }
            int c1 = saving_distance[i].l, c2 = saving_distance[i].r;
            // 找路径的首尾城市相关的 saving
            while (!((citySelected[c1] == 0 && citySelected[c2] == 1) || (citySelected[c1] == 1 && citySelected[c2] == 0)) && ++i < SAVINGLEN) {
                while (i < SAVINGLEN && saving_distance[i].used == 1) ++i;
                if (i >= SAVINGLEN) break;
                c1 = saving_distance[i].l;
                c2 = saving_distance[i].r;
            }
            // 已无法合并
            if (i >= SAVINGLEN) {
                for (int i = routes[routesSize].left + 1, j = routes[routesSize].right; i < j; ++i) {
                    citySelected[routes[routesSize].route[i]] = 2;
                }
                routesSize++;
                allLen += length;
                break;
            }

            int city_in = citySelected[c1]? c1: c2;
            int head = routes[routesSize].left + 1, tail = routes[routesSize].right - 1;
            if (city_in != routes[routesSize].route[head] && city_in != routes[routesSize].route[tail]) continue;

            // 考虑 weight 和 length 约束
            int city_out = citySelected[c1]? c2: c1;
            double temp_len = length + distance[city_in][city_out] + distance[0][city_out] - distance[0][city_in];
            double temp_weight = weight + weights[city_out];
            if (temp_len > MAXDISTANCE || temp_weight > MAXWEIGHT) {
                for (int i = routes[routesSize].left + 1, j = routes[routesSize].right; i < j; ++i) {
                    citySelected[routes[routesSize].route[i]] = 2;
                }
                routesSize++;
                allLen += length;
                break;
            }
            length = temp_len;
            weight = temp_weight;
            citySelected[city_out] = 1;
            saving_distance[i].used = 1;
            cnt++;
            if (city_in == routes[routesSize].route[head]) {
                routes[routesSize].route[routes[routesSize].left--] = city_out;
            }
            if (city_in == routes[routesSize].route[tail]) {
                routes[routesSize].route[routes[routesSize].right++] = city_out;
            }
        }
    }
}

int main () {
    read ();
    createSD ();
    for (int i = 0; i < SAVINGLEN; ++i) {
        printf ("%02d%02d: %5lf\n", saving_distance[i].l, saving_distance[i].r, saving_distance[i].value);
    }
    printf ("\n");

    quickSort (saving_distance, 0, SAVINGLEN);
    for (int i = 0; i < SAVINGLEN; ++i) {
        printf ("%02d%02d: %5lf\n", saving_distance[i].l, saving_distance[i].r, saving_distance[i].value);
    }
    printf("\n");

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
