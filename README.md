# Cliente UDP - README

Este es un proyecto de cliente UDP que implementa un protocolo de comunicación a través de sockets UDP. El programa consta de un archivo `clientmain.cpp` que contiene el programa principal del cliente.

## Requisitos

El proyecto requiere las siguientes bibliotecas:

- `<iostream>`
- `<fstream>`
- `<cstring>`
- `<unistd.h>`
- `<cstdio>`
- `<cstdlib>`
- `<sys/socket.h>`
- `<sys/types.h>`
- `<netinet/in.h>`
- `<arpa/inet.h>`
- `<memory>`
- `<ctime>`
- `<chrono>`
- `<cstdint>`
- `<string>`
- `<fcntl.h>`
- `<cmath>`
- `<stdio.h>`
- `<set>`
- `<map>`

## Estructuras de datos

El programa utiliza las siguientes estructuras para representar los paquetes de datos:

```cpp
struct Packet {
    uint16_t cksum;
    uint16_t len;
    uint32_t seqno;
    char data[CHUNK_SIZE];
};

struct Ack_Packet {
    uint16_t cksum;
    uint16_t len;
    uint32_t ackno;
};

struct Ack_Server_Packet {
    uint32_t packets_numbers;
};

```


## Clase Sender
La clase Sender se utiliza para enviar paquetes a través del socket. Tiene los siguientes métodos:

- send_packet(Packet packet, int socket_fd): envía un paquete de datos.
- send_ack(Ack_Packet ack_packet, int socket_fd): envía un paquete de confirmación.
- send_server_ack(Ack_Server_Packet ack_server_packet, int socket_fd): envía un paquete de confirmación del servidor.
## Clase Receiver
La clase Receiver se utiliza para recibir paquetes del socket.

## Uso
Para utilizar este cliente UDP, sigue los siguientes pasos:

- Compila el archivo clientmain.cpp utilizando el compilador de C++.
- Ejecuta el programa generado.
- Sigue las instrucciones del programa para realizar la comunicación con el servidor.

---

¡Este es solo un resumen del archivo `clientmain.cpp`! Asegúrate de revisar el código fuente completo para poder hacer mejoras!. 

# ServerMain.cpp

Este es el archivo de código fuente `servermain.cpp` que contiene la implementación de un servidor en C++.

## Dependencias

El archivo de código utiliza las siguientes bibliotecas:

- iostream
- string
- cstring
- stdio.h
- sys/socket.h
- sys/types.h
- arpa/inet.h
- netinet/in.h
- unistd.h
- chrono
- cstdlib
- ctime
- cstdio
- fcntl.h
- cmath
- set
- vector
- map
- bits/stdc++.h
- stdint.h
- signal.h
- stdbool.h
- thread
- mutex
- fstream

## Constantes

El archivo define las siguientes constantes:

- `CHUNK_SIZE`: Tamaño del fragmento de datos en bytes.
- `STOP_AND_WAIT_TIMEOUT`: Tiempo de espera en segundos para el protocolo Stop-and-Wait.
- `SR_TIMEOUT`: Tiempo de espera en segundos para el protocolo Selective Repeat.

## Estructuras de datos

El archivo define las siguientes estructuras:

- `Packet`: Estructura para paquetes de solo datos que incluye un número de secuencia y una suma de verificación.
- `Ack_Packet`: Estructura para paquetes de solo reconocimiento.
- `Ack_Server_Packet`: Estructura para la confirmación del servidor, incluida la cantidad de paquetes del archivo deseado.

## Clase Sender

La clase `Sender` se utiliza para enviar paquetes a través de un socket. Proporciona los siguientes métodos:

- `send_packet`: Envía un objeto `Packet` a través de un socket especificado.
- `send_ack`: Envía un objeto `Ack_Packet` a través de un socket especificado.
- `send_server_ack`: Envía un objeto `Ack_Server_Packet` a través de un socket especificado.

## Clase Receiver

La clase `Receiver` se utiliza para recibir paquetes a través de un socket. Proporciona los siguientes métodos:

- `receive_packet`: Recibe un objeto `Packet` a través de un socket especificado.
- `receive_ack_packet`: Recibe un objeto `Ack_Packet` a través de un socket especificado.
- `receive_ack_server_packet`: Recibe un objeto `Ack_Server_Packet` a través de un socket especificado.

## Clase PacketHandler

La clase `PacketHandler` proporciona métodos para manipular paquetes y calcular sumas de verificación. Incluye métodos para crear paquetes y paquetes de reconocimiento, así como para calcular y comparar sumas de verificación.

## Clase FileReader

La clase `FileReader` se utiliza para leer un archivo. Proporciona métodos para obtener el tamaño del archivo, leer fragmentos de datos y cerrar el archivo.

## Clase FileWriter

La clase `FileWriter` se utiliza para escribir en un archivo. Proporciona métodos para escribir fragmentos de datos en el archivo y cerrar el archivo.

## Clase SR_Receiver

La clase `SR_Receiver` implementa un receptor que utiliza el protocolo Selective Repeat para recibir archivos. Proporciona un método `recevFile` para iniciar la recepción del archivo.

---

¡Este es solo un resumen del archivo `servermain.cpp`! Asegúrate de revisar el código fuente completo para poder hacer mejoras!. 
