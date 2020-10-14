
#include <stdio.h>      // libreria estandar
#include <stdlib.h>     // para usar exit y funciones de la libreria standard
#include <string.h>
#include <pthread.h>    // para usar threads
#include <semaphore.h>  // para usar semaforos
#include <unistd.h>


#define LIMITE 50
//creo variable para la segunda hamburguesa
int segunda=2;
//inicializo los MUTEX

pthread_mutex_t salero= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t horno= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t plancha= PTHREAD_MUTEX_INITIALIZER;

//creo estructura de semaforos 
struct semaforos {
    sem_t sem_mezclar;
    sem_t sem_salar;
    sem_t sem_armar;
    sem_t sem_cocinar;
    sem_t sem_terminar;
    sem_t sem_terminar2;
    sem_t sem_terminar3;
	//poner demas semaforos aqui
};

//creo los pasos con los ingredientes
struct paso {
   char accion [LIMITE];
   char ingredientes[4][LIMITE];
   
};

//creo los parametros de los hilos 
struct parametro {
 int equipo_param;
 struct semaforos semaforos_param;
 struct paso pasos_param[8];
};

//funcion para imprimir las acciones y los ingredientes de la accion
void* imprimirAccion(void *data, char *accionIn) {
	FILE * flujo = fopen("ejecucion.txt","a");
	if(flujo==NULL){
		perror("Error en la creaccion del archivo \n\n");
	}
	struct parametro *mydata = data;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos 
	int i;
	char *fin= "terminar";
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){
		printf("Equipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		fprintf(flujo,"Equipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		if(strcmp(accionIn,fin) == 0){
		 printf("\t\tESTE EQUIPO YA TERMINO!!\n");
		 fprintf(flujo,"\t\tESTE EQUIPO YA TERMINO!!\n");
		}
		//calculo la longitud del array de ingredientes
		int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
		//indice para recorrer array de ingredientes
		int h;
		printf(" -----------INGREDIENTES ----------\n");
		fprintf(flujo," -----------INGREDIENTES ----------\n"); 
			for(h = 0; h < sizeArrayIngredientes; h++) {
				//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
				if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
							printf(" Ingrediente  %d : %s \n",h,mydata->pasos_param[i].ingredientes[h]);
							fprintf(flujo," Ingrediente  %d : %s \n",h,mydata->pasos_param[i].ingredientes[h]);

				}
			}
		}
	}
	//solo estetico
	printf("\n");
	fprintf(flujo,"\n");
	//cierro el archivo
	fflush(flujo);
	fclose(flujo);
}

//funcion para tomar de ejemplo
void* cortar(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "cortar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	for(int cont=0;cont<segunda;cont++){
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 1000000 );
	}
	//doy la senal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_mezclar);
	
    pthread_exit(NULL);
}
void* mezclar(void *data) {
        //creo el nombre de la accion de la funcion
        char *accion = "mezclar";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;
         sem_wait(&mydata->semaforos_param.sem_mezclar);
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 1000000 );
        //doy la senal a la siguiente accion (cortar me habilita mezclar)
    	sem_post(&mydata->semaforos_param.sem_salar);

    pthread_exit(NULL);
}
void* salar(void *data) {
	//bloqueo el mutex
	pthread_mutex_lock(&salero);
        //creo el nombre de la accion de la funcion
        char *accion = "salar";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;
         sem_wait(&mydata->semaforos_param.sem_salar);
        //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 1000000 );
        //doy la senal a la siguiente accion (cortar me habilita mezclar)
        sem_post(&mydata->semaforos_param.sem_armar);
	//desbloqueo el mutex
        pthread_mutex_unlock(&salero);

    pthread_exit(NULL);
}
void* armar(void *data) {
	//creo el nombre de la accion de la funcion
        char *accion = "armar";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;
         sem_wait(&mydata->semaforos_param.sem_armar);
	for(int cont=0;cont<segunda;cont++){
        //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 1000000 );
	}	
	 //doy la senal a la siguiente accion (cortar me habilita mezclar)
        sem_post(&mydata->semaforos_param.sem_cocinar);

    pthread_exit(NULL);
}
void* cocinar(void *data) {
	//bloqueo el mutex
	pthread_mutex_lock(&plancha);
        //creo el nombre de la accion de la funcion
        char *accion = "cocinar";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;
         sem_wait(&mydata->semaforos_param.sem_cocinar);
	for(int cont=0;cont<segunda;cont++){
        //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 2000000 );
 	}
        //doy la senal a la siguiente accion (cortar me habilita mezclar)
        sem_post(&mydata->semaforos_param.sem_terminar);
	//desbloqueo el mutex
	pthread_mutex_unlock(&plancha);


    pthread_exit(NULL);
}
void* terminar(void *data) {
        //creo el nombre de la accion de la funcion
        char *accion = "terminar";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;
         sem_wait(&mydata->semaforos_param.sem_terminar);
         sem_wait(&mydata->semaforos_param.sem_terminar2);
	 sem_wait(&mydata->semaforos_param.sem_terminar3);
	 //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 1000000 );
        //doy la senal a la siguiente accion (cortar me habilita mezclar)
        

    pthread_exit(NULL);
}
void* hornear(void *data) {
	//bloqueo el mutex
	pthread_mutex_lock(&horno);
        //creo el nombre de la accion de la funcion
        char *accion = "hornear";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;

        //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 4000000 );
        //doy la senal a la siguiente accion (cortar me habilita mezclar)
	sem_post(&mydata->semaforos_param.sem_terminar2);
	//desbloqueo el mutex
	pthread_mutex_unlock(&horno);

    pthread_exit(NULL);
}
void* cortarExtras(void *data) {
        //creo el nombre de la accion de la funcion
        char *accion = "cortarExtras";
        //creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero).
        struct parametro *mydata = data;
	for(int cont=0;cont<segunda;cont++){
        //llamo a la funcion imprimir le paso el struct y la accion de la funcion
        imprimirAccion(mydata,accion);
        //uso sleep para simular que que pasa tiempo
        usleep( 1000000 );
	}
        //doy la senal a la siguiente accion (cortar me habilita mezclar)
        sem_post(&mydata->semaforos_param.sem_terminar3);

    pthread_exit(NULL);
}


void* ejecutarReceta(void *i) {
	
	//variables semaforos
	sem_t sem_mezclar;
	sem_t sem_salar;
	sem_t sem_armar;
	sem_t sem_cocinar;
	sem_t sem_terminar;
	sem_t sem_terminar2;
	sem_t sem_terminar3;
	//crear variables semaforos aqui
	
	//variables hilos
	pthread_t p1; 
	pthread_t p2;
	pthread_t p3;
	pthread_t p4;
	pthread_t p5;
	pthread_t p6;
	pthread_t p7;
	pthread_t p8;

	//crear variables hilos aqui
	
	//numero del equipo (casteo el puntero a un int)
	int p = *((int *) i);

	//reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	//seteo los valores al struct
	
	//seteo numero de grupo
	pthread_data->equipo_param = p;

	//seteo semaforos
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
 	pthread_data->semaforos_param.sem_salar = sem_salar; 
 	pthread_data->semaforos_param.sem_armar = sem_armar;
 	pthread_data->semaforos_param.sem_cocinar = sem_cocinar;
 	pthread_data->semaforos_param.sem_terminar = sem_terminar;
 	pthread_data->semaforos_param.sem_terminar = sem_terminar2;
	pthread_data->semaforos_param.sem_terminar = sem_terminar3;
	//setear demas semaforos al struct aqui
	

	//seteo las acciones y los ingredientes (Faltan acciones e ingredientes) ¿Se ve hardcodeado no? ¿Les parece bien?

     	FILE* miarchivo=NULL;
	char* nombrearchivo="receta.txt";
	char accion[80];
	char ingrediente [80];
	char *token;
	int a;
	int j;

	miarchivo=fopen(nombrearchivo,"r");
	//if(miarchivo==NULL)return -1;
	
	for(a=0;a<8;a++){
		fscanf(miarchivo," %[^\n]",&accion);
		//se copia la accion leida en el archivo
		strcpy(pthread_data->pasos_param[a].accion,accion);
		j=0;
		fscanf(miarchivo," %[^\n]",&ingrediente);
		token= strtok(ingrediente,"-");
		while(token){
			//se copia en ingredientes el ingrediente del token
			strcpy(pthread_data->pasos_param[a].ingredientes[j],token);
			j=j+1;
			token= strtok(NULL,"-");

		}
	}

	//inicializo los semaforos

	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_salar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_armar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_cocinar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_terminar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_terminar2),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_terminar3),0,0);

	//inicializar demas semaforos aqui


	//creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos 
    int rc;
    rc = pthread_create(&p1,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia

    rc = pthread_create(&p2,                           //identificador unico
                            NULL,                          //atributos del thread
                                mezclar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia


    rc = pthread_create(&p3,                           //identificador unico
                            NULL,                          //atributos del thread
                                salar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia

    rc = pthread_create(&p4,                           //identificador unico
                            NULL,                          //atributos del thread
                                armar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia

    rc = pthread_create(&p5,                           //identificador unico
                            NULL,                          //atributos del thread
                                cocinar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia

    rc = pthread_create(&p6,                           //identificador unico
                            NULL,                          //atributos del thread
                                hornear,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia

    rc = pthread_create(&p7,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortarExtras,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
    rc = pthread_create(&p8,                           //identificador unico
                            NULL,                          //atributos del thread
                                terminar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia


	//crear demas hilos aqui
	
	
	//join de todos los hilos
	pthread_join (p1,NULL);
	pthread_join (p2,NULL);
	pthread_join (p3,NULL);
	pthread_join (p4,NULL);
	pthread_join (p5,NULL);
	pthread_join (p6,NULL);
	pthread_join (p7,NULL);
	pthread_join (p8,NULL);

	//crear join de demas hilos


	//valido que el hilo se alla creado bien 
    if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     }

	 
	//destruccion de los semaforos 
	sem_destroy(&sem_mezclar);
	sem_destroy(&sem_salar);
	sem_destroy(&sem_armar);
	sem_destroy(&sem_cocinar);
	sem_destroy(&sem_terminar);
	sem_destroy(&sem_terminar2);
	sem_destroy(&sem_terminar3);

	//destruir demas semaforos 
	
	//salida del hilo
	 pthread_exit(NULL);
}


int main ()
{
	//limpio el archivo que va a guardar la ejecucion
	FILE * flujo = fopen("ejecucion.txt","w");
	if(flujo==NULL){
		perror("Error en la creaccion del archivo \n\n");
	}
	else{
		fflush(flujo);
		fclose(flujo);

	}
	//creo los nombres de los equipos 
	int rc;
	int *equipoNombre1 =malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 =malloc(sizeof(*equipoNombre2));
	int *equipoNombre3 =malloc(sizeof(*equipoNombre3));
	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
	*equipoNombre3 = 3;

	//creo las variables los hilos de los equipos
	pthread_t equipo1; 
	pthread_t equipo2;
	pthread_t equipo3;
 
	//inicializo los hilos de los equipos
    rc = pthread_create(&equipo1,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre1); 

    rc = pthread_create(&equipo2,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre2);

    rc = pthread_create(&equipo3,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre3);

   if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     } 

	//join de todos los hilos
	pthread_join (equipo1,NULL);
	pthread_join (equipo2,NULL);
	pthread_join (equipo3,NULL);


    pthread_exit(NULL);
}


//Para compilar:   gcc HellsBurgers.c -o ejecutable -lpthread
//Para ejecutar:   ./ejecutable
