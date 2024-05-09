/*


2) Es suficiente ejecutar popa para continuar con la ejecución de next_thread, ya que esta instrucción restaura el contexto del hilo
que se eligio, es decir restaurar el almacenado en next_thread. Entonces la cpu empieza a ejecutarlo.

3) La politica de planificacion para los hilos en xv6 no es apropiativa sino cooperativa. Esta politica de planificacion usa colas FIFO
en la que los mismos procesos deciden dejar de utilizar la CPU al ejecutar la porcion de codigo thread_yield(); que hace que el proceso
pase a estado Runnable y ejecuta el planificador.

4) Si se ejecutan más de dos hilos, se ejecutaran solamente los dos primeros y no se ejecutará el tercero hasta que los primeros dos terminen.
Esto se debe a que el planificador empieza a recorrer el array de hilos desde el principio, por ende encontrará a los primeros
dos hasta que estos lugares se liberen.



*/