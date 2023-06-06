#include <iostream>

using namespace std;

#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory>
#include <ctime>
#include <chrono>
#include <cstdint>
#include <string>
#include <fcntl.h>
#include <cmath>
#include <stdio.h>
#include <set>
#include <map>


//#include "constants.h"
///////////////////////////////////////////
#define CHUNK_SIZE 20000
#define STOP_AND_WAIT_TIMEOUT 5
#define SR_TIMEOUT 5
///////////////////////////////////////////

//#include "Packet.h"
///////////////////////////////////////////
/* Paquetes de solo datos*/
struct Packet {
    /* Header */
    uint16_t cksum; /* Parte de bonificación opcional */
    uint16_t len;
    uint32_t seqno;
    /* Data */
    char data[CHUNK_SIZE]; /* No siempre 500 bytes, puede ser menos */
};

/* Los paquetes solo "Ack" son solo 8 bytes */
struct Ack_Packet {
    uint16_t cksum; /* Parte de bonificación opcional */
    uint16_t len;
    uint32_t ackno;
};

/* "Ack" del servidor, incluido el número de paquetes del archivo deseado */
struct Ack_Server_Packet {
    uint32_t packets_numbers;
};
///////////////////////////////////////////

//#include "Sender.h"
///////////////////////////////////////////
class Sender {

private:
    struct sockaddr_in socket_address;

public:
    explicit Sender(struct sockaddr_in socket_addres) {
    Sender::socket_address = socket_addres;
}
    explicit Sender() {

}

    void send_packet(Packet packet, int socket_fd) {
    sendto(socket_fd, &packet, sizeof(packet),
            MSG_CONFIRM,
            (const struct sockaddr*)&socket_address,
                    sizeof(socket_address));
}
    void send_ack(Ack_Packet ack_packet, int socket_fd) {
    sendto(socket_fd, (void*) &ack_packet, sizeof(ack_packet), MSG_CONFIRM, (struct sockaddr*)&socket_address, sizeof(socket_address));
}
    void send_server_ack(Ack_Server_Packet ack_server_packet, int socket_fd) {
    sendto(socket_fd, (void*) &ack_server_packet, 
            sizeof(ack_server_packet), MSG_CONFIRM, 
            (struct sockaddr*)&socket_address, 
                sizeof(socket_address));
}
};
///////////////////////////////////////////

//#include "Receiver.h"
///////////////////////////////////////////
class Receiver {

public:
    explicit Receiver();

    // complete la dirección del socket con la dirección del sender
    static Packet receive_packet(int socket_fd, struct sockaddr_in socket_address) {
    Packet packet;
    socklen_t addrlen = sizeof(socket_address);
    //make it a blocking for some time only.
    int bytes = recvfrom(socket_fd, &packet, sizeof(packet),
                         MSG_WAITALL, (struct sockaddr *) &socket_address, &addrlen);
    if(bytes != sizeof(Packet) || bytes <= 0) {
        perror("No se recibieron todos los datos del paquete");
        return Packet();
    }
    printf("paquete %d recibido.\n", packet.seqno);
    cout << "el tamaño del paquete es " << packet.len << endl;
    return packet;
}
    
    // complete la dirección del socket con la dirección del sender
    static Ack_Packet receive_ack_packet(int socket_fd, struct sockaddr_in socket_address, int& status, int TIMEOUT = 1000) {

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
    
    // complete la dirección del socket con la dirección del sender
    static Ack_Server_Packet receive_ack_server_packet(int socket_fd, struct sockaddr_in socket_address) {
    Ack_Server_Packet ack_server_packet;
    socklen_t addrlen = sizeof(socket_address);
    int bytes = recvfrom(socket_fd, &ack_server_packet, sizeof(ack_server_packet),
                         MSG_WAITALL, (struct sockaddr*)&socket_address, &addrlen);
    if(bytes != sizeof(Ack_Server_Packet)){
        std::perror("No se recibieron todos los datos del paquete");
    }
    printf("Ack recibido.\n");
    return ack_server_packet;
}
};
///////////////////////////////////////////

//#include "PacketHandler.h"
///////////////////////////////////////////
class PacketHandler {

public:
    /* para paquete de datos */
    static Packet create_packet(char* data, int seqno, int len){
    Packet packet;
    strcpy(packet.data, data);
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
///////////////////////////////////////////

//#include "FileWriter.h"
///////////////////////////////////////////
class FileWriter {

private:
    FILE *file;
    int chunk_size;
    int current_chunk_index;

public:
    FileWriter(string file_path, int chunk_size=CHUNK_SIZE) {
    //FileWriter::file = fopen("NuevoCliente.txt"/*"file_path.c_str()"*/, "wa+");
    string aux = "Copia_" + file_path;
    FileWriter::file = fopen(aux.c_str(), "wa+");
    FileWriter::chunk_size = chunk_size;
    FileWriter::current_chunk_index = 0;
}
    FileWriter() {
}

    void write_chunk(string data){
    current_chunk_index++;
    write_chunk_data(current_chunk_index - 1, data);
}
    void write_chunk_data(int chunk_index, string data) {
    //fseek(FileWriter::file, chunk_index * chunk_size, SEEK_SET);
    cout  << "client wrote "<< data.size()
            << " char" << " as chunk " << chunk_index * chunk_size << endl;
    fwrite(data.c_str(), sizeof(char), data.size(), file);
}
};
///////////////////////////////////////////



//#include "SR_Receiver.h"
///////////////////////////////////////////
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
    struct sockaddr_in server_addr;

public:
    explicit SR_Receiver(int socket_fd, string file_path, int total_packets, struct sockaddr_in server_addr): writer(file_path), sender(server_addr){
    SR_Receiver::socket_fd = socket_fd;
    SR_Receiver::file_path = file_path;
    received.clear();
    start_window_packet = 0;
    end_window_packet = min(cwnd - 1, total_packets - 1);
    SR_Receiver::total_packets = total_packets;
    SR_Receiver::server_addr = server_addr;
}

    void recevFile(){
    while(start_window_packet < total_packets){
        Packet packet = Receiver::receive_packet(socket_fd, server_addr);
        if(PacketHandler::compare_packet_checksum(packet) && packet.seqno >= start_window_packet && packet.seqno <= end_window_packet && received.find(packet.seqno) == received.end()){
            // aceptar el paquete y enviar "ack"
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
    // recibió todos los paquetes, luego los clasificó y los escribió en el archivo    int current_packet = 0;
    int current_packet = 0;
    while(current_packet < total_packets){
        Packet packet = received[current_packet];
        writer.write_chunk_data(current_packet, string(packet.data, packet.len));
        current_packet++;
    }
}
};
///////////////////////////////////////////


//#include "Client.h"
///////////////////////////////////////////
class Client {

private:
    string server_ip_address;
    int server_port_number;
    int client_port_number;
    string requested_file_name;
    int initial_window_size;
    int sock_fd;
    struct sockaddr_in serv_address;

public:
    explicit Client(string client_conf_file_dir) {
    ifstream conf_file(client_conf_file_dir);
    if (conf_file) {
        conf_file >> server_ip_address >> server_port_number;
        conf_file >> client_port_number;
        conf_file >> initial_window_size;
    } else {
        perror("Archivo de configuración no encontrado !!\n");
        exit(EXIT_FAILURE);
    }
}


    void connect_to_server() {
    init_client_socket();
    memset(&serv_address, 0, sizeof(serv_address));

    //  Relleno de información del servidor
    serv_address.sin_family = AF_INET;
    serv_address.sin_port = htons(3000);
    serv_address.sin_addr.s_addr = INADDR_ANY;
    printf("El cliente está listo.\n");

}
    void receive_file(string& name) {
    requested_file_name = name;
    uint32_t number_of_packets = send_request_to_server();
    cout << "debe recibir " << number_of_packets << " paquetes" << endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    // selective repeat
    SR_Receiver SR(sock_fd, requested_file_name, number_of_packets, serv_address);
    SR.recevFile();
    
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Tiempo transcurrido: " << elapsed.count() << " s\n";
    printf("Cliente termino\n");
}

private:
    void init_client_socket() {
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("la creación del socket falló");
        exit(EXIT_FAILURE);
    }
}
    uint32_t send_request_to_server() {
    printf("Empezar a enviar req.\n");
    // send the file name packet
    Packet packet = PacketHandler::create_packet(strdup(requested_file_name.c_str()), 0 , requested_file_name.size());
    Sender sender = Sender(serv_address);
    sender.send_packet(packet, sock_fd);
    printf("Req enviado.\n");
    /*
     * wait until receive ack from server
     */
    Ack_Server_Packet server_ack_packet =  Receiver::receive_ack_server_packet(sock_fd, serv_address);
    printf("Ack recibido.\n");
    return server_ack_packet.packets_numbers;
}
};
///////////////////////////////////////////


int main() {

    Client client("client.txt");
    client.connect_to_server();

    string name;
    cout<< "Ingresar Nombre del Archivo: ";
    cin>>name;

    cout<<endl;
    
    client.receive_file(name);
    return 0;
}