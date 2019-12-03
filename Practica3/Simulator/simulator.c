#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
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
int esperando_parada[N_PARADAS] //= {0,0,0,0,0};

// personas que desean bajar en cada parada
int esperando_bajar[N_PARADAS] //= {0,0,0,0,0};

// Otras definiciones globales (comunicacion y sincronizacion)
pthread_mutex_t esperar_bajada;
pthread_mutex_t parada[N_PARADAS];
pthread_cond_t enRuta;
//pthread_mutex_t busEnParada[N_PARADAS];

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
	pthread_mutex_init(&esperar_parada, NULL);
	pthread_mutex_init(&esperar_bajada, NULL);
	
	for (i = 0; i < N_PARADAS; i++){
		esperando_parada[i] = 0;
		esperando_bajar[i] = 0;
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
	
	while(esperando_parada[parada_actual]){
		
	}
	
	estado = EN_RUTA;
}

void Conducir_Hasta_Siguiente_Parada(){
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada*/
	sleep(rand() % 5);
	parada_actual = (parada_actual + 1) % N_PARADAS;
}

void Subir_Autobus(int id_usuario, int origen){
	/* El usuario indicara que quiere subir en la parada ’origen’, esperara a que
		el autobus se pare en dicha parada y subira. El id_usuario puede utilizarse para
		proporcionar informacion de depuracion */
	pthread_mutex_lock(&parada[origen]);
	esperando_parada[origen]++;
	pthread_mutex_unlock(&parada[origen]);
	
	//wait
	pthread_mutex_lock(&parada[origen]);
	while(estado == EN_RUTA){
		pthread_cond_wait(&enRuta, &parada[origen]);
	}
	pthread_mutex_unlock(&parada[origen]);

	pthread_mutex_lock(&esperar_parada);
	esperando_parada[origen]--;
	pthread_mutex_unlock(&esperar_parada);
}

void Bajar_Autobus(int id_usuario, int destino){
	/* El usuario indicara que quiere bajar en la parada ’destino’, esperara a que
		el autobus se pare en dicha parada y bajara. El id_usuario puede utilizarse para
		proporcionar informacion de depuracion */
	pthread_mutex_lock(&esperar_bajada);
	esperando_bajar[destino]++;
	pthread_mutex_unlock(&esperar_bajada);
	
	
	
	pthread_mutex_lock(&esperar_bajada);
	esperando_bajar[destino]--;
	pthread_mutex_unlock(&esperar_bajada);
	
}
