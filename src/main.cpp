/**
 * Gabriel Nunes Rodrigues Fonseca
 * 16/0006597
 * Universidade de Brasília - UnB
 * Trabalho da disciplina Programação concorrente
 **/

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <bits/stdc++.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

#define THINKING(x) (((3) / (x)) + 3)
#define DIFFICULTY ((rand() % 5) + 3)
#define SOLVING(x,y) ((x)/(y))
#define SUBMITTING 1
#define JUDGING 1
#define ACCEPTED (rand() % 4)

#define NUM_TEAMS 5
#define NUM_STUDENTS 3
#define NUM_PROBLEMS 5

int vencedor;
int tem_vencedor = 0;
int teams_submissions[NUM_TEAMS];
std::queue<int> submissions;

pthread_mutex_t cond_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t empty_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t solution = PTHREAD_COND_INITIALIZER;

struct Student {
    int student_id;
    int team_id;
    float inteligence;
    pthread_mutex_t& computer;

    Student(int sid, int tid, float iq, pthread_mutex_t& c): student_id(sid), team_id(tid), inteligence(iq), computer(c) {}
};

void judging(int team) {
    printf(BLU "JUIZ ESTÁ JULGANDO SUBMISSÃO DO TIME %d!\n" RESET, team);
    if(ACCEPTED) {
        printf(GRN "AC PARA O TIME %d!\n" RESET, team);
        teams_submissions[team]++;
        if(teams_submissions[team] == NUM_PROBLEMS) {
            vencedor = team;
            tem_vencedor = 1;
        }
    }
    else {
        printf(YEL "WA PARA O TIME %d!\n" RESET, team);
    }
}

void thinking(Student * st) {
    printf("T%dE%d está pensando em uma questão.\n", st->team_id, st->student_id);
    sleep(THINKING(st->inteligence));
    printf("T%dE%d teve uma ideia e quer o computador.\n" , st->team_id, st->student_id);
}

void implementing(Student * st) {
    float diff = DIFFICULTY;
    printf("T%dE%d está implementado e levará %.2f\n", st->team_id, st->student_id, diff/st->inteligence);
    sleep(SOLVING(diff,st->inteligence));
    printf("T%dE%d terminou de implementar\n", st->team_id, st->student_id);
}

void submitting(Student * st) {
    printf("T%dE%d está submetendo a solução ao juiz.\n", st->team_id, st->student_id);
    pthread_mutex_lock(&cond_lock);
    submissions.push(st->team_id);
    sleep(SUBMITTING);

    if(submissions.size() >= 1) {
        pthread_cond_signal(&solution);
    }
    pthread_mutex_unlock(&cond_lock);
}

void coffe_break(Student * st) {
    printf("T%dE%d saiu para o coffe break\n", st->team_id, st->student_id);
    sleep(2);
}

void * student_thread(void * arguments) {
    auto st = (Student *)(arguments);


    while(!tem_vencedor) {

        thinking(st);
        
        pthread_mutex_lock(&st->computer);
        implementing(st);
        pthread_mutex_unlock(&st->computer);
        
        pthread_mutex_lock(&empty_lock);
        if(submissions.size() >= 10) {
            pthread_cond_signal(&solution);
            pthread_cond_wait(&empty, &empty_lock);
        }
        pthread_mutex_unlock(&empty_lock);


        pthread_mutex_lock(&queue);
        submitting(st);
        pthread_mutex_unlock(&queue);

        coffe_break(st);
    }
    pthread_exit(0);
}

void * judge_thread(void * id) {

    while(true) {

        pthread_mutex_lock(&cond_lock);
        if(submissions.size() == 0) {
            printf(MAG "NÃO HÁ SUBMISSÕES AGUARDANDO JULGAMENTO!\n" RESET);
            pthread_cond_broadcast(&empty);
            pthread_cond_wait(&solution, &cond_lock);
            printf(CYN "JUIZ ESTÁ LIGADO E PRONTO PARA JULGAR!\n" RESET);
        }
        pthread_mutex_unlock(&cond_lock);

        pthread_mutex_lock(&queue);
        int team = submissions.front();
        submissions.pop();
        pthread_mutex_unlock(&queue);

        judging(team);
    }

    pthread_cond_broadcast(&empty);
    printf("SAI DESSA PORRA\n");
    pthread_exit(0);
}

void * team_thread(void * id) {
    Student * del[NUM_STUDENTS];
    pthread_t students[NUM_STUDENTS];
    pthread_mutex_t computador = PTHREAD_MUTEX_INITIALIZER;

    
    for(int i = 0; i < NUM_STUDENTS; i++) {
        auto st = new Student(i,*(int*)id,(random() % 3)+1, computador);
        del[i] = st;
        pthread_create(&students[i], NULL, student_thread, (void *)(st));
    }

    for(int i = 0; i < NUM_STUDENTS; i++) {
        pthread_join(students[i], NULL);
    }
    for(int i = 0; i < NUM_STUDENTS; i++) {
        delete del[i];
    }
    pthread_exit(0);
}

int main() {
    srand(time(NULL));
    memset(teams_submissions,0,NUM_TEAMS);

    pthread_t teams[NUM_TEAMS];
    pthread_t judge;

    int * j = (int *)malloc(sizeof(int));

    pthread_create(&judge, NULL, judge_thread, (void *)j);

    for (int i = 0; i < NUM_TEAMS; i++) {
        int * id = (int *)malloc(sizeof(int));
        *id = i;
		pthread_create(&teams[i], NULL, team_thread, (void *)(id));
	}

    for(int i = 0; i < NUM_TEAMS; i++) {
        pthread_join(teams[i], NULL);
    }

    free(j);
    printf(YEL "TIME %d FOI VENCEDOR DA MARATONA DE PROGRAMAÇÃO!\n" RESET, vencedor);
}
