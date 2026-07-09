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

    if(config_file == NULL){
        printf("[ERROR]: could not open file: %s\n", path);
        return NULL;
    }else{
        int sectionId = 0;
        bool inSection = false;

        char line[100];
        char *section = NULL;
        while(fgets(line, sizeof(line), config_file)){
            printf("line %s\n", line);
            if(strstr(line, "#")) continue;
            if(!inSection){
                printf("looking for sections\n");
                if((section = strtok(line, "{"))){
                    printf("section %s\n", section);
                    if(strstr(section, "window")){
                        printf("section found, %s\n", section);
                        sectionId = 1;
                        inSection = true;
                    }else if(strstr(section, "container")){
                        printf("section found, %s\n", section);
                        sectionId = 2;
                        inSection = true;
                    }else if(strstr(section, "binds")){
                        printf("section found, %s\n", section);
                        sectionId = 3;
                        inSection = true;
                    }
                }
            }else{
                char *data = NULL;
                int value = 0;
                if(strstr(line, "}")){
                    inSection = false;
                    sectionId = 0;
                    continue;
                }
                if((data = strtok(line, "="))){
                    printf("data: %s\n", data);
                    char *data_value = strtok(NULL, "=");

                    if(sectionId == 3){
                        if(strstr(data_value, "mod")){

                        }
                    }else
                        if(data_value != NULL) value = atoi(data_value);
                        printf("value: %i\n", value);

                        if(strstr(data, "gap")){
                            if(sectionId == 1){
                                Configuration->windowGap = value;
                            }else if(sectionId == 2){
                                Configuration->containerGap = value;
                            }
                        }else if(strstr(data, "padding")){
                            if(sectionId == 1){
                                Configuration->windowPadding = value;
                            }else if(sectionId == 2){
                                Configuration->containerPadding = value;
                            }
                        }
                    }

                }
            }
        }
    }

    fclose(config_file);

    return Configuration;
}
