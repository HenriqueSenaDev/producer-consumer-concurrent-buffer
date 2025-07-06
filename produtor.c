#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "./globals.h"

// Tempos de espera segundos.
float PRODUCER_INTERVAL = 1, TIME_ON_BUFFER_FULL = 0.5;

typedef struct ProducerArgs {
  int index;
  int qty;
} ProducerArgs;

void *producer_fn(void *args) {
  ProducerArgs *data = (ProducerArgs *) args;

  // Produz "qty" itens
  for (int i = 1; i <= data->qty; i++) {
    // Intervalo para produzir
    usleep(1000000 * PRODUCER_INTERVAL);
    // Número aleatório [0, 99]
    int generated_value = rand() % 100;

    // Início da sessão crítica
    if (pthread_mutex_lock(&mutex) != 0) {
      perror("mutex lock");
      exit(1);
    }

    // Buffer cheio
    while (size == buff_size) {
      // Fim da sessão crítica
      if (pthread_mutex_unlock(&mutex) != 0) {
        perror("mutex unlock");
        exit(1);
      }

      // Espera para produzir, fora da sessão crítica
      usleep(1000000 * TIME_ON_BUFFER_FULL);

      // Início da sessão críticam, tenta novamente
      if (pthread_mutex_lock(&mutex) != 0) {
        perror("mutex lock");
        exit(1);
      }
    }

    buffer[end] = generated_value;
    end = (end+1)%buff_size;
    size++;

    printf("Produtor %d produziu %02d (total = %d)", data->index, generated_value, i);

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

  // Encerra ao produzir "qty" itens
  pthread_exit(NULL);
}
