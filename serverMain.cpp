#include <iostream>

using namespace std;

#include <string>
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <cstdio>
#include <fcntl.h>
#include <cmath>
#include <set>
#include <vector>
#include <map>
#include <bits/stdc++.h>
#include <stdint.h>
#include <signal.h>
#include <stdbool.h>
#include <thread>
#include <mutex>
#include <fstream>


//#include "constants.h"
/////////////////////////////////////////////
#define CHUNK_SIZE 20000
#define STOP_AND_WAIT_TIMEOUT 5
#define SR_TIMEOUT 5
/////////////////////////////////////////////

//#include "Packet.h"
/////////////////////////////////////////////
/*Paquetes de solo datos: 
un paquete de datos de tamaño fijo con un encabezado que incluye 
un número de secuencia y una verificación de suma*/
struct Packet {
    /* Header */
    uint16_t cksum; /* Parte de bonificación opcional */
    uint16_t len = -1;
    uint32_t seqno;
    /* Data */
    char data[CHUNK_SIZE]; /* No siempre 500 bytes, puede ser menos */
};

/* Los paquetes de solo reconocimiento tienen solo 8 bytes */
struct Ack_Packet {
    uint16_t cksum; /* Parte de bonificación opcional */
    uint16_t len;
    uint32_t ackno;
};

/* Confirmación del servidor, incluida la cantidad de paquetes del archivo deseado */
struct Ack_Server_Packet {
    uint32_t packets_numbers;
};
/////////////////////////////////////////////

//#include "Sender.h"
/////////////////////////////////////////////
class Sender {

private:
    struct sockaddr_in socket_address;

public:
    explicit Sender(struct sockaddr_in socket_addres) {
    Sender::socket_address = socket_addres;
}
    explicit Sender() {

}

    /*se utiliza para enviar un objeto Packet a través de un socket 
    especificado por socket_fd. El paquete se envía a la dirección 
    de socket especificada en el miembro de datos socket_address de 
    la clase Sender.

    La función sendto es una función de bajo nivel de sockets que se 
    utiliza para enviar datos a través de un socket. Se le pasan cuatro 
    argumentos: el descriptor de socket, un puntero al buffer de datos 
    que se envía, el tamaño del buffer de datos, y la dirección del 
    destinatario. La opción MSG_CONFIRM indica que la función sendto 
    debe esperar a que el paquete se envíe correctamente antes de 
    devolver control al programa.

    La función también imprime un mensaje de depuración que muestra 
    el número de secuencia del paquete que se está enviando. Esto puede 
    ser útil para depurar y rastrear los paquetes que se envían y reciben 
    durante la transmisión.*/
    void send_packet(Packet packet, int socket_fd) {
    cout << "enviando paquete " << packet.seqno << endl;
    sendto(socket_fd, &packet, sizeof(packet),
            MSG_CONFIRM,
            (const struct sockaddr*)&socket_address,
                    sizeof(socket_address));
}

    /*se utiliza para enviar un objeto Ack_Packet a través de un socket 
    especificado por socket_fd. El paquete de reconocimiento se envía a 
    la dirección de socket especificada en el miembro de datos s
    ocket_address de la clase Sender.

    La función sendto es una función de bajo nivel de sockets que se 
    utiliza para enviar datos a través de un socket. Se le pasan cuatro 
    argumentos: el descriptor de socket, un puntero al buffer de datos 
    que se envía, el tamaño del buffer de datos, y la dirección del 
    destinatario. La opción MSG_CONFIRM indica que la función sendto 
    debe esperar a que el paquete se envíe correctamente antes de devolver 
    control al programa.

    Los paquetes de reconocimiento se utilizan para indicar que se ha 
    recibido un paquete de datos correctamente y para permitir que el 
    emisor envíe el siguiente paquete. Esto es esencial para la 
    implementación de un protocolo de transporte de datos fiable, ya que 
    ayuda a garantizar que todos los paquetes de datos se entregan 
    correctamente y en el orden correcto.*/
    void send_ack(Ack_Packet ack_packet, int socket_fd) {
    sendto(socket_fd, &ack_packet, sizeof(ack_packet), MSG_CONFIRM, (const struct sockaddr*)&socket_address, sizeof(socket_address));
}
    /*se utiliza para enviar un objeto Ack_Server_Packet a través de un 
    socket especificado por socket_fd. El paquete de reconocimiento del 
    servidor se envía a la dirección de socket especificada en el miembro
     de datos socket_address de la clase Sender.

    La función sendto es una función de bajo nivel de sockets que se 
    utiliza para enviar datos a través de un socket. Se le pasan cuatro 
    argumentos: el descriptor de socket, un puntero al buffer de datos 
    que se envía, el tamaño del buffer de datos, y la dirección del 
    destinatario. La opción MSG_CONFIRM indica que la función sendto debe 
    esperar a que el paquete se envíe correctamente antes de devolver 
    control al programa.

    Los paquetes de reconocimiento del servidor se utilizan para confirmar 
    que el servidor ha recibido la solicitud del cliente y para 
    proporcionar información adicional sobre el archivo que se va a 
    enviar. Por ejemplo, el paquete de reconocimiento del servidor 
    puede incluir el tamaño del archivo y el número de paquetes que se 
    espera enviar. Esto es esencial para la implementación de un protocolo 
    de transporte de datos fiable, ya que ayuda a garantizar que el 
    cliente tenga toda la información necesaria para iniciar la 
    recepción del archivo.*/
    void send_server_ack(Ack_Server_Packet ack_server_packet, int socket_fd) {
    sendto(socket_fd, &ack_server_packet,
            sizeof(ack_server_packet), MSG_CONFIRM,
            (const struct sockaddr*)&socket_address,
                    sizeof(socket_address));
}
};
/////////////////////////////////////////////

//#include "Receiver.h"
/////////////////////////////////////////////
class Receiver {

public:
    explicit Receiver();

    // llenar la dirección del socket con la dirección de el sender
    static Packet receive_packet(int socket_fd, struct sockaddr_in &socket_address) {
    Packet packet;
    socklen_t addrlen = sizeof(socket_address);
    //Se necesitaba una prueba.
    int bytes = recvfrom(socket_fd, &packet, sizeof(packet),
                         MSG_WAITALL, (struct sockaddr *) &socket_address, &addrlen);

    if(bytes != sizeof(Packet) || bytes <= 0) {
        perror("No se recibieron todos los datos del paquete");
        //status = 0;
        //return Packet();
    }
    //status = 1;
    return packet;
}

    // llenar la dirección del socket con la dirección de el sender
    static Ack_Packet receive_ack_packet(int socket_fd, struct sockaddr_in socket_address, int& status, int TIMEOUT = 1) {
    clock_t begin = clock();
    int flags = fcntl(socket_fd, F_GETFL);
    fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
    while((clock() - begin)/ CLOCKS_PER_SEC < TIMEOUT){
        Ack_Packet ack_packet;
        socklen_t addrlen = sizeof(socket_address);
        int bytes = recvfrom(socket_fd, &ack_packet, sizeof(ack_packet),
                             0, (struct sockaddr*)&socket_address, &addrlen);
        if(bytes > 0){
            status = 1;
            return ack_packet;
        }
    }
    status = 0;
    return Ack_Packet();
}

    // llenar la dirección del socket con la dirección de el sender
    static Ack_Server_Packet receive_ack_server_packet(int socket_fd, struct sockaddr_in socket_address) {
    Ack_Server_Packet ack_server_packet;
    socklen_t addrlen = sizeof(socket_address);
    int bytes = recvfrom(socket_fd, &ack_server_packet, sizeof(ack_server_packet),
                         0, (struct sockaddr*)&socket_address, &addrlen);
    if(bytes != sizeof(Ack_Server_Packet)){
        std::perror("No se recibieron todos los datos del paquete");
    }
    return ack_server_packet;
}
};
/////////////////////////////////////////////

//#include "PacketHandler.h"
/////////////////////////////////////////////
class PacketHandler {

public:
    /* para paquete de datos */
    static Packet create_packet(char* data, int seqno, int len){
    Packet packet;
    for(int i = 0 ;i < len; i++){
        packet.data[i] = data[i];
    }
    packet.seqno = seqno;
    packet.len = len;
    packet.cksum = calculate_packet_checksum(packet);
    return packet;
}
    static uint16_t calculate_packet_checksum(Packet packet){
    uint32_t sum = 0;
    for(int i = 0; i < packet.len; i++){
        sum += packet.data[i];
    }
    sum += packet.len;
    sum += packet.seqno;
    // Agrega los acarreos
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);
    // Devuelve el complemento a uno de la suma
    return ( (uint16_t)(~sum)  );
}
    static bool compare_packet_checksum(Packet packet){
    return packet.cksum == calculate_packet_checksum(packet);
}


    /* para el paquete "ack" */
    static Ack_Packet create_ack_packet(uint32_t ackno, uint16_t len){
    Ack_Packet packet;
    packet.len = len;
    packet.ackno = ackno;
    packet.cksum = calculate_ack_packet_checksum(packet);
    return packet;
}
    static uint16_t calculate_ack_packet_checksum(Ack_Packet packet){
    uint32_t sum = 0;
    sum += packet.len;
    sum += packet.ackno;
    // Agrega los acarreos
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);
    // Devuelve el complemento a uno de la suma
    return ( (uint16_t)(~sum)  );
}
    static bool compare_ack_packet_checksum(Ack_Packet packet){
    return packet.cksum == calculate_ack_packet_checksum(packet);
}
};
/////////////////////////////////////////////

//#include "FileReader.h"
/////////////////////////////////////////////
class FileReader {

private:
    FILE *file;
    int chunk_size;
    int current_chunk_index;
    char* file_path;

public:
    explicit FileReader(string file_path, int chunk_size=CHUNK_SIZE) {
    FileReader::file = fopen(file_path.c_str(), "rb");
    file_path = file_path.c_str();
    if (!file){
        perror("Ningún archivo existente en el servidor");
    }
    FileReader::chunk_size = chunk_size;
    FileReader::current_chunk_index = 0;
}
    explicit FileReader(const char* file_path, int chunk_size=CHUNK_SIZE) {
    FileReader::file = fopen(file_path, "rb");
    file_path = file_path;
    if (!file){
        perror("Ningún archivo existente en el servidor");
    }
    FileReader::chunk_size = chunk_size;
    FileReader::current_chunk_index = 0;
}

    int get_file_size() {
    fseek(file, 0, SEEK_END);
    int x = ftell(file);
    rewind(file);
    return x;
}
    bool is_finished(){
    return ftell(file) == get_file_size();
}
    int get_current_chunk_index() {
    return current_chunk_index;
}
    Packet get_current_chunk_data() {
    if(is_finished()){
        perror("No quedan bytes para leer");
        return Packet();
    }
    current_chunk_index++;
    return get_chunk_data(current_chunk_index - 1);
}
    Packet get_chunk_data(int chunk_index) {
    int size = min(chunk_size, get_file_size() - (chunk_index*chunk_size));
    char * buffer = (char*) malloc(size);
    int bytes_readed;
    fseek(FileReader::file, chunk_index * chunk_size, SEEK_SET);
    bytes_readed = fread (buffer, sizeof(char), size, file);
    Packet packet = PacketHandler::create_packet(buffer, chunk_index, bytes_readed);
    return packet;
}
    void advance_chunk_pointer() {
    current_chunk_index++;
}
    int get_total_packet_number(){
    return ceil(1.0 * get_file_size() / FileReader::chunk_size);
}
    void close(){
    fclose(file);
}
};
/////////////////////////////////////////////

//#include "FileWriter.h"
/////////////////////////////////////////////
class FileWriter {

private:
    FILE *file;
    int chunk_size;
    int current_chunk_index;

public:
    FileWriter(string file_path, int chunk_size=CHUNK_SIZE) {
    FileWriter::file = fopen("NuevoServidor.PNG"/*"file_path.c_str()"*/, "wb");
    FileWriter::chunk_size = chunk_size;
    FileWriter::current_chunk_index = 0;
}
    FileWriter() {
}
    ~FileWriter(){
    fclose(file);
    free(this);
}

    void write_chunk(string data){
    current_chunk_index++;
    write_chunk_data(current_chunk_index - 1, data);
}
    void write_chunk_data(int chunk_index, string data) {
    fseek(FileWriter::file, chunk_index * chunk_size, SEEK_SET);
    char * buffer = strdup(data.c_str());
    fwrite(buffer, 1, sizeof(buffer), file);
}
};
/////////////////////////////////////////////

//#include "SR_Receiver.h"
/////////////////////////////////////////////
class SR_Receiver {

private:
    // socket fd
    int socket_fd;
    string file_path;
    // window
    int cwnd = 10;
    map<int, Packet> received;
    int start_window_packet;
    int end_window_packet;
    int total_packets;
    // helpers
    Sender sender;
    FileWriter writer;

public:
    explicit SR_Receiver(int socket_fd, string file_path, int total_packets): writer(file_path), sender(){
    SR_Receiver::socket_fd = socket_fd;
    SR_Receiver::file_path = file_path;
    received.clear();
    start_window_packet = 0;
    end_window_packet = min(cwnd - 1, total_packets - 1);
    SR_Receiver::total_packets = total_packets;
}

    void recevFile(){
    while(start_window_packet < total_packets){
        struct sockaddr_in socket_address;
        Packet packet = Receiver::receive_packet(socket_fd, socket_address);
        if(PacketHandler::compare_packet_checksum(packet) && packet.seqno >= start_window_packet && packet.seqno <= end_window_packet && received.find(packet.seqno) == received.end()){
            // accept the packet and send ack
            received[packet.seqno] = packet;
            Ack_Packet ack_packet = PacketHandler::create_ack_packet(packet.seqno, sizeof(int));
            sender.send_ack(ack_packet, socket_fd);
            // update the window
            int idx = start_window_packet;
            while(idx < total_packets && received.find(idx) != received.end()) idx++;
            start_window_packet = idx;
            end_window_packet = min(start_window_packet + cwnd - 1, total_packets - 1);
        }
        else{
            // ignorar
        }
    }
    // recibió todos los paquetes, luego clasifíquelos y escríbalos en el archivo
    int current_packet = 0;
    while(current_packet < total_packets){
        Packet packet = received[current_packet];
        writer.write_chunk_data(current_packet, string(packet.data, packet.len));
    }
}

};
/////////////////////////////////////////////

//#include "SR_Sender.h"
/////////////////////////////////////////////
class SR_Sender {
private:
    // socket fd
    int socket_fd;
    string file_path;
    set<int> loss_packets_indices;
    vector<int> window_changes;
    // window
    int cwnd = 1;
    map<int, pair<Packet, clock_t>> window;
    map<int, Ack_Packet> acked;
    int start_window_packet;
    int end_window_packet;
    // window handling
    int threshold = 10;
    int additive_increase = 1;
    int multiplicative_decrease = 2;
    // helpers
    Sender sender;
    FileReader reader;
    // threads and mutex
    thread sender_thread;
    thread receiver_thread;
    mutex mtx;
    struct sockaddr_in client_socket;

public:
    explicit SR_Sender(int socket_fd, string file_name, double loss_prob, int seed_number,
         struct sockaddr_in client_socket): sender(client_socket), reader(file_name){
    SR_Sender::socket_fd = socket_fd;
    SR_Sender::file_path = file_name;
    std::ifstream infile("control.txt");
    int a;
    while(infile >> a){
        window_changes.push_back(a);
    }
    get_loss_packets(loss_prob, seed_number);
    cwnd = 1;
    threshold = 10;
    additive_increase = 1;
    multiplicative_decrease = 2;
    window.clear();
    acked.clear();
    start_window_packet = 0;
    end_window_packet = 0;
    SR_Sender::client_socket = client_socket;
}

    void sendFile(){
    sender_thread = std::thread (&SR_Sender::send_handling, this);
    receiver_thread  = std::thread (&SR_Sender::recev_ack_handling, this);
    sender_thread.join();
    printf("subproceso del sender terminado en repetición selectiva.\n");
    receiver_thread.join();
    printf("subproceso receiver terminado en repetición selectiva.\n");
}

private:
    void get_loss_packets(double loss_prob, int seed_number){
    srand(seed_number);
    int total_packets = reader.get_total_packet_number();
    int total_loss_packets = ceil(loss_prob * total_packets);
    set<int> loss_packets_indices;
    for(int i = 0; i < total_loss_packets; i++){
        int index = rand() % total_packets;
        if(loss_packets_indices.count(index) == 0){
            loss_packets_indices.insert(index);
        }
        else{
            // como ese índice ya existe
            i--;
        }
    }
    SR_Sender::loss_packets_indices = loss_packets_indices;
}
    void send_handling(){
    //mtx.lock();
    int total_packets = reader.get_total_packet_number();
    int sended = 0;
    int window_congestion_index = 0;
    while(start_window_packet < total_packets){

        for(int index = start_window_packet; index <= end_window_packet; index++){
            mtx.lock();
            if(window.find(index) != window.end()){
                Packet packet = reader.get_chunk_data(index);
                // loss
                if(loss_packets_indices.count(index)){
                    loss_packets_indices.erase(index);
                    clock_t start_time = clock();
                    window[index] = {packet, start_time};
                } else if(window_congestion_index < window_changes.size() && sended == window_changes[window_congestion_index]){
                    window.clear();
                    //acked.clear();
                    sended = 0;
                    window_congestion_index++;
                    cwnd = max((int)floor(1.0 * cwnd / multiplicative_decrease), 1);
                    start_window_packet = start_window_packet;
                    end_window_packet = min(start_window_packet + cwnd - 1, total_packets - 1);
                    mtx.unlock();
                    break;
                } else{
                    sender.send_packet(packet, socket_fd);
                    clock_t start_time = clock();
                    window[index] = {packet, start_time};
                    sended++;
                }
            }
            if(acked.find(index) == acked.end()){
                clock_t time_now = clock();
                if((time_now - window[index].second)/ CLOCKS_PER_SEC >= SR_TIMEOUT){
                    // TODO manejar el tiempo de espera
                    window.clear();
                    acked.clear();
                    cwnd = 1;
                    start_window_packet = start_window_packet;
                    end_window_packet = start_window_packet;
                    mtx.unlock();
                    break;
                }
            }
            mtx.unlock();
        }
    }
}
    void recev_ack_handling(){
    //mtx.lock();
    int total_packets = reader.get_total_packet_number();
    while(start_window_packet < total_packets){
        int status;
        Ack_Packet ack_packet = Receiver::receive_ack_packet(socket_fd, client_socket, status);
        if(status == 1){
            mtx.lock();
            if(PacketHandler::compare_ack_packet_checksum(ack_packet) && window.find(ack_packet.ackno) != window.end()){
                cout << "paquete " << ack_packet.ackno << " ack." << endl;
                acked[ack_packet.ackno] = ack_packet;
                while(start_window_packet <= end_window_packet && acked.find(start_window_packet) != acked.end()){
                    start_window_packet++;
                }
                if(cwnd < threshold){
                    //exp
                    cout << "window size duplicado." << endl;
                    cwnd *= 2;
                }else if(cwnd < total_packets){
                    cout << "window size incrementado por" << additive_increase << endl;
                    cwnd += additive_increase;
                }
                end_window_packet = min(start_window_packet + cwnd - 1, total_packets - 1);
            }
            else{
                // ignorar el paquete "ack"
            }
            mtx.unlock();
        }
        else{
            // no recibo por mucho tiempo
        }
    }
    return;
}
};
/////////////////////////////////////////////

//#include "Server.h"
/////////////////////////////////////////////
class Server {

private:
    int server_port_number;
    int maximum_window_size;
    int random_seed;
    double packet_loss_prob;
    int active_clients;
    int server_socket_fd;

public:
    explicit Server(string server_conf_file_dir) {
    freopen(server_conf_file_dir.c_str(), "r", stdin);
    cin >> server_port_number >> maximum_window_size >> random_seed >> packet_loss_prob;
    active_clients = 0;
}
    void start_server() {
    init_server();
    printf("El servidor está esperando clientes\n");
        /* bucle principal del servidor */
        printf("Conectado exitosamente con un cliente\n");
        struct sockaddr_in client_address;
        // recibir el nombre del archivo
        memset(&client_address, 0, sizeof(client_address));

        Packet packet = Receiver::receive_packet(server_socket_fd, client_address);
        printf("paquete recibido.\n");
        // revisando el archivo
        cout << packet.data <<endl;
        string file_name(packet.data, packet.len);
        FileReader file_reader(file_name.c_str(), CHUNK_SIZE);

        int number_of_packets = ceil(file_reader.get_file_size() * 1.0 / CHUNK_SIZE);
        // enviando el "ack" con la cantidad de paquetes que se enviarán

        Sender sender = Sender(client_address);
        Ack_Server_Packet server_ack_packet;
        server_ack_packet.packets_numbers = number_of_packets;
        sender.send_server_ack(server_ack_packet, server_socket_fd);
        cout << "el tamaño del archivo es " << file_reader.get_file_size() << endl;
        cout << "transfiriendo " << ceil(1.0 * file_reader.get_file_size() / CHUNK_SIZE) << " packet" <<endl;
        // llamar al método deseado para enviar el archivo
        file_reader.close();
        auto start = std::chrono::high_resolution_clock::now();
        
        
        // selective repeat
        SR_Sender SR(server_socket_fd, file_name.c_str(), packet_loss_prob, random_seed, client_address);
        SR.sendFile();

        printf("Cliente terminado\n");
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Elapsed time: " << elapsed.count() << " s\n";
}

private:
    void init_server() {
    struct sockaddr_in address;
    address.sin_family = AF_INET; //IPv$
    // Crear un descriptor de archivo de socket para el servidor
    if ((server_socket_fd = socket(address.sin_family, SOCK_DGRAM, 0)) < 0) {
        perror("la creación del socket falló");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    // Verifique si se usa y conecte con fuerza el zócalo a nuestra dirección y puerto deseados
    if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt Error");
        exit(EXIT_FAILURE);
    }
    /*
     * construir la dirección de Internet del servidor
     */
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(3000);
    int ret = bind(server_socket_fd, (struct sockaddr *)&address, sizeof(address));
    if (ret < 0) {
        perror("enlace fallido");
        exit(EXIT_FAILURE);
    }
    printf("vincular a la dirección 0.0.0.0\n");
}
};
/////////////////////////////////////////////

int main() {
    Server server("server.txt");
    
    server.start_server();
    return 0;
}