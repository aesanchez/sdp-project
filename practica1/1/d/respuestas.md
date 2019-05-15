## Multiplicacion por bloques

Se subdivide una matriz en submatrices, trazando imaginariamente lineas horizontales y verticales. La division de submatrices puede ser heterogenea.

En criollo, se ven esas submatrices como un numero unico a la hora de hacer la multiplicacion.

## Resultados

| N     | punto 1a optimizado	| x bloques r=1	| x bloques r=2	| x bloques r=4	| x bloques r=8	| x bloques r=16| x bloques r=32| x bloques r=64|
| ---   | ---			| ---		| ---		| ---		| ---		| ---		| ---		| ---		|
| 512   | 0.476011		| 1.612549	| 1.084336	| 0.737438	| 0.669283	| 0.629638	| 0.674617	| -		|	
| 1024  | 4.448955		| 30.971691	| 14.040102	| 8.649039	| 5.740517	| -		| 5.270401	| 5.228514	|	
| 2048  | 34.302676		| -		| 107.804613	| - 		| 63.354144	| 57.960320	| 49.708186	| 45.588972	|

 > "Según el tamaño de las matrices y de bloque elegido ¿Cuál es más rápido? ¿Por qué? ¿Cuál
sería el tamaño de bloque óptimo para un determinado tamaño de matriz?"

Al definir y operar por subbloques, aprovecha la localidad temporal y espacial explícitamente.
Trae bloques de datos juntos, y los usa hasta que los gasta (hace todas las operaciones que
tiene que hacer con esos datos). De esta forma accede más veces a cache y menos a
memoria.
El tamanio optimo depende del tamaño de las memorias cache.