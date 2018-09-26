					  # Informe

####################################################################################################

					### Integrantes:
					* Joaquín Tellez
					* Gustavo Vicuña

####################################################################################################

## Implementado:
* Optget
* Fork
* Formato de salida
* Serial Quicksort
* Parallel Quicksort
* Resultados
* Threads (6)

####################################################################################################

## Algunos errores:
* Logramos hacer que el Parallel Quicksort funcionara para un thread, al principio, pero al agregar los demás threads sospechamos que no funciona tal como el enunciado dice. Creemos que funciona cada uno ordenando el mismo arreglo pero sin el mismo pivote.

####################################################################################################

## A Considerar
* La funcion "first" sirve como una especie de puente entre el thread y la funcion parallel quicksort. Esto se usa de esta manera, ya que el thread solo puede recibir una funcion de tipo de retorno void y que reciba argumento de tipo void. Es por esto que utilizamos "first" para que el thread pueda recibir el struct donde se almacena la informacion necesaria para la funcion parallel_quicksort.
* La funcion parallel quicksort crea los threads necesarios que ordenan el subarreglo asignado y hacen llamadas recursivas a la misma funcion parallel quicksort hasta que quede todo localmente ordenado, luego se utiliza quicksort para ordenar globalmente.

####################################################################################################

## Por Implementar:
* Cambiar el tamaño del sub arreglo que reciben los threads segun el pivote utilizado. El ordenamiento se realiza con exito, pero creemos que podemos arreglar nuestro codigo para hacerlo mas eficiente.
