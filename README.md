# Sistemas Operativos y Redes

## Trabajo Práctico de hilos y semáforos


#### Nombre: Lautaro Manuel Avalos
------------------------------------
Punto1:
    Uno de los problemas encontrados en este punto fue la manera de leer el archivo (receta.txt),la solucion encontrada fue leer las acciones con la funcion fscanf y copiarlas con la funcion strcpy a los datos del hilo,además para leer los ingredientes se utilizaron tokens separados por el caracter "-".Todos los equipos leen del mismo archivo.Con esta solucion se logro evitar el hardcodeo de las variables.
------    
Punto3:
    Para la correcta escritura del log en un archivo primero se limpio el archivo con el modo "w"y luego dentro de la funcion tambien encargada de imprimir por pantalla se abre nuevamente el archivo y se escribe linea con el modo "a" que no sobreescribe el archivo para eitar la perdida de informacion.Asi es como la salida estandar y el archivo son iguales.
------
Paso4:
  Para la correcta manipulacion de zonas criticas se utilizaron mutex inicializados al comienzo del cofigo de manera global para poder utilizarlos en las funciones requeridas sin necesidad de inicializarlos dentro,ya que esto provocaria la inicializacion de un mutex por cada hilo y no tendria sentido.
------
Punto7:
Adjunto en un .txt
-------
