#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#include "main.h"

config* ReadConfigFile(const char* path){
    config *Configuration = malloc(sizeof(config));;

    FILE* config_file = fopen(path, "r");

    char data[50];
    if(config_file == NULL){
        printf("[ERROR]: could not open file: %s\n", path);
        return NULL;
    }else{
        bool readFile = true;
        bool inSection = false;
        int sectionId = -1;

        while(readFile){
            if(!inSection){
                char section[50];
                bool foundSection = false;
                while(fscanf(config_file, "%s %*s", section) == 1){
                    printf("%s\n", section);
                    if(strcmp(section, "window") == 0){
                        inSection = true;
                        sectionId = 1;
                        printf("found section\n");
                        foundSection = true;
                        break;
                    }else if(strcmp(section, "container") == 0){
                        inSection = true;
                        sectionId = 2;
                        printf("found section\n");
                        foundSection = true;
                        break;
                    }
                }
                if(!foundSection){
                    readFile = false;
                }
            }else{
                if (sectionId == 1) {
                    char variable[50];
                    int value;
                    while(fscanf(config_file, "%s %*s %i", variable, &value) == 2){
                        printf("variable: %s , value: %i\n", variable, value);
                        if(strcmp(variable, "gap") == 0) Configuration->windowGap = value;
                        if(strcmp(variable, "padding") == 0) Configuration->windowPadding = value;
                    }
                    printf("section end\n");
                    inSection = false;
                    sectionId = -1;
                }else if(sectionId == 2){
                    char variable[50];
                    int value;
                    while(fscanf(config_file, "%s %*s %i", variable, &value) == 2){
                        printf("variable: %s , value: %i\n", variable, value);
                        if(strcmp(variable, "gap") == 0) Configuration->containerGap = value;
                        if(strcmp(variable, "padding") == 0) Configuration->containerPadding = value;
                    }
                    printf("section end\n");
                    inSection = false;
                    sectionId = -1;
                }else{
                    break;
                }
            }
        }
    }

    return Configuration;
}
