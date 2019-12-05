#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N_PARADAS  5   // numero de paradas de la ruta
#define EN_RUTA    0   // autobus en ruta
#define EN_PARADA  1   // autobus en la parada
#define MAX_USUARIOS 40  // capacidad del autobus
#define USUARIOS   4   // numero de usuarios

// estado inicial
int estado        = EN_RUTA;
int parada_actual = 0;     // parada en la que se encuentra el autobus
int n_ocupantes   = 0;     // ocupantes que tiene el autobus

// personas que desean subir en cada parada
int esperando_parada[N_PARADAS]; //= {0,0,0,0,0};

// personas que desean bajar en cada parada
int esperando_bajar[N_PARADAS]; //= {0,0,0,0,0};

// Otras definiciones globales (comunicacion y sincronizacion)
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;
sem_t multiplex[N_PARADAS]; //MAX_USUARIOS
sem_t mutex_bus[N_PARADAS]; //0
sem_t todosSubidos, todosBajados; //0

void * thread_autobus(void * args) {
	while (1) {
		// esperar a que los viajeros suban y bajen
		Autobus_En_Parada();
		// conducir hasta siguiente parada
		Conducir_Hasta_Siguiente_Parada();
	}
}

void * thread_usuario(void * arg) {
	int id_usuario;
	int a, b;
	// obtener el id del usario
	id_usuario = pthread_self();
	
	while (1) {
		a=rand() % N_PARADAS;
		do{
			b=rand() % N_PARADAS;
		} while(a==b);
		Usuario(id_usuario,a,b);
	}
}

void Usuario(int id_usuario, int origen, int destino) {
	printf("Usuario: %d origen: %d destino: %d", id_usuario, origen, destino);
	// Esperar a que el autobus este en parada origen para subir
	Subir_Autobus(id_usuario, origen);
	// Bajarme en estacion destino
	Bajar_Autobus(id_usuario, destino);
}

int main(int argc, char* argv[])
{
	int i;
	// Definicion de variables locales a main
	pthread_t bus;
	pthread_t usuarios[USUARIOS];
	unsigned int args;
	// Opcional: obtener de los argumentos del programa la capacidad del
	// autobus, el numero de usuarios y el numero de paradas
	printf("Inicializando");
	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);
	sem_init(&todosSubidos, 0, 0);
	sem_init(&todosBajados, 0, 0);
	
	for (i = 0; i < N_PARADAS; i++){
		printf("Inicializando %d", (i/N_PARADAS)*100);
		esperando_parada[i] = 0;
		esperando_bajar[i] = 0;
		sem_init(&mutex_bus[i], 0, 0);
		sem_init(&multiplex[i], 0, MAX_USUARIOS);
	}
	
	// Crear el thread Autobus
	pthread_create(&bus, NULL, thread_autobus, NULL);
	
	for ( i = 0; i < USUARIOS; i++){
		// Crear thread para el usuario i
		pthread_create(&usuarios[i], NULL, thread_usuario, NULL);
	}
	
	for ( i = 0; i < USUARIOS; i++){
		// Esperar terminacion de los hilos
		pthread_join(usuarios[i], NULL);
	}

	return 0;
}

void Autobus_En_Parada(){
	/*	Ajustar el estado y bloquear al autobus hasta que no haya pasajeros que
		quieran bajar y/o subir la parada actual. Despues se pone en marcha */
	estado = EN_PARADA;
	
	printf("Autobus llega a parada: %d", parada_actual);
	
	pthread_mutex_lock(&mutex1);
	if(esperando_parada[parada_actual] > 0){
		printf("Autobus espera en: %d", parada_actual);
		sem_post(&mutex_bus[parada_actual]);
		sem_wait(&todosSubidos);
	}
	pthread_mutex_unlock(&mutex1);
	
	printf("Autobus se va de %d", parada_actual);
	estado = EN_RUTA;
}

void Conducir_Hasta_Siguiente_Parada(){
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada*/
	printf("Autobus conduciendo");
	sleep(rand() % 5);
	parada_actual = (parada_actual + 1) % N_PARADAS;
}

void Subir_Autobus(int id_usuario, int origen){
	/* El usuario indicara que quiere subir en la parada ’origen’, esperara a que
		el autobus se pare en dicha parada y subira. El id_usuario puede utilizarse para
		proporcionar informacion de depuracion */
		
	sem_wait(&multiplex[origen]);
	pthread_mutex_lock(&mutex1);
	printf("Usuario %d llega a parada: %d", id_usuario, origen);
	esperando_parada[origen]++;
	pthread_mutex_unlock(&mutex1);
	
	printf("Usuario %d espera para subir en %d", id_usuario, origen);
	sem_wait(&mutex_bus[origen]);
	sem_post(&multiplex[origen]);
	
	printf("Usuario %d sube al bus", id_usuario);
	n_ocupantes++;
	esperando_parada[origen]--;
	if(n_ocupantes == MAX_USUARIOS || esperando_parada[origen] == 0){
		if(esperando_bajar[origen] > 0){
			sem_wait(&todosBajados);
		}
		sem_post(&todosSubidos);
	}
	else{
		sem_post(&mutex_bus[origen]);
	}
}

void Bajar_Autobus(int id_usuario, int destino){
	/* El usuario indicara que quiere bajar en la parada ’destino’, esperara a que
		el autobus se pare en dicha parada y bajara. El id_usuario puede utilizarse para
		proporcionar informacion de depuracion */
	
	sem_wait(&multiplex[destino]);
	pthread_mutex_lock(&mutex2);
	printf("Usuario %d quiere bajar en %d", id_usuario, destino);
	esperando_bajar[destino]++;
	pthread_mutex_unlock(&mutex2);
	
	printf("Usuario %d espera para bajar en %d", id_usuario, destino);
	sem_wait(&mutex_bus[destino]);
	sem_post(&multiplex[destino]);
	
	printf("Usuario %d baja en %d", id_usuario, destino);
	n_ocupantes--;
	esperando_bajar[destino]--;
	if(n_ocupantes == 0 || esperando_bajar[destino] == 0){
		sem_post(&todosBajados);
	}
	else{
		sem_post(&mutex_bus[destino]);
	}
	
}
