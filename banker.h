#ifndef BANKER_H
#define BANKER_H

#include <stdbool.h>
#include <stdio.h>

bool is_safe_state(int num_customers, int num_resources, int *available, int *allocation, int *need);
void print_system_state(FILE *out, int num_customers, int num_resources, int *max, int *allocation, int *need, int *available);

#endif