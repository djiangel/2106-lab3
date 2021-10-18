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
typedef struct BALLINFO {
    int id;
    int colour;
    struct BALLINFO *next;
} ballinfo;
ballinfo *head;
sem_t *sem_list[4];


void packer_init(void) {
    // Write initialization code here (called once at the start of the program).
    sem_init(&mutex, 0, 1);
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);
    sem_init(&sem3, 0, 0);
    sem_list[1] = &sem1;
    sem_list[2] = &sem2;
    sem_list[3] = &sem3;
    head = NULL;
}

void packer_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    sem_destroy(&mutex);
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

int pack_ball(int colour, int id) {
    // Write your code here.
    sem_wait(&mutex);
    ballinfo *ball = malloc(sizeof(ballinfo));
    ball->id = id;
    ball->colour = colour;
    bool wait = true;
    int partner_id = -1;
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
    while (b != NULL) {
        if (b->colour == colour) {
            if (b->id != id) {
                wait = false;
                partner_id = b->id;
            }
        }
        b = b->next;
    }
    sem_post(&mutex);
    if (wait) {
        sem_wait(sem_list[colour]);
    } else {
        sem_post(sem_list[colour]);
    }
    if (partner_id == -1) {
        b = head;
        while (b != NULL) {
            if (b->colour == colour) {
                if (b->id != id) {
                    wait = false;
                    partner_id = b->id;
                }
            }
        }
        b = b->next;
    }
    return partner_id;
}