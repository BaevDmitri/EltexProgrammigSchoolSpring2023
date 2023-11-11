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
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <net/if.h>

#define SRV_L2ADDRESS "08:00:27:9b:22:34"
#define CLT_L2ADDRESS "74:12:b3:59:27:03"
#define SRV_ADDRESS "192.168.1.4"
#define CLT_ADDRESS "192.168.1.5"
#define SRV_PORT 48888
#define CNT_PORT 64999
#define PACKET_SIZE 128
#define PAYLOAD_SIZE 86

unsigned short l3chksum(unsigned short *header, int len) {
    int sum = 0;
    short tmp;
    unsigned short *ptr = header;
    unsigned short chksum;
    for(int i = 0; i < (len / 2); i++){
      sum += *ptr;
      ptr++;
    }
    tmp = (sum >> 16);
    chksum =  ~((sum & 0xffff) + tmp);
    return chksum;
}

void main()
{
    /* инициализация структур сервера и клиента */
    struct sockaddr_ll serv;
    struct sockaddr_in clnt;
    /* создаем сокет */
    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (fd == -1)           
    {
        perror("socket fd");
        exit(EXIT_FAILURE);
    }
    /* инициализируем структуру пакета */
    char packet[PACKET_SIZE];
    memset(&packet, 0, PACKET_SIZE);
    /* формируем структуру пакета */
    struct ether_header *l2header = (struct ether_header*)packet; 
    struct iphdr *l3header = (struct iphdr *)(packet + sizeof(struct ether_header));
    struct udphdr *l4header = (struct udphdr *)(packet + sizeof(struct ether_header) + sizeof(struct iphdr));
    char *l4payload = packet + 
                        sizeof(struct ether_header) +
                        sizeof(struct iphdr) +
                        sizeof(struct udphdr);
    /* преобразуем МАС-адреса в массив побайтно */
    sscanf(
            SRV_L2ADDRESS,
            "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
            &l2header->ether_dhost[0],
            &l2header->ether_dhost[1],
            &l2header->ether_dhost[2],
            &l2header->ether_dhost[3],
            &l2header->ether_dhost[4],
            &l2header->ether_dhost[5]
            );
    sscanf(
            CLT_L2ADDRESS,
            "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
            &l2header->ether_shost[0],
            &l2header->ether_shost[1],
            &l2header->ether_shost[2],
            &l2header->ether_shost[3],
            &l2header->ether_shost[4],
            &l2header->ether_shost[5]
            );
    /* дозаполняем заголовки L2 */
    l2header->ether_type = htons(ETH_P_IP);
    /* заполняем структуру информации о сокете сервера */                    
    memset(&serv, 0, sizeof(struct sockaddr_ll));
    serv.sll_family = AF_PACKET; /* семейство сокета */
    serv.sll_protocol = htons(ETH_P_ALL); /* протокол физического уровня */
    serv.sll_ifindex = if_nametoindex("wlp2s0"); /* номер выходного интерфейса */
    /* тип пакета по типу трафика, тут: пакет до хоста, т.е. юникаст */
    serv.sll_pkttype = PACKET_HOST;
    serv.sll_halen = 6; /* длина адреса */
    //memcpy(serv.sll_addr, l2header->ether_dhost, ETH_ALEN);
    strncpy(serv.sll_addr, l2header->ether_dhost, 6); /* MAC получателя */

    /* заполняем заголовки L3 */
    l3header->ihl = 5; /* Длина Заголовка Internet, минимальный -- 5 байт */
    l3header->version = 4; /* Версия IP, в нашем случае IPv4 */
    l3header->tos = 0; /* ToS -- метка сервиса, используемая в QoS */
    /* расчет и заполнение общей длины пакета */ 
    int totallength = PAYLOAD_SIZE + sizeof(struct iphdr) + sizeof(struct udphdr);
    l3header->tot_len = htons(totallength);
    /* id пакета, используется для индентификации группы */
    l3header->id = 0;
    l3header->frag_off = 0; /* Флаг фрагментации */
    l3header->ttl = 64; /* TTL -- время жизни пакета */
    /* Тип протокола, данные которого содержатся в L3-payload */
    l3header->protocol = 17;
    /* IP -- адреса источника и назначения */
    l3header->daddr = inet_addr(SRV_ADDRESS);
    l3header->saddr = inet_addr(CLT_ADDRESS);
    /* L3 чек-сумма, заполняем из функции вычисления чек-суммы 
     * по полностью заполненному IP-заголовку */
    l3header->check = 0;
    l3header->check = l3chksum((unsigned short *)l3header, sizeof(struct iphdr));
    /* заполняем заголовки L4 */
    l4header->source = htons(CNT_PORT);
    l4header->dest = htons(SRV_PORT);
    l4header->len = htons(sizeof(struct udphdr) + PAYLOAD_SIZE);
    l4header->check = 0;
    /* помещаем сообщение в payload */
    strncpy(l4payload, "Hello!\n", PACKET_SIZE);
    /* структура клиента */
    memset(&clnt, 0, sizeof(struct sockaddr_in));
    socklen_t len = sizeof(clnt);
    /* отправка */
    if(sendto(
                fd,
                packet,
                PACKET_SIZE,
                0,
                (struct sockaddr *)&serv,
                sizeof(serv)
                ) == -1)
    {
        perror("send");
        exit(EXIT_FAILURE);
    }
    /* структура полученного пакета */
    //struct udphdr* recvl4header = (struct udphdr *)(packet + sizeof(struct iphdr));
    //char* recvpayload = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
    /* получение */
    while(1)
    {
        if(recvfrom(
                    fd,
                    packet,
                    PACKET_SIZE,
                    0,
                    (struct sockaddr *)&clnt,
                    &len
                    ) == -1)
        {
            perror("receive");
        }
        if(l4header->source == htons(SRV_PORT))
        {
            break;
        }
    }
    /* из полученного пакета достаем payload */
    printf("%s", l4payload);
    close(fd);
}
