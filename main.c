#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "banker.h"

static bool is_blank_line(const char *line) {
    for (; *line != '\0'; line++) {
        if (!isspace((unsigned char)*line)) return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Erro: Forneca os recursos iniciais como argumentos.\n");
        return 1;
    }
    
    int num_resources = argc - 1;
    int num_customers = 0;
    char buffer[256];
    
    FILE *file = fopen("customer.txt", "r");
    if (!file) {
        fprintf(stderr, "Erro: nao foi possivel abrir customer.txt\n");
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), file)) {
        if (is_blank_line(buffer)) continue;
        num_customers++;
    }
    
    if (num_customers == 0) {
        fprintf(stderr, "Erro: customer.txt vazio.\n");
        fclose(file);
        return 1;
    }
    rewind(file);

    int *available   = (int *)malloc(num_resources * sizeof(int));
    int *max         = (int *)malloc(num_customers * num_resources * sizeof(int));
    int *allocation  = (int *)calloc(num_customers * num_resources, sizeof(int));
    int *need        = (int *)malloc(num_customers * num_resources * sizeof(int));
    int *request     = (int *)malloc(num_resources * sizeof(int));

    if (!available || !max || !allocation || !need || !request) {
        fprintf(stderr, "Erro: falha na alocacao de memoria.\n");
        if (available) free(available);
        if (max) free(max);
        if (allocation) free(allocation);
        if (need) free(need);
        if (request) free(request);
        fclose(file);
        return 1;
    }

    for (int i = 0; i < num_resources; i++) {
        available[i] = atoi(argv[i + 1]);
    }

    int i = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        if (is_blank_line(buffer)) continue;
        
        char *token = strtok(buffer, ",\r\n");
        int j = 0;
        
        while (token != NULL && j < num_resources) {
            char *endptr;
            long val = strtol(token, &endptr, 10);
            
            if (endptr == token || *endptr != '\0' || val < 0) {
                fprintf(stderr, "Erro: dado nao numerico ou negativo encontrado em customer.txt.\n");
                fclose(file);
                free(available); free(max); free(allocation); free(need); free(request);
                return 1;
            }
            
            max[i * num_resources + j]  = (int)val;
            need[i * num_resources + j] = max[i * num_resources + j];
            token = strtok(NULL, ",\r\n");
            j++;
        }
        
        if (j != num_resources || token != NULL) {
            fprintf(stderr, "Erro: customer.txt tem numero de recursos diferente dos argumentos fornecidos.\n");
            fclose(file);
            free(available); free(max); free(allocation); free(need); free(request);
            return 1;
        }
        i++;
    }
    fclose(file);

    FILE *cmd_file = fopen("commands.txt", "r");
    if (!cmd_file) {
        fprintf(stderr, "Erro: nao foi possivel abrir commands.txt\n");
        free(available); free(max); free(allocation); free(need); free(request);
        return 1;
    }

    FILE *out_file = fopen("result.txt", "w");
    if (!out_file) {
        fprintf(stderr, "Erro: nao foi possivel criar result.txt\n");
        fclose(cmd_file);
        free(available); free(max); free(allocation); free(need); free(request);
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), cmd_file)) {
        if (is_blank_line(buffer)) continue;
        
        if (buffer[0] == '*') {
            print_system_state(out_file, num_customers, num_resources, max, allocation, need, available);
            continue;
        }

        char type[3];
        int customer_id;

        if (sscanf(buffer, "%2s %d", type, &customer_id) != 2) {
            fprintf(stderr, "Erro: formatacao invalida no commands.txt.\n");
            fclose(cmd_file); fclose(out_file);
            free(available); free(max); free(allocation); free(need); free(request);
            return 1;
        }

        if (strcmp(type, "RQ") != 0 && strcmp(type, "RL") != 0) {
            fprintf(stderr, "Erro: comando desconhecido no commands.txt.\n");
            fclose(cmd_file); fclose(out_file);
            free(available); free(max); free(allocation); free(need); free(request);
            return 1;
        }

        if (customer_id < 0 || customer_id >= num_customers) {
            fprintf(stderr, "Erro: customer_id %d fora dos limites.\n", customer_id);
            fclose(cmd_file); fclose(out_file);
            free(available); free(max); free(allocation); free(need); free(request);
            return 1;
        }

        strtok(buffer, " \t\r\n"); 
        strtok(NULL, " \t\r\n"); 
        char *token = strtok(NULL, " \t\r\n"); 

        int j = 0;
        while (token != NULL && j < num_resources) {
            char *endptr;
            long val = strtol(token, &endptr, 10);

            if (endptr == token || *endptr != '\0' || val < 0) {
                fprintf(stderr, "Erro: dado nao numerico ou negativo em commands.txt.\n");
                fclose(cmd_file); fclose(out_file);
                free(available); free(max); free(allocation); free(need); free(request);
                return 1;
            }
            
            request[j] = (int)val;
            token = strtok(NULL, " \t\r\n");
            j++;
        }

        if (j != num_resources || token != NULL) {
            fprintf(stderr, "Erro: commands.txt exige uma quantidade de recursos diferente dos argumentos informados.\n");
            fclose(cmd_file); fclose(out_file);
            free(available); free(max); free(allocation); free(need); free(request);
            return 1;
        }

        if (strcmp(type, "RQ") == 0) {
            bool valid_need = true;
            for (int r = 0; r < num_resources; r++) {
                if (request[r] > need[customer_id * num_resources + r]) {
                    valid_need = false;
                    break;
                }
            }
            if (!valid_need) {
                fprintf(out_file, "The customer %d request ", customer_id);
                for (int r = 0; r < num_resources; r++) fprintf(out_file, "%d ", request[r]);
                fprintf(out_file, "was denied because exceed its maximum allocation\n");
                continue;
            }

            bool enough_avail = true;
            for (int r = 0; r < num_resources; r++) {
                if (request[r] > available[r]) {
                    enough_avail = false;
                    break;
                }
            }
            if (!enough_avail) {
                fprintf(out_file, "The resources ");
                for (int r = 0; r < num_resources; r++) fprintf(out_file, "%d ", available[r]);
                fprintf(out_file, "are not enough to customer %d request ", customer_id);
                for (int r = 0; r < num_resources; r++) fprintf(out_file, "%d ", request[r]);
                fprintf(out_file, "\n");
                continue;
            }

            for (int r = 0; r < num_resources; r++) {
                available[r]                          -= request[r];
                allocation[customer_id * num_resources + r] += request[r];
                need[customer_id * num_resources + r]       -= request[r];
            }

            if (is_safe_state(num_customers, num_resources, available, allocation, need)) {
                fprintf(out_file, "Allocate to customer %d the resources ", customer_id);
                for (int r = 0; r < num_resources; r++) {
                    fprintf(out_file, "%d", request[r]);
                    if (r < num_resources - 1) fprintf(out_file, " ");
                }
                fprintf(out_file, "\n");
            } else {
                for (int r = 0; r < num_resources; r++) {
                    available[r]                          += request[r];
                    allocation[customer_id * num_resources + r] -= request[r];
                    need[customer_id * num_resources + r]       += request[r];
                }
                fprintf(out_file, "The customer %d request ", customer_id);
                for (int r = 0; r < num_resources; r++) fprintf(out_file, "%d ", request[r]);
                fprintf(out_file, "was denied because result in an unsafe state\n");
            }

        } else if (strcmp(type, "RL") == 0) {
            
            bool valid_release = true;
            for (int r = 0; r < num_resources; r++) {
                if (request[r] > allocation[customer_id * num_resources + r]) {
                    valid_release = false;
                    break;
                }
            }
            
            if (!valid_release) {
                fprintf(out_file, "The customer %d request to release ", customer_id);
                for (int r = 0; r < num_resources; r++) fprintf(out_file, "%d ", request[r]);
                fprintf(out_file, "was denied because it exceeds allocated resources\n");
                continue;
            }

            for (int r = 0; r < num_resources; r++) {
                available[r]                          += request[r];
                allocation[customer_id * num_resources + r] -= request[r];
                need[customer_id * num_resources + r]       += request[r];
            }
            fprintf(out_file, "Release from customer %d the resources ", customer_id);
            for (int r = 0; r < num_resources; r++) {
                fprintf(out_file, "%d", request[r]);
                if (r < num_resources - 1) fprintf(out_file, " ");
            }
            fprintf(out_file, "\n");
        }
    }

    fclose(cmd_file);
    fclose(out_file);
    free(available); free(max); free(allocation); free(need); free(request);

    printf("Processamento concluido! Cheque o result.txt.\n");
    return 0;
}