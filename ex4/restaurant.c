#include "restaurant.h"
#include "semaphore.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// You can declare global variables here
int number_of_tables[5];
typedef struct TABLEINFO {
    bool occupied;
    int id;
} tableinfo;
tableinfo** tables;
group_state *head;
sem_t mutex;
sem_t sem1;
sem_t sem2;
sem_t sem3;
sem_t sem4;
sem_t sem5;
sem_t *sem_list[5];


void restaurant_init(int num_tables[5]) {
    // Write initialization code here (called once at the start of the program).
    // It is guaranteed that num_tables is an array of length 5.
    // TODO
    int n = 5;
    tables = (tableinfo **)malloc(n * sizeof(tableinfo *));
    for (int r = 0; r < n; r++) {
		tables[r] = (tableinfo *)malloc(num_tables[r] * sizeof(tableinfo));
    }
    int count = 0;
    for (int i = 0; i < n; i++) {
        for (int a = 0; a < num_tables[a]; a++) {
            tableinfo *table= malloc(sizeof(tableinfo));
            table->occupied = false;
            table->id = count;
            tables[i][a] = *table;
            count += 1;
        }
    }
    for (int i = 0; i < 5; i++) {
        number_of_tables[i] = num_tables[i];
    }
    sem_init(&mutex, 0, 1);
    sem_init(&sem1, 0, number_of_tables[0]);
    sem_init(&sem2, 0, number_of_tables[1]);
    sem_init(&sem3, 0, number_of_tables[2]);
    sem_init(&sem4, 0, number_of_tables[3]);
    sem_init(&sem5, 0, number_of_tables[4]);
    sem_list[0] = &sem1;
    sem_list[1] = &sem2;
    sem_list[2] = &sem3;
    sem_list[3] = &sem4;
    sem_list[4] = &sem5;
    head = NULL;
}

void restaurant_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    // TODO
}

int request_for_table(group_state *state, int num_people) {
    // Write your code here.
    // Return the id of the table you want this group to sit at.
    // TODO
    state->people = num_people;
    state->table_id = -1;
    state->next = NULL;
    sem_init(&state->sem, 0, 0);
    int index = num_people-1;
    sem_wait(&mutex);
    bool wait = false;
    on_enqueue();
    if (number_of_tables[index] == 0) {
        wait = true;
        if (head == NULL) {
            head = state;
        } else {
            group_state *temp;
            temp = head;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = state;
        }
    }
    sem_post(&mutex);
    if (wait) {
        sem_wait(&state->sem);
    }
    sem_wait(&mutex);
    if (wait) {
        group_state *g;
        group_state *prev;
        g = head;
        while (g != NULL) {
            if (g->people == num_people) {
                if (g == head) {
                    head = g->next;
                } else {
                    prev->next = g->next;
                }
                break;
            }
            prev = g;
            g = g->next;
        }
    }
    int tableid_assigned;
    for (int i = 0; i < number_of_tables[index]; i++) {
        tableinfo *table = &tables[index][i];
        if (table->occupied == false) {
            state->table_id = table->id;
            tableid_assigned = table->id;
            table->occupied = true;
            break;
        }
    }
    sem_post(&mutex);
    return tableid_assigned;
}

void leave_table(group_state *state) {
    // Write your code here.
    // TODO
    sem_wait(&mutex);
    int table_id = state->table_id;
    int index = state->people - 1;
    for (int i = 0; i < number_of_tables[index]; i++) {
        tableinfo *table = &tables[index][i];
        if (table->id == table_id) {
            table->occupied = false;
        }
    }
    int people = state->people;
    group_state *g;
    g = head;
    while (g != NULL) {
        if (g->people == people) {
            sem_post(&g->sem);
            break;
        }
    }
    sem_post(&mutex);
}