#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "./globals.h"

// Tempo de espera em segundos.
float CONSUME_INTERVAL = 1, TIME_ON_BUFFER_EMPTY = 0.5;

typedef struct ConsumerArgs {
  int index;
  int qty;
} ConsumerArgs;

void *consumer_fn(void *args) {
  ConsumerArgs *data = (ConsumerArgs *) args;

  // Consome "qty" itens
  for (int i = 1; i <= data->qty; i++) {
    // Intervalo para consumir
    usleep(1000000 * CONSUME_INTERVAL);

    // Início da sessão crítica
    if (pthread_mutex_lock(&mutex) != 0) {
      perror("mutex lock");
      exit(1);
    }

    // Buffer vazio
    while (size == 0) {
      // Fim da sessão crítica
      if (pthread_mutex_unlock(&mutex) != 0) {
        perror("mutex unlock");
        exit(1);
      }

      // Espera para consumir, fora da sessão crítica
      usleep(1000000 * TIME_ON_BUFFER_EMPTY);

      // Início da sessão crítica, tenta consumir novamente
      if (pthread_mutex_lock(&mutex) != 0) {
        perror("mutex lock");
        exit(1);
      }
    }

    int consumed_value = buffer[start];
    start = (start+1)%buff_size;
    size--;

    printf("Consumidor %d consumiu %02d (total = %d)", data->index, consumed_value, i);

    if (show_buffer) {
      printf("\t");
      print_buffer();
    } else {
      printf("\n");
    }

    // Fim da sessão crítica
    if (pthread_mutex_unlock(&mutex) != 0) {
      perror("mutex unlock");
      exit(1);
    }
  }

  // Libera memória alocada para parâmetros
  free(data);

  // Encerra ao consumir "qty" itens
  pthread_exit(NULL);
}
