## Inciso a)

Con x=39916801 y=719 obtuvimos:

| Suma		| Resta		| Producto	| Division	|
| ---		| ---		| ---		| ---		|
| 1.7550179958	| 1.6542949677	| 1.6506130695	| 1.6446940899	|

> ¿Qué análisis se puede hacer de cada operación?

Repitiendo varias veces el programa se puede notar que los resultados tienden a ser casi iguales.

> ¿Qué ocurre si x e y son potencias de 2?

Con x=39916801 y=1024 obtuvimos:

| Suma		| Resta		| Producto	| Division	|
| ---		| ---		| ---		| ---		|
| 1.8721010685	| 1.8418200016	| 1.6713540554	| 1.7801520824	|

Misma conclusion.

Con x=2^28=268435456 y=1024 obtuvimos:

| Suma		| Resta		| Producto	| Division	|
| ---		| ---		| ---		| ---		|
| 1.8274328709	| 1.9228670597	| 1.7414731979	| 1.7528579235	|

Misma conclusion.

## Inciso b)

Agregamos un orden de magnitud a la iteracion del bucle para poder analizar mejor los resultados y obtuvimos lo siguiente.

| Producto	| Division	|
| ---		| ---		|
| 15.8173999786	| 14.9665210247	|

Creo que lo que se trata de mostrar en los incisos a y b es que conviene hacer multiplicaciones en vez de divisiones. Pero los resultados que obtenemos indican lo contrario.

## Inciso c)

Con mismo N y varios m suele dar similar. Por ejemplo con N=10000000:

| Parametro	| Modulo	| Equivalencia	|
| ---		| ---		| ---		|
| m=512		| 0.104682	| 0.056414	|

En todos los casos nos dio que la segunda opcion es mucho mas rapida. Por lo tanto, en este caso, utilziar mascara binaria es mas eficiente.
