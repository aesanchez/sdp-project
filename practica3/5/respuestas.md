## Teoria

`sections`: se utiliza para distribuir secciones de código independiente entre los hilos. Cada sección se ejecuta una sola vez por un único hilo.

## ¿Se Consigue mayor speedup al incrementar la cantidad de threads? Justificar

No, porque aunque tengamos mucha cantidad de threads, cada seccion se va a ejecutar con un unico hilo.

Se podra subdividr cada seccion en mas threads para mejorar el speed up? Vaya uno a saber