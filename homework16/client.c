#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define SRV_ADDRESS "127.0.0.1"
#define SRV_PORT 48888
#define CNT_PORT 48889
#define PACKET_SIZE 128
#define PAYLOAD_SIZE 64

void main()
{
    /* инициализация структур сервера и клиента */
    struct sockaddr_in serv, clnt;
    /* создаем сокет */
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (fd == -1)           
    {
        perror("socket fd");
        exit(EXIT_FAILURE);
    }
    /* заполняем структуру информации о сокете сервера */                    
    memset(&serv, 0, sizeof(struct sockaddr_in));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(SRV_PORT);
    serv.sin_addr.s_addr = inet_addr(SRV_ADDRESS);
    /* инициализируем структуру L4 пакета */ 
    char packet[PACKET_SIZE];
    memset(&packet, 0, PACKET_SIZE);
    struct udphdr *l4header = (struct udphdr *)packet;
    char *l4payload = packet + sizeof(struct udphdr);
    /* заполняем заголовки L4 */
    l4header->source = htons(CNT_PORT);
    l4header->dest = htons(SRV_PORT);
    l4header->len = htons(PACKET_SIZE);
    l4header->check = 0;
    /* помещаем сообщение в payload */
    strncpy(l4payload, "Hello!\n", PACKET_SIZE);
    /* структура клиента */
    memset(&clnt, 0, sizeof(struct sockaddr_in));
    socklen_t len = sizeof(struct sockaddr_in);
    /* отправка */
    if(sendto(
                fd,
                packet,
                PACKET_SIZE,
                MSG_WAITALL,
                (struct sockaddr *)&serv,
                len
                ) == -1)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }
    /* структура полученного пакета */
    struct udphdr* recvl4header = (struct udphdr *)(packet + sizeof(struct iphdr));
    char* recvpayload = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
    /* получение */
    while(1)
    {
        if(recvfrom(
                    fd,
                    packet,
                    PACKET_SIZE,
                    MSG_WAITALL,
                    (struct sockaddr *)&clnt,
                    &len
                    ) == -1)
        {
            perror("receive");
        }
        if(recvl4header->source == htons(SRV_PORT))
        {
            break;
        }
    }
    /* из полученного пакета достаем payload */
    fputs(recvpayload, stdout);
    close(fd);
}
