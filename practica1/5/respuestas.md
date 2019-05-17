## Inciso a)

| N		| R		| While		| For		|
| ---		| ---		| ---		| ---		|
| 512	 	| 1	 	| 0.190558	| 0.172058	|
| 512		| 10		| 1.746748 	| 1.800963	|
| 512		| 100		| 15.970073	| 16.489638	|
| 1024		| 1		| 1.392565 	| 1.324322	|
| 1024		| 10		| 14.798078	| 16.093656	|

Se puede apreciar que por lo general tardan lo mismo las dos opciones del while y del for, pero en algunos casos tarda menos el while.

## Inciso b)

| N		| R		| a[i]		| *p		|
| ---		| ---		| ---		| ---		|
| 512	 	| 10000000 	| 0.878701	| 1.746122	|
| 512		| 100000000	| 8.979370 	| 17.636470	|
| 1024		| 1000000	| 0.198989 	| 0.326450	|
| 1024		| 10000000	| 1.953350	| 3.336616	|
| 2048		| 10000000	| 3.630137	| 6.862905	|

Claramente hacer un barrido por puntero tarda menos.