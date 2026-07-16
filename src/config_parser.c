#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <xkbcommon/xkbcommon.h>

#include "main.h"

#define KEY_COUNT 49

typedef struct key{
    char* name;
    xkb_keysym_t key_sym;
} Key;

void CreateKeyList(Key list[]){
    printf("creating key list\n");
    list[0] = (Key){" A ", XKB_KEY_a};
    list[1] = (Key){" B ", XKB_KEY_b};
    list[2] = (Key){" C ", XKB_KEY_c};
    list[3] = (Key){" D ", XKB_KEY_d};
    list[4] = (Key){" E ", XKB_KEY_d};
    list[5] = (Key){" F ", XKB_KEY_f};
    list[6] = (Key){" G ", XKB_KEY_g};
    list[7] = (Key){" H ", XKB_KEY_h};
    list[8] = (Key){" I ", XKB_KEY_i};
    list[9] = (Key){" J ", XKB_KEY_j};
    list[10] = (Key){" K ", XKB_KEY_k};
    list[11] = (Key){" L ", XKB_KEY_l};
    list[12] = (Key){" M ", XKB_KEY_m};
    list[13] = (Key){" N ", XKB_KEY_n};
    list[14] = (Key){" O ", XKB_KEY_o};
    list[15] = (Key){" P ", XKB_KEY_p};
    list[16] = (Key){" Q ", XKB_KEY_q};
    list[17] = (Key){" R ", XKB_KEY_r};
    list[18] = (Key){" S ", XKB_KEY_s};
    list[19] = (Key){" T ", XKB_KEY_t};
    list[20] = (Key){" U ", XKB_KEY_u};
    list[21] = (Key){" V ", XKB_KEY_v};
    list[22] = (Key){" W ", XKB_KEY_w};
    list[23] = (Key){" X ", XKB_KEY_x};
    list[24] = (Key){" Y ", XKB_KEY_y};
    list[25] = (Key){" Z ", XKB_KEY_z};
    list[26] = (Key){" F1 ", XKB_KEY_F1};
    list[27] = (Key){" F2 ", XKB_KEY_F2};
    list[28] = (Key){" F3 ", XKB_KEY_F3};
    list[29] = (Key){" F4 ", XKB_KEY_F4};
    list[30] = (Key){" F5 ", XKB_KEY_F5};
    list[31] = (Key){" F6 ", XKB_KEY_F6};
    list[32] = (Key){" F7 ", XKB_KEY_F7};
    list[33] = (Key){" F8 ", XKB_KEY_F8};
    list[34] = (Key){" F9 ", XKB_KEY_F9};
    list[35] = (Key){" F10 ", XKB_KEY_F10};
    list[36] = (Key){" F11 ", XKB_KEY_F11};
    list[37] = (Key){" F12 ", XKB_KEY_F12};
    list[38] = (Key){" 0 ", XKB_KEY_0};
    list[39] = (Key){" 1 ", XKB_KEY_1};
    list[40] = (Key){" 2 ", XKB_KEY_2};
    list[41] = (Key){" 3 ", XKB_KEY_3};
    list[42] = (Key){" 4 ", XKB_KEY_4};
    list[43] = (Key){" 5 ", XKB_KEY_5};
    list[44] = (Key){" 6 ", XKB_KEY_6};
    list[45] = (Key){" 7 ", XKB_KEY_7};
    list[46] = (Key){" 8 ", XKB_KEY_8};
    list[47] = (Key){" 9 ", XKB_KEY_9};
    list[48] = (Key){" ESC ", XKB_KEY_Escape};
    printf("keylist created\n");
}

bool CheckBindAction(config *config, char *action, xkb_keysym_t key_sym){
    printf("checking for actions\n");
    if(strstr(action, "exit")){
        config->binds[config->keybind_count] = malloc(sizeof(keybind));
        config->binds[config->keybind_count]->key_sym = key_sym;
        config->binds[config->keybind_count]->cmd = NULL;
        config->binds[config->keybind_count]->action = exit_wm;
        printf("keybind created for exit at: %i\n", config->keybind_count);
        config->keybind_count++;
        return true;
    }else if(strstr(action, "exec")){
        char *exec = NULL;
        if((exec = strtok(action, "("))){
            char *program = strtok(NULL, "(");
            program = strtok(program, ")");
            if(program == NULL) return false;
            config->binds[config->keybind_count] = malloc(sizeof(keybind));
            config->binds[config->keybind_count]->key_sym = key_sym;
            config->binds[config->keybind_count]->cmd = (char *)malloc(strlen(program) + 1);
            config->binds[config->keybind_count]->cmd_len = strlen(program) + 1;
            strcpy(config->binds[config->keybind_count]->cmd, program);
            config->binds[config->keybind_count]->action = exec_cmd;
            printf("keybind created for exec at: %i, for: %s\n", config->keybind_count, config->binds[config->keybind_count]->cmd);
            config->keybind_count++;
            return true;
        }
    }else if(strstr(action, "cycle_toplevel")){
        config->binds[config->keybind_count] = malloc(sizeof(keybind));
        config->binds[config->keybind_count]->key_sym = key_sym;
        config->binds[config->keybind_count]->cmd = NULL;
        config->binds[config->keybind_count]->action = cycle_toplevel;
        printf("keybind created for cycle_toplevel at: %i\n", config->keybind_count);
        config->keybind_count++;
        return true;
    }else if(strstr(action, "change_container")){
        config->binds[config->keybind_count] = malloc(sizeof(keybind));
        config->binds[config->keybind_count]->key_sym = key_sym;
        config->binds[config->keybind_count]->cmd = NULL;
        config->binds[config->keybind_count]->action = change_container;
        printf("keybind created for change_container at: %i\n", config->keybind_count);
        config->keybind_count++;
        return true;
    }else if(strstr(action, "create_vertical_container")){
        config->binds[config->keybind_count] = malloc(sizeof(keybind));
        config->binds[config->keybind_count]->key_sym = key_sym;
        config->binds[config->keybind_count]->cmd = NULL;
        config->binds[config->keybind_count]->action = create_vertical_container;
        printf("keybind created for chreate_vertical_container at: %i\n", config->keybind_count);
        config->keybind_count++;
        return true;
    }else if(strstr(action, "create_horizontal_container")){
        config->binds[config->keybind_count] = malloc(sizeof(keybind));
        config->binds[config->keybind_count]->key_sym = key_sym;
        config->binds[config->keybind_count]->cmd = NULL;
        config->binds[config->keybind_count]->action = create_horizontal_container;
        printf("keybind created for create_horizontal_container at: %i\n", config->keybind_count);
        config->keybind_count++;
        return true;

    }

    return false;
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
                        Key supportedKeys[49];
                        CreateKeyList(supportedKeys);

                        if(supportedKeys == NULL) return NULL;

                        char *m_key = NULL;
                        if((m_key = strtok(data, "+"))){
                            printf("m_key: %s\n", m_key);
                            char *key = strtok(NULL, "+");
                            printf("key: %s\n", key);
                            if(strstr(m_key, "mod")){
                                printf("has mod key\n");
                                for (int i=0;i<KEY_COUNT;i++){
                                    if(!strcmp(key, supportedKeys[i].name)){
                                        if(CheckBindAction(Configuration, data_value, supportedKeys[i].key_sym)) break;
                                    }
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
