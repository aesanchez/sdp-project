## Resultados

Con el valor original N=100000000, no habia diferencia alguna. Utilizando N=1000000000000 obtuvimos lo siguiente.

| 1		| 2		|
| ---		| ---		|
| 36.0050580502	| 42.0203969479	|

> ¿Cuál es más rápido? ¿Por qué?

La primer forma es mas rapida. La diferencia recae que hace el calculo de las variables independientes de la sumatoria, afuera del bucle. Por lo que calcula el valor una sola vez. Mientras que el segundo lo hace en todas las iteraciones.