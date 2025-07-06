#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "./produtor.c"
#include "./consumidor.c"

// Buffer e índices (recurso disputado)
int size = 0, start = 0, end = 0, buff_size = 0, show_buffer = 0;
int *buffer;

// Mutex
pthread_mutex_t mutex;

int main()
{
  int m = 0, n = 0;
  printf("M (produtores) = ");
  scanf("%d", &m);

  while (m < 1 || m > 100) {
    printf("Valor deve estar em [1, 100]. M (produtores) = ");
    scanf("%d", &m);
  }

  printf("N (consumidores) = ");
  scanf("%d", &n);

  while (n < 1 || n > 100) {
    printf("Valor deve estar em [1, 100]. N (consumidores) = ");
    scanf("%d", &n);
  }

  // Inicialização do buffer
  printf("Tamanho do buffer = ");
  scanf("%d", &buff_size);

  while (buff_size < 1 || buff_size > 10000) {
    printf("Valor deve estar em [1, 10_000]. Tamanho do buffer = ");
    scanf("%d", &buff_size);
  }

  buffer = malloc(sizeof(int) * buff_size);

  char print_char;
  printf("Printar buffer (recomendado para buffers pequenos) (y/n) = ");
  scanf(" %c", &print_char);

  if (print_char == 'y' || print_char == 'Y') {
    show_buffer = 1;
  }

  int prod_items[m], total_items = 0;
  for (int i = 0; i < m; i++)
  {
    int temp = 0;
    printf("Nº de itens do produtor %d = ", i + 1);
    scanf("%d", &temp);

    while (temp < 1 || temp > 10000) {
      printf("Valor deve estar em [1, 10_000]. Nº de itens do produtor %d = ", i + 1);
      scanf("%d", &temp);
    }

    prod_items[i] = temp;
    total_items += temp;
  }

  printf("\n");

  // Inicialização do mutex
  if (pthread_mutex_init(&mutex, NULL) == -1) {
    perror("mutex init error");
    exit(1);
  }

  // Criação das threads produtoras
  pthread_t producers[m];
  for (int i = 0; i < m; i++)
  {
    ProducerArgs *data = malloc(sizeof(ProducerArgs));
    data->index = i+1;
    data->qty = prod_items[i];

    pthread_create(&producers[i], NULL, producer_fn, (void *)data);
  }

  // Criação das threads consumidoras
  pthread_t consumers[n];
  int itens_per_consumer = total_items / n;
  for (int i = 0; i < n; i++)
  {
    ConsumerArgs *data = malloc(sizeof(ConsumerArgs));
    data->index = i+1;
    data->qty = itens_per_consumer;

    pthread_create(&consumers[i], NULL, consumer_fn, (void *)data);
  }

  // Sincronização
  // for (int i = 0; i < m; i++) pthread_join(producers[i], NULL);
  for (int i = 0; i < n; i++) pthread_join(consumers[i], NULL);

  // Destruição do mutex
  if (pthread_mutex_destroy(&mutex) != 0) {
    perror("mutex destroy error");
    exit(1);
  }

  // Sem a exclusão mútua (mutex) os produtores sobrescrevem alguns índices e o total de itens produzidos/consumidos fica inconsistente.

  // É possível demonstrar a consistência do acesso ao buffer passando um somatório de itens não divisível pelo número de consumidores,
  // o estado final do buffer não será vazio.
}

void print_buffer() {
    printf("Buffer: [");

    for (int i = 0; i < buff_size; i++) {
	if ((start + size) / buff_size != 0) {
		if (i >= start || i < end) {
			printf("%02d", buffer[i]);
		} else {
			printf("__");
		}
	} else {
		if (i >= start && i < end) {
			printf("%02d", buffer[i]);
		} else {
			printf("__");
		}
	}

      if (i < buff_size-1) printf(", ");
    }

    printf("]\tLimits = [%d, %d[\n", start, end);
}
