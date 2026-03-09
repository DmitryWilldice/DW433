#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

int client_socks[MAX_CLIENTS];
int num_clients = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
volatile int server_running = 1;

void signal_handler(int sig) {
    server_running = 0;
    printf("\nЗавершение сервера...\n");
}

void *client_handler(void *arg) {
    int sock = *(int*)arg;
    free(arg);
    char buffer[BUFFER_SIZE];
    int leave = 0;

    while (server_running && !leave) {
        int read_size = read(sock, buffer, BUFFER_SIZE - 1);
        if (read_size <= 0) break;
        buffer[read_size] = '\0';

        if (strcmp(buffer, "exit\n") == 0) {
            leave = 1;
        } else {
            printf("Чаттер %d: %s", sock, buffer);
            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < num_clients; i++) {
                if (client_socks[i] != sock && client_socks[i] > 0) {
                    write(client_socks[i], buffer, read_size);
                }
            }
            pthread_mutex_unlock(&clients_mutex);
        }
    }

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < num_clients; i++) {
        if (client_socks[i] == sock) {
            client_socks[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    printf("Чаттер %d отключился\n", sock);
    close(sock);
    return NULL;
}
int main(){


    return 0;
}