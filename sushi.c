/*SUSHI BAR
**
** Andre Nogueira Brandao - RA116130
** Andre Seiji Tamanaha - RA116134
**
*/

#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ANSI_COLOR_RED     "\033[22;31m"
#define ANSI_COLOR_GREEN   "\033[22;32m"
#define ANSI_COLOR_YELLOW  "\033[01;33m"
#define ANSI_COLOR_BLUE    "\033[22;34m"
#define ANSI_COLOR_MAGENTA "\033[22;35m"
#define ANSI_COLOR_CYAN    "\033[22;36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define NO_OF_CUSTOMERS 20
#define TEMPO 30000
#define TAM_MESA 45

void todos_saem_sushibar(int client_id);
void remove_cliente(int client_id);
void zera_posicoes(); /*zera vetor de posicao x dos clientes/sushis*/
void exibe_mesa(int client_id);
void entra_sushibar(int client_id);

pthread_t customers[NO_OF_CUSTOMERS];

int eating = 0, waiting = 0, sentando = 0, leaving = 0, all_leaving = 0; /*flags de estado*/
int no_of_chairs, no_of_customers, n_espacos;

sem_t block;
pthread_mutex_t mutex;
int must_wait = 0;

/* exibição de estados dos clientes */
/* Waiting, Sitting, Eating, Leaving, Out */
typedef enum {W, S, E, L, O} estado_t;
estado_t estado[NO_OF_CUSTOMERS];

int posicao[NO_OF_CUSTOMERS]; /*vetor de posicao x dos clientes/sushis*/


void* sushi_bar(void* arg) { 
	int client_id = *(int *) arg;

	while(1){
		int i, n;

		pthread_mutex_lock(&mutex);

		if(must_wait) { 
			waiting+=1;

			/* CLIENTE ESPERANDO */
			estado[client_id] = W;
			exibe_mesa(client_id);

			/* MUTEX LIBERADO */
			pthread_mutex_unlock(&mutex);

			sem_wait(&block);
			waiting -= 1;
		} 
		eating+=1;
		must_wait = (eating == no_of_chairs);

		/* muda o estado do cliente para SITTING */
		estado[client_id] = S;
		entra_sushibar(client_id);

		/* Muda estado do cliente para COMENDO */
		estado[client_id] = E;
		exibe_mesa(client_id);


		/* MUTEX LIBERADO */
		if(waiting && !must_wait) 
			sem_post(&block);
		else
			pthread_mutex_unlock(&mutex);

		/* Sleep randomico para cada thread comer */
		sleep(rand() % 10 + 1);

		pthread_mutex_lock(&mutex);
		eating-=1;
		estado[client_id] = L;

		/* CLIENTE SAINDO */
		exibe_mesa(client_id);
		leaving++;
		remove_cliente(client_id);

		if(eating == 0)
			must_wait = 0;

		/* Muda o estado do cliente para OUT (saiu do Sushi Bar) */
		estado[client_id] = O;
		if(waiting && !must_wait)
			sem_post(&block);
		else
			pthread_mutex_unlock(&mutex);

		/* cliente espera 3s fora do bar */
		sleep(3);
	} 
} 

int main() { 
	int i=0;
	char c;
	int customer_id[NO_OF_CUSTOMERS];

	zera_posicoes();
	srand ( time(NULL) );

	printf("Insira o numero de cadeiras do Sushi Bar (1 a 20): ");
	scanf(" %d", &no_of_chairs);

	while(no_of_chairs < 1 || no_of_chairs > 20) {
		printf("Insira um valor válido para o numero de cadeiras do Sushi Bar (1 a 20): ");
		scanf(" %d", &no_of_chairs);
	}

	/* calcula o numero de espacos entre cada cliente */
	n_espacos = TAM_MESA/(no_of_chairs+1)-1; 

	/* inicia o id dos clientes e os estados como WAITING */
	for(i=0;i<NO_OF_CUSTOMERS;i++) {
		customer_id[i]=i;
		estado[i] = W;
	}

  	/* inicia o mutex */
	pthread_mutex_init(&mutex,0);

  	/* inicia o semaphore block */
	sem_init(&block,0,0);

	/* cria as Threads */
	for(i=0;i<NO_OF_CUSTOMERS;i++) {
		pthread_create(&customers[i],0,sushi_bar,&customer_id[i]);
	}

	/* tentativa de sair do programa com a tecla 'q' - ainda nao sucedida */
	while(c != 'q') {
		c = getchar();
	};

	return 0;
} 

/*funcao para clientes sairem do sushibar juntos*/
void todos_saem_sushibar(int client_id) {
	int i, j, k, p, encontrado=0;

	for(k=0; k < (leaving*(n_espacos+1) + 9); k++) {
		usleep(TEMPO);
		exibe_mesa(client_id);

		for(p=k; p<10; p++) {
			printf(" ");
		}

		for(i=1; i<=TAM_MESA; i++){
			for(j=0; j<NO_OF_CUSTOMERS; j++) {
				if((posicao[j]-10) == i) {
					printf("S");
					encontrado = 1;
					if(k>=10) {
						posicao[j]--; 	/* decrementa a posicao do cliente */
					}
				}
			}
			if(!encontrado) {
				printf(" ");
			}
			encontrado=0;
		}
		printf("\n");
	}
}

void remove_cliente(int client_id) {
	int i, n_clientes = 0;

	for(i=0; i<NO_OF_CUSTOMERS; i++) {
		if(posicao[i] != 0)
			n_clientes++;
	}

	if(n_clientes != no_of_chairs) {
		for(i=0; i<NO_OF_CUSTOMERS; i++) {
			if(estado[i] == L) {
				posicao[i] = 0;
				leaving--;
			}
		}
	}
	else {
		if(leaving == no_of_chairs) {
			all_leaving = 1;
			todos_saem_sushibar(client_id);
			all_leaving = 0;
			leaving = 0;
			zera_posicoes();
		}
	}
}

void zera_posicoes() {
	int i;

	for(i=0; i<NO_OF_CUSTOMERS; i++) {
		posicao[i] = 0;
	}
}

void exibe_mesa(int client_id) {
	int i, j, encontrado = 0, sentando = 0, comendo = 0, saindo = 0;
	system("clear");
	/*imprime mesa do sushibar*/
	printf("\n\n");
	printf("                          ___________\n");
	printf("                         | SUSHI BAR |\n"); 
	printf("                          ‾‾‾‾‾‾‾‾‾‾‾\n");                                        
	printf("\n");
	printf("                漢"ANSI_COLOR_RED"o"ANSI_COLOR_RESET"字         漢"ANSI_COLOR_RED"o"ANSI_COLOR_RESET"字          漢"ANSI_COLOR_RED"o"ANSI_COLOR_RESET"字         \n");                                            
	printf("         |‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|");

	printf("   CLIENTES ESPERANDO\n");

	printf("         |        "ANSI_COLOR_YELLOW"_______"ANSI_COLOR_RESET"            "ANSI_COLOR_YELLOW"_______"ANSI_COLOR_RESET"           |");   
	
	printf("        |‾‾‾‾‾‾|\n");

	printf("         |     "ANSI_COLOR_YELLOW"-<|"ANSI_COLOR_GREEN"@@@@@@@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_GREEN"@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_RESET"      "ANSI_COLOR_YELLOW"-<|"ANSI_COLOR_GREEN"@@@@@@@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_GREEN"@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_RESET"        |");

	printf("        |  %d  ", waiting);
	if(waiting < 10)
		printf(" ");
	printf("|\n");

	printf("         |     "ANSI_COLOR_YELLOW"-<|"ANSI_COLOR_GREEN"@@@@@@@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_GREEN"@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_RESET"      "ANSI_COLOR_YELLOW"-<|"ANSI_COLOR_GREEN"@@@@@@@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_GREEN"@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_RESET"        |");  

	printf("        |______|\n", waiting);

	printf("         |        "ANSI_COLOR_YELLOW"‾‾‾‾‾‾‾"ANSI_COLOR_RESET"            "ANSI_COLOR_YELLOW"‾‾‾‾‾‾‾"ANSI_COLOR_RESET"           |\n");   

	/* checa quantidade comendo */
	for(j=0; j<NO_OF_CUSTOMERS; j++) {
		if(estado[j]==E)
			comendo += 1;
	}

	/* checa se alguem esta sentando */
	for(j=0; j<NO_OF_CUSTOMERS; j++) {
		if(estado[j]==S)
			sentando = 1;
	}

	/* checa se alguem esta saindo */
	for(j=0; j<NO_OF_CUSTOMERS; j++) {
		if(estado[j]==L)
			saindo = 1;
	}

	printf("         |");
	
	/*imprime sushi caso cliente esteja comendo*/
	for(i=1; i<=45; i++){
		for(j=0; j<NO_OF_CUSTOMERS; j++) {
			if(estado[j]==E && (posicao[j]-10) == i) {
				printf(ANSI_COLOR_GREEN"@"ANSI_COLOR_RESET);
				encontrado = 1;
			}
		}
		if(!encontrado)
			printf("_");
		encontrado=0;
	}

	printf("|");

	/*imprime estados da execucao do programa*/
	if(comendo == no_of_chairs) {
		printf("    :: MESA CHEIA ::\n");
	}
	else if(all_leaving) {
		printf("  <== CLIENTES SAINDO\n");	
	}
	else if(sentando) {
		printf("  [CLIENTES ENTRANDO]\n");	
	}
	else
		printf("\n");

	/*imprime clientes que estao comendo*/
	if(!sentando && !saindo) {
		printf("          ");
		for(i=1; i<=45; i++){
			for(j=0; j<NO_OF_CUSTOMERS; j++) {
				if((posicao[j]-10) == i) {
					printf("C");
					encontrado = 1;
				}
			}
			if(!encontrado)
				printf(" ");
			encontrado=0;
		}
		printf("\n");
	}

	if(saindo && !all_leaving) {
		printf("          ");
		for(i=1; i<=45; i++){
			for(j=0; j<NO_OF_CUSTOMERS; j++) {
				if((posicao[j]-10) == i) {
					printf("C");
					encontrado = 1;
				}
			}
			if(!encontrado)
				printf(" ");
			encontrado=0;
		}
		printf("\n");
	}

	if(comendo == no_of_chairs) {
		usleep(100000);
	}

}

/*imprime clientes entrando*/
void entra_sushibar(int client_id) {
	int i, j, k;      

	if(eating == 1) {
		/* posicao final do cliente (quanto maior, mais pra esquerda anda) */
		for(i=0; i<61-n_espacos; i++) {
			usleep(TEMPO);
			exibe_mesa(client_id); 

			/* posicao inicial de entrada (quanto maior, mais longe sai da fila) */
			for(j=i; j<70; j++) {
				printf(" ");
			}

			printf("C\n");
		}
		posicao[client_id] = j-i+1;
	}
	else {
		/* posicao final do cliente (quanto maior, mais pra esquerda anda) */
		for(i=0; i<(71-n_espacos*eating-n_espacos); i++) {
			usleep(TEMPO);
			exibe_mesa(client_id); 

			/* posicao inicial do primeiro */
			for(j=(61-n_espacos); j<70; j++) {
				printf(" ");
			}
			printf("C");

			for(j=2; j<eating; j++) {
				for(k=0; k<n_espacos; k++) {
					printf(" ");
				}
				printf("C");
			}
			/* posicao inicial de entrada (quanto maior, mais longe sai da fila) */
			for(j=i; j<(70-eating*n_espacos); j++) {
				printf(" ");
			}

			printf("C\n");
		}
		posicao[client_id] = eating*(n_espacos+1) + 9;
	}
}