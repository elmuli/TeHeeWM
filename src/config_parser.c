#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <xkbcommon/xkbcommon.h>

#include "main.h"

void CheckBindAction(config *config, char *action, xkb_keysym_t key_sym){
    printf("checking for actions\n");
    if(strstr(action, "exit")){
        config->binds[config->keybind_count] = malloc(sizeof(keybind));
        config->binds[config->keybind_count]->key_sym = key_sym;
        config->binds[config->keybind_count]->action = exit_wm;
        config->keybind_count++;
        printf("keybind created for exit\n");
    }
}

typedef struct key{
    char* name;
    xkb_keysym_t key_sym;
} Keys;

Keys *CreateKeyList(Keys *list){
    list = {
        {"esc", XKB_KEY_Escape},
    };
    return list;
}

void CheckKeys(config *config, char* key, char* action){

}

config* ReadConfigFile(const char* path){
    config *Configuration = malloc(sizeof(config));
    Configuration->keybind_count = 0;
    Configuration->binds = malloc(sizeof(keybind*) * 100);

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
                    printf("data_value: %s\n", data_value);

                    if(sectionId == 3){

                        Keys *supportedKeys[];
                        CreateKeyList(supportedKeys);

                        char *m_key = NULL;
                        if((m_key = strtok(data, "+"))){
                            printf("m_key: %s\n", m_key);
                            char *key = strtok(NULL, "+");
                            printf("key: %s\n", key);
                            if(strstr(m_key, "mod")){
                                printf("has mod key\n");
                                if(strstr(key, "esc")){
                                    CheckBindAction(Configuration, data_value, XKB_KEY_Escape);
                                }
                            }
                        }
                    }else{
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
