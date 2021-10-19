#include "packer.h"
#include "semaphore.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// You can declare global variables here
sem_t sem1;
sem_t sem2;
sem_t sem3;
sem_t mutex;
sem_t mutex1;
typedef struct BALLINFO {
    int id;
    int colour;
    struct BALLINFO *next;
} ballinfo;
ballinfo *head;
sem_t *sem_list[4];
int *count_list[4];
int number_per_pack;

void packer_init(int balls_per_pack) {
    // Write initialization code here (called once at the start of the program).
    // It is guaranteed that balls_per_pack >= 2.
    sem_init(&mutex, 0, 1);
    sem_init(&mutex1, 0, 0);
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);
    sem_init(&sem3, 0, 0);
    sem_list[1] = &sem1;
    sem_list[2] = &sem2;
    sem_list[3] = &sem3;
    count_list[1] = &balls_per_pack;
    count_list[2] = &balls_per_pack;
    count_list[3] = &balls_per_pack;
    number_per_pack = balls_per_pack;
    head = NULL;
}

void packer_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    sem_destroy(&mutex);
    sem_destroy(&mutex1);
    sem_destroy(&sem1);
    sem_destroy(&sem2);
    sem_destroy(&sem3);
    ballinfo *current = head;
    while (current != NULL) {
        ballinfo *temp = current->next;
        free(current);
        current = temp;
    }
    head = NULL;
}

void pack_ball(int colour, int id, int *other_ids) {
    // Write your code here.
    // Remember to populate the array `other_ids` with the (balls_per_pack-1) other balls.
    sem_wait(&mutex);
    bool wait = true;
    ballinfo *ball = malloc(sizeof(ballinfo));
    ball->id = id;
    ball->colour = colour;
    count_list[colour] -= 1;
    // printf("ballid: %d", id);
    // printf("%d: count: %d\n", count_list[colour]);
    if (head == NULL) {
        head = ball;
    } else {
        ballinfo *temp;
        temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = ball;
    }
    ball->next = NULL;
    ballinfo *b = head;
    ballinfo *prev;
    if (count_list[colour] == 0) {
        int count = 0;
        wait = false;
        while (b != NULL) {
            if (b->colour == colour) {
                if (b->id != id) {
                    other_ids[count] = b->id;
                    count += 1;
                }
            }
            prev = b;
            b = b->next;
        }
        for (int i = 1; i < number_per_pack; i++) {
            sem_post(sem_list[colour]);
        }
        count_list[colour] += 1;
    }
    if (wait == false) {
        sem_wait(&mutex1);
    }
    sem_post(&mutex);
    if (wait) {
        sem_wait(sem_list[colour]);
    }
    if (*count_list[colour] < number_per_pack) {
        b = head;
        int count = 0;
        while (b != NULL) {
            if (b->colour == colour) {
                if (b->id != id) {
                    other_ids[count] = b->id;
                    count = count + 1;
                }
            }
            prev = b;
            b = b->next;
        }
        if (*count_list[colour] == number_per_pack - 1) {
            b = head;
            while (b != NULL) {
                if (b->colour == colour) {
                    if (b == head) {
                        head = b->next;
                    } else {
                        prev->next = b->next;
                    }
                }
                prev = b;
                b = b->next;
            }
            sem_post(&mutex1);
        }
        count_list[colour] += 1;
    }
}