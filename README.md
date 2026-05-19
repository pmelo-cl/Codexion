*Este proyecto ha sido creado como parte del currículo de 42 por pmelo-cl.*

# Codexion

## Descripción

**Codexion** es un simulador de programadores que trabajan en un espacio compartido y necesitan acceder a un número limitado de *dongles USB* para poder compilar su código. El objetivo es orquestar múltiples hilos (uno por programador) que compiten por estos recursos respetando políticas de planificación justas (FIFO o EDF), evitando que algún programador se "agote" (*burnout*) por no poder compilar a tiempo.

Cada programador alterna entre tres fases:

- **Compilación**: requiere tomar los dos dongles adyacentes a su posición.
- **Depuración**: fase de descanso sin necesidad de dongles.
- **Refactorización**: otra fase sin dongles antes de intentar compilar de nuevo.

La simulación termina cuando:

- Todos los programadores han completado un número mínimo de compilaciones
- Algún programador no consigue empezar a compilar antes de su *deadline* personal y se quema (*burnout*).

El programa gestiona la concurrencia mediante hilos POSIX, mutexes y variables de condición, implementando colas de espera con políticas FIFO (orden de llegada) o EDF (*Earliest Deadline First*), donde la prioridad la tiene quien tiene el plazo de *burnout* más cercano.

## Instrucciones

### Compilación

El proyecto incluye un `Makefile` con las reglas `all`, `clean`, `fclean` y `re`.  
Para compilar, simplemente ejecuta en la raíz del proyecto:

```bash
make
```

Se generará el ejecutable codexion. La compilación se realiza con -Wall -Wextra -Werror -pthread.

### Ejecución

```bash

./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

### Parámetros (todos obligatorios, en milisegundos excepto los contadores)

| Parámetro                   | Descripción                                                                                                             | Rango    |
|:----------------------------|:------------------------------------------------------------------------------------------------------------------------|:---------|
| number_of_coders            | Número de programadores (y también de dongles).                                                                         | ≥ 2.     |
| time_to_burnout             | Tiempo máximo que un programador puede estar en estado IDLE sin comenzar una nueva compilación. Si se supera, se quema. | ≥1       |
| time_to_compile             | Duración de la fase de compilación (sujetando los dos dongles).                                                         | ≥1       |
| time_to_debug               | Duración de la fase de depuración.                                                                                      | ≥1       |
| time_to_refactor            | Duración de la fase de refactorización.                                                                                 | ≥1       |
| number_of_compiles_required | Número de compilaciones que cada programador debe completar para que la simulación termine con éxito.                   | ≥1       |
| dongle_cooldown             | Tiempo de enfriamiento de un dongle tras ser liberado (no puede tomarse durante ese periodo).                           | ≥0       |
| scheduler                   | Política de asignación de dongles: fifo (First In, First Out) o edf (Earliest Deadline First).                          | fifo/edf |

### Ejemplo

```bash

./codexion 3 500 200 100 100 2 0 edf
```

Esto lanza una simulación con:

- 3 programadores y 3 dongles

- Burnout a los 500 ms sin compilar

- Compilación de 200 ms, depuración y refactorización de 100 ms cada una

- Se requieren 2 compilaciones por programador

- Dongles sin enfriamiento (0)

- Planificación por fecha límite más cercana (edf)

### Logs de salida

El programa imprime en tiempo real las acciones de cada programador con el siguiente formato:


<timestamp_ms> <coder_id> has taken a dongle  
<timestamp_ms> <coder_id> is compiling  
<timestamp_ms> <coder_id> is debugging  
<timestamp_ms> <coder_id> is refactoring  
<timestamp_ms> <coder_id> burned out  

La salida está protegida por un mutex, por lo que las líneas nunca aparecerán mezcladas o truncadas.
Blocking cases handled

La solución maneja explícitamente los siguientes problemas clásicos de concurrencia:

- **Deadlock** (interbloqueo): cada programador necesita dos dongles adyacentes. Para evitar que dos programadores tomen cada uno un dongle y esperen eternamente el otro, se impone un orden canónico en la adquisición: los dongles se solicitan siempre en orden ascendente de dirección de memoria. Esto elimina la posibilidad de espera circular (condición de Coffman).

- **Inanición** (starvation): con el planificador FIFO, los programadores que llevan más tiempo esperando son atendidos antes. Con EDF, se prioriza al que tiene el deadline más próximo, evitando que alguien con un plazo lejano bloquee indefinidamente a los demás.

- **Gestión del cooldown**: cuando un dongle se libera, entra en un periodo de enfriamiento durante el cual no puede ser tomado. Los hilos que esperan en la cola del dongle son despertados mediante pthread_cond_broadcast y verifican si ya ha pasado el cooldown antes de tomarlo.

- **Detección precisa del burnout**: un hilo monitor independiente comprueba periódicamente si algún programador en estado IDLE ha superado su deadline. Para evitar falsos positivos, el monitor solo considera a aquellos que aún no han completado las compilaciones requeridas. El mensaje de burnout se imprime dentro de los 10 ms posteriores al evento real, cumpliendo el requisito del subject.

- **Serialización del log**: toda escritura en la salida estándar se realiza dentro de una sección crítica protegida por print_mutex, garantizando que los mensajes de distintos hilos no se solapen.

- **Finalización ordenada**: cuando la simulación debe detenerse (por éxito o por burnout), se activa una bandera global running bajo mutex y se despierta a todos los hilos bloqueados en dongles mediante pthread_cond_broadcast. Así los hilos pueden terminar limpiamente y liberar sus recursos.

## Mecanismos de sincronización

La coordinación entre hilos se basa en las siguientes primitivas `POSIX`:

- Mecanismo Uso en el proyecto

- **pthread_mutex_t**: Protege el acceso a cada dongle (mutex), al estado de cada programador (state_mutex), a la bandera global running y a la salida de logs (print_mutex).

- **pthread_cond_t**: Cada dongle tiene una variable de condición (cond) sobre la que los programadores esperan hasta que el dongle esté disponible. El monitor también tiene su propia variable de condición para ser notificado de cambios de estado.

- **pthread_cond_wait** / **pthread_cond_timedwait**: Los programadores se bloquean en la cola del dongle hasta que son despertados cuando el recurso cambia de estado. El monitor usa timedwait para despertarse justo en el siguiente deadline y comprobar burnout.

- **pthread_cond_broadcast**: Cuando un dongle se libera, se usa broadcast para despertar a todos los hilos en espera. Cada uno reevalúa si es el primero de la cola y si el dongle está libre y sin cooldown.
Cómo se evitan las condiciones de carrera

- **Estado de los dongles**: toda modificación de occupied o cooldown_end ocurre bajo el mutex del dongle. Las colas de espera también se manipulan dentro de esa sección crítica.

- **Estado de los programadores**: state y deadline se actualizan exclusivamente bajo state_mutex, y el monitor los consulta bajo el mismo mutex.

- **Bandera global running**: cualquier lectura o escritura se hace con running_mutex bloqueado, asegurando visibilidad inmediata entre hilos.

- **Adquisición ordenada de múltiples dongles**: al ordenar por dirección de memoria, se garantiza que todos los hilos adquieran los recursos en el mismo orden global, previniendo el deadlock.

## Comunicación thread-safe entre programadores y monitor

Cuando un programador cambia a estado IDLE, actualiza su deadline y el monitor es notificado mediante pthread_cond_signal sobre su variable de condición. Así el monitor recalcula el siguiente deadline y ajusta su espera.

El monitor detiene la simulación poniendo running = 0 bajo mutex y luego hace broadcast en todos los dongles. Los programadores, al despertarse, comprueban running y salen limpiamente.

## Recursos y uso de IA

### Referencias técnicas

- Documentación oficial de pthreads: man pthreads

- Programming with POSIX Threads – David R. Butenhof

- Material del campus 42 sobre sincronización y condiciones de carrera.

### Uso de inteligencia artificial

Durante el desarrollo se ha utilizado IA para:

- Generar ejemplos de estructuras de datos para las colas de espera.

- Revisar la lógica de prevención de deadlocks en la adquisición ordenada de dongles.

- Redactar comentarios y mensajes de depuración.

- Obtener sugerencias sobre el manejo de pthread_cond_timedwait con tiempos absolutos.
