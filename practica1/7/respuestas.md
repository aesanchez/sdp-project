
	En la linea int x = n*p/100 n * p alcanza un valor de 2500000000, el cual excede el valor maximo que puede guardarse en una variable de tipo int. 
	Dependiendo del rango de valores en el cual sera aplicado el programa y asumiendo que unicamente se ingresaran numeros enteros, una solucion practica seria reescribir la linea de la siguiente manera:
	
	int x = (p/100)*n;
	
	De esta forma se incrementa el valor maximo permitido por el programa.