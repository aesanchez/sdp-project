# Distribucion del trabajo en la version original con N=16 T=4

|		|ID:0		|ID:1		|ID:2		|ID:3		|
|---		|---		|---		|---		|---		|
|i		|(0..3)		|(4..7) 	|(8..11)	|(12..14)	|
|#iteraciones	|54		|38 		|22		|6		|

Vemos que divide "equitativamente" la cantidad de iteraciones del primer for. El problema recae en que la cantidad de iteraciones
del segundo for desminuye a medida que aumenta i. Por lo tanto, en terminos de trabajo total, no se lo esta dividiendo correctamente.

# Distribucion del trabajo version arreglada con N=16 T=4

Se agrega la directiva de sincronizacion `schedule(dynamic,1)` que distribuye chunks(de tamanio 1) por demanda.

|		|ID:0		|ID:1		|ID:2		|ID:3		|
|---		|---		|---		|---		|---		|
|i		| aleatorio	|aleatorio	|aleatorio	|aleatorio	|
|#iteraciones	|36		|32		|28		|28		|

Vemos que de esta manera el trabajo esta mas balanceado, por lo tanto el tiempo total es menor. (la distribucion es no determinante)

# Comparacion con valores mas grandes

En la siguiente tabla se puede ver el numero de iteraciones para N=2048 y T=4. Se nota en mas medida los desbalances, y el impacto de la solucion.

|		|ID:0		|ID:1		|ID:2		|ID:3		| tiempo final	|
|---		|---		|---		|---		|---		| ---		|
|original	|917248		|655104		|392960		|130816		| 0.06119	|
|dynamic	|526532		|520410		|525269		|523917		| 0.02481	|