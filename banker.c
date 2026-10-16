#include "banker.h"
#include <stdlib.h>

bool is_safe_state(int num_customers, int num_resources, int *available, int *allocation, int *need) {
    int *work = (int *)malloc(num_resources * sizeof(int));
    if (!work) return false;

    // Deep Copy do available
    for (int i = 0; i < num_resources; i++) {
        work[i] = available[i];
    }
    
    bool *finish = (bool *)calloc(num_customers, sizeof(bool));
    if (!finish) {
        free(work);
        return false;
    }

    int count = 0;
    while (count < num_customers) {
        bool found = false;
        for (int i = 0; i < num_customers; i++) {
            if (finish[i] == false) {
                bool can_finish = true;
                for (int j = 0; j < num_resources; j++) {
                    if (need[i * num_resources + j] > work[j]) {
                        can_finish = false;
                        break;
                    }
                }
                
                if (can_finish) {
                    for (int j = 0; j < num_resources; j++) {
                        work[j] += allocation[i * num_resources + j];
                    }
                    finish[i] = true;
                    found = true;
                    count++;
                }
            }
        }
        if (found == false) {
            free(work);
            free(finish);
            return false;
        }
    }
    
    free(work);
    free(finish);
    return true;
}

void print_system_state(FILE *out, int num_customers, int num_resources, int *max, int *allocation, int *need, int *available) {
    fprintf(out, "MAXIMUM   |   ALLOCATION  |   NEED\n");
    
    for (int i = 0; i < num_customers; i++) {
        for (int j = 0; j < num_resources; j++) fprintf(out, "%d ", max[i * num_resources + j]);
        fprintf(out, "    |   "); 
        for (int j = 0; j < num_resources; j++) fprintf(out, "%d ", allocation[i * num_resources + j]);
        fprintf(out, "      |   "); 
        for (int j = 0; j < num_resources; j++) fprintf(out, "%d ", need[i * num_resources + j]);
        fprintf(out, "\n");
    }
    
    fprintf(out, "AVAILABLE ");
    for (int j = 0; j < num_resources; j++) fprintf(out, "%d ", available[j]);
    fprintf(out, "\n");
}