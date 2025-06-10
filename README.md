
Sistema operativo simple y eficiente basado en BAREBONES x86_64. Creado para 72.11 SISTEMAS OPERATIVOS/2024.

# Compilacion y ejecucion

Es necesario contar con docker y QEMU instalados.

Se utiliza docker para compilar el SO, y QEMU para su ejecución. 

Para la compilación, se puede conseguir la imagen de docker y ejecutar un contenedorcon el siguiente comando:

### En macOS

| $> docker run --platform linux/amd64 -it -v /tu_path_local:/shared agodio/itba-so-multi-platform:3.0 |


### En Linux o usando WSL

| $> docker run -it -v /tu_path_local:/shared agodio/itba-so-multi-platform:3.0 |


Luego, ya en el docker:

| $> cd root |


Para compilar se ejecuta este comando en el directorio 'root' del SO y en el directorio 'ToolChain':

| $> make clean all |


También podemos optar por usar el buddy system como memory manager (notar que no funciona correctamente):

| $> make clean buddy |


Cuando esté compilado se debe correr el siguiente comando desde la máquina local, en el directorio del SO:

| $> ./run.sh |


O:

| $> qemu-system-x86_64 Image/x64BareBonesImage.qcow2 -m 512 --rtc base=localtime |


# Comandos

|  Comando    |    Descripción |


| `help`o `ls` | Muestra información sobre estos comandos |


| `time` | Muestra la hora actual |


| `clear` | Limpia la pantalla |


| `(+)` | Aumenta el tamaño de la fuente |


| `(-)` | Disminuye el tamaño de la fuente |


| `registersinfo` | Imprime los valores actuales de los registros |


| `zerodiv` | Prueba la excepción de división por cero |


| `invopcode` | Prueba la excepción de código de operación inválido |


| `memtest` | Prueba el gestor de memoria |


| `schetest` | Prueba el planificador de procesos |


| `priotest` | Prueba el planificador de prioridades |


| `testsync` | Prueba la sincronización de procesos |


| `ps` | Lista todos los procesos |


| `cat` | Imprime el contenido de un archivo |


| `loop` | Imprime un saludo corto y el PID del proceso |


|`kill [PID]`|  Termina el proceso especificado |


| `block [PID]`| Bloquea el proceso especificado |


|`unblock [PID]` | Desbloquea el proceso especificado |


| `nice [PID] [prio]` | Cambia la prioridad de un proceso dado |


| `philo` | Prueba el problema de los filósofos comensales |


| `wc` | Cuenta el número total de líneas de entrada |


| `filter` | Filtra todas las vocales de la entrada |


| `mem` | Imprime el estado de la memoria |


| `exit` | Sale del SO |
