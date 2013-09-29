#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_COLOR_RED     "\033[22;31m"
#define ANSI_COLOR_GREEN   "\033[22;32m"
#define ANSI_COLOR_YELLOW  "\033[01;33m"
#define ANSI_COLOR_BLUE    "\033[22;34m"
#define ANSI_COLOR_MAGENTA "\033[22;35m"
#define ANSI_COLOR_CYAN    "\033[22;36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define NO_OF_CUSTOMERS 8

pthread_t customers[NO_OF_CUSTOMERS];

int eating = 0, waiting = 0;
int no_of_chairs, no_of_customers;

sem_t block;
sem_t mutex;
int must_wait = 0;

/* exibição de estados dos clientes */
typedef enum {W, S, E, L} estado_t;
estado_t estado[NO_OF_CUSTOMERS];
sem_t image_block;

void exibe_mesa() {
  int i;
       
        system("clear");
printf("\t        漢"ANSI_COLOR_RED"o"ANSI_COLOR_RESET"字         漢"ANSI_COLOR_RED"o"ANSI_COLOR_RESET"字          漢"ANSI_COLOR_RED"o"ANSI_COLOR_RESET"字         \n");                                            
printf("\t |‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|\n");             
printf("\t |        "ANSI_COLOR_YELLOW"_______"ANSI_COLOR_RESET"            "ANSI_COLOR_YELLOW"_______"ANSI_COLOR_RESET"           |\n");   
printf("\t |     "ANSI_COLOR_YELLOW"-<|"ANSI_COLOR_GREEN"@@@@@@@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_GREEN"@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_RESET"      "ANSI_COLOR_YELLOW"-<|"ANSI_COLOR_GREEN"@@@@@@@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_GREEN"@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_RESET"        |\n");
printf("\t |     "ANSI_COLOR_YELLOW"-<|"ANSI_COLOR_GREEN"@@@@@@@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_GREEN"@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_RESET"      "ANSI_COLOR_YELLOW"-<|"ANSI_COLOR_GREEN"@@@@@@@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_GREEN"@"ANSI_COLOR_YELLOW"|"ANSI_COLOR_RESET"        |\n");   
printf("\t |        "ANSI_COLOR_YELLOW"‾‾‾‾‾‾‾"ANSI_COLOR_RESET"            "ANSI_COLOR_YELLOW"‾‾‾‾‾‾‾"ANSI_COLOR_RESET"           |\n");   
printf("\t |_____________________________________________|\n");          

  for (i = 0; i < NO_OF_CUSTOMERS; i++)
    switch (estado[i]) {
    /* case W:  printf("W ");
      break; */
    case S:  printf(ANSI_COLOR_YELLOW "S " ANSI_COLOR_RESET);
      break;
    case E:  printf(ANSI_COLOR_CYAN "E " ANSI_COLOR_RESET);
      break;
    }
  printf("\n");
 
}

void* sushi_bar(void* arg) { 
  int client_id = *(int *) arg;
  
  while(1){

	int i,n;

	sem_wait(&mutex);

	if(must_wait) { 
		waiting+=1;
		
		sem_post(&mutex);
		printf("waiting customer...%d\n", client_id);
		estado[client_id] = W;
		
		sem_wait(&image_block);
		exibe_mesa();
		sem_post(&image_block);
		
		sem_wait(&block);
		waiting -= 1;
	} 
	
	estado[client_id] = S;
	printf("-----Sitting customer...%d\n", client_id);
		sem_wait(&image_block);
		exibe_mesa();
		sem_post(&image_block);
	
	eating+=1;
	must_wait = (eating == no_of_chairs);
	
	if(eating == no_of_chairs) {
	  printf("TABLE FULL WITH CUSTOMERS...\n");
	}
	
	if(waiting && !must_wait) 
	  sem_post(&block);
	else
	  sem_post(&mutex);

	estado[client_id] = E;
	printf("Eating customer...%d\n", client_id);
		sem_wait(&image_block);
		exibe_mesa();
		sem_post(&image_block);

	sleep(3);

	sem_wait(&mutex);
	eating-=1;
	estado[client_id] = L;
	printf("Leaving customer...%d\n", client_id);
	
	if(eating == 0)
	  must_wait = 0;
	
	if(waiting && !must_wait)
	  sem_post(&block);
	else
	 sem_post(&mutex);

	sleep(3);
  } 
} 

void main() { 

  int i=0;
  
  char c;

  int customer_id[NO_OF_CUSTOMERS];
  
  printf("Insira o numero de cadeiras do Sushi Bar: ");
  scanf(" %d", &no_of_chairs);


  for(i=0;i<NO_OF_CUSTOMERS;i++)
	  customer_id[i]=i+1;

  //mutex=make_semaphore(1);

  sem_init(&mutex,0,1);

  //block=make_semaphore(0);

  sem_init(&block,0,0);
  
  sem_init(&image_block,0,1);

  for(i=0;i<NO_OF_CUSTOMERS;i++)
	  pthread_create(&customers[i],0,sushi_bar,&customer_id[i]);
  
  /* use system call to make terminal send all keystrokes directly to stdin */
//   system ("/bin/stty raw");
  while(c != 'q') {
	c = getchar();
  };
  /* use system call to set terminal behaviour to more normal behaviour */
//   system ("/bin/stty cooked");


} 