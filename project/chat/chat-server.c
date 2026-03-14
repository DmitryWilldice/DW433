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
int main() {
    int server_sock, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;

    signal(SIGINT, signal_handler);

    // Создание сокета
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("socket");
        exit(1);
    }

    // Настройка адреса
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Сервер запущен на порту %d. Подключайтесь: telnet localhost %d\n", PORT, PORT);

    while (server_running) {
        new_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (new_sock < 0) {
            if (!server_running) break;
            perror("accept");
            continue;
        }

        pthread_mutex_lock(&clients_mutex);
        if (num_clients < MAX_CLIENTS) {
            client_socks[num_clients++] = new_sock;
            int *new_sock_ptr = malloc(sizeof(int));
            *new_sock_ptr = new_sock;
            pthread_create(&thread_id, NULL, client_handler, new_sock_ptr);
            printf("Новый клиент подключён: %s:%d (всего %d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), num_clients);
        } else {
            close(new_sock);
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    close(server_sock);
    pthread_mutex_destroy(&clients_mutex);
    return 0;
}
