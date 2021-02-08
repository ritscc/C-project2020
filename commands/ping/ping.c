#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

typedef struct
{
    int c;
}Option;

char* name_solve(char* argv);

int make_raw_socket();

uint16_t checksum(unsigned short* buf, int size);

uint16_t set_id();

void set_icmphdr(uint8_t type, uint8_t code, uint16_t id, uint16_t seq, struct icmphdr* icmphdr);

void sender(int soc, struct icmphdr* icmphdr, struct sockaddr_in addr);

void recever(int soc, char* buf, clock_t start_time, char* argv, uint16_t id, int seq);

void check_option(char* argv[], int argc, Option* opt);

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Usage: ping IP_ADDRESS");
        exit(EXIT_FAILURE);
    }

    Option *opt, ins_opt;
    opt = &ins_opt;

    check_option(argv, argc, opt);

    char* host = argv[1];
    char* ip_addr = name_solve(host);

    int soc = make_raw_socket();

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_addr);

    uint16_t id = set_id();

    int count = 0;
    while (count < opt->c) {
        clock_t start_time;
        start_time = clock();

        struct icmphdr icmphdr;

        uint16_t seq = count;

        set_icmphdr(ICMP_ECHO, 0, id, seq, &icmphdr);

        sender(soc, &icmphdr, addr);

        char buf[2000];
        memset(buf, 0, sizeof(buf));

        recever(soc, buf, start_time, argv[1], id, seq);

        count++;
    }

    close(soc);
}

char* name_solve(char* argv) {
    struct addrinfo *hints, *res, ins_hints;
    struct in_addr addr;

    memset(&ins_hints, 0, sizeof(ins_hints));
    hints = &ins_hints;

    hints->ai_socktype = SOCK_STREAM;
    hints->ai_family = AF_INET;

    int e = getaddrinfo(argv, NULL, hints, &res);
    if (e != 0) {
        perror("failed address infomation");
        exit(EXIT_FAILURE);
    }

    addr.s_addr = ((struct sockaddr_in*)(res->ai_addr))->sin_addr.s_addr;
    freeaddrinfo(res);

    return inet_ntoa(addr);
}

int make_raw_socket() {
    int s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (s < 0) {
        perror("failed make socket");
        exit(EXIT_FAILURE);
    }

    return s;
}

uint16_t set_id() {
    pid_t p = getpid();
    return (uint16_t)p;
}

uint16_t checksum(unsigned short* buf, int size) {

    unsigned long sum = 0;

    while (size > 1) {
        sum += *buf;
        buf++;
        size -= 2;
    }

    if (size == 1)
        sum += *(unsigned char*)buf;

    sum = (sum & 0xffff) + (sum >> 16);
    sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}

void set_icmphdr(uint8_t type, uint8_t code, uint16_t id, uint16_t seq, struct icmphdr* icmphdr) {

    memset(icmphdr, 0, sizeof(struct  icmphdr));

    icmphdr->type = type;
    icmphdr->code = code;
    icmphdr->checksum = 0;
    icmphdr->un.echo.id = id;
    icmphdr->un.echo.sequence = seq;

    icmphdr->checksum = checksum((unsigned short*)icmphdr, sizeof(struct icmphdr));
}

void sender(int soc, struct icmphdr* icmphdr, struct sockaddr_in addr) {
    int n = sendto(soc, (char*)icmphdr, sizeof(icmphdr), 0, (struct sockaddr*)&addr, sizeof(addr));
    if (n < 0) {
        perror("failed sendto");
        exit(EXIT_FAILURE);
    }
}

void recever(int soc, char* buf, clock_t start_time, char* argv, uint16_t id, int seq) {
    int n = recv(soc, buf, sizeof(buf), 0);
    if (n < 0) {
        perror("failed receave");
        exit(EXIT_FAILURE);
    }

    struct iphdr* recv_iphdr;
    recv_iphdr = (struct iphdr*)buf;

    struct in_addr inaddr;
    inaddr.s_addr = recv_iphdr->saddr;

    struct icmphdr* recv_icmphdr;
    recv_icmphdr = (struct icmphdr*)(buf + (recv_iphdr->ihl << 2));

    clock_t end_time;
    end_time = clock();

    double result_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    if (recv_icmphdr->type == ICMP_ECHOREPLY) {
        printf("icmp echo reply from %s\ntime:%f[s]\n", argv, result_time);
    } else {
        printf("packet loss\n");
    }
}

void check_option(char* argv[], int argc, Option* opt) {
    if (argc < 4) {
        opt->c = 1;
    } else if (!strcmp(argv[2], "-c") && argc >= 4) {
        opt->c = atoi(argv[3]);
    } else {
        opt->c = 1;
    }
}