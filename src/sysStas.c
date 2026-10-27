#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "lib/clay.h"

int getCpuPr(){

    unsigned long long user, nice, sys, idle_c, iowait, irq, softirq, steal;

    FILE *f = fopen("/proc/stat", "r");
    if (!f) {
        return 0;
    }

    int n = fscanf(f, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
        &user, &nice, &sys, &idle_c,
        &iowait, &irq, &softirq, &steal);
    fclose(f);

    unsigned long long idle = idle_c + iowait;
    unsigned long long tot = idle + user + nice + sys + irq + softirq + steal;
    double usage = 100.0 * (double)((tot) - (idle)) / (double)(tot);

    return usage;
}

void getTime(char **text){
    *text = malloc(30);
    time_t mytime = time(NULL);
    char *time_str = ctime(&mytime);
    strcpy(*text, time_str);
}
