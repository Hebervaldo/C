#include "getipv4.h"

#define inaddrr(x) (*(struct in_addr *) &ifr->x[sizeof sa.sin_port])
#define IFRSIZE ((int)(size * sizeof (struct ifreq)))

#define cntNumeroCaracteres 200
#define cntNumeroInterface 16

int intNumeroInterface = 0;

typedef struct
{
    char Interface[cntNumeroCaracteres];
    char IP_Address[cntNumeroCaracteres];
    char HW_Address[cntNumeroCaracteres];
    char Netmask[cntNumeroCaracteres];
    char Broadcast[cntNumeroCaracteres];
    char MTU[cntNumeroCaracteres];
    char Metric[cntNumeroCaracteres];
}
stcInformacoesInterfaceRede;

stcInformacoesInterfaceRede objInformacoesInterfaceRede[cntNumeroInterface];

void uinttochar(char* a, unsigned int n)
{
    if (n == 0)
    {
        *a = '0';
        *(a+1) = '\0';

        return;
    }

    char aux[20];
    aux[19] = '\0';
    char* auxp = aux + 19;

    int c = 1; 

    while (n != 0)
    {
        int mod = n % 10;
        *(--auxp) = mod | 0x30;
        n /=  10;
        c++;
    }

    memcpy(a, auxp, c);
}

static char *get_addr(int sock, char * ifname, struct sockaddr * ifaddr)
{
    struct ifreq *ifr;
    struct ifreq ifrr;
    struct sockaddr_in sa;

    ifr = &ifrr;
    ifrr.ifr_addr.sa_family = AF_INET;
    strncpy(ifrr.ifr_name, ifname, sizeof(ifrr.ifr_name));

    if (ioctl(sock, SIOCGIFADDR, ifr) < 0)
    {
        // printf("No %s interface.\n", ifname);
        // return -1;
        return "No interface.";
    }

    *ifaddr = ifrr.ifr_addr;
    // printf("Address for %s: %s\n", ifname, inet_ntoa(inaddrr(ifr_addr.sa_data)));
    
    return inet_ntoa(inaddrr(ifr_addr.sa_data));
}

void mtdGerarVetorEstruturaInformacoesInterfaceRede()
{
    intNumeroInterface = 0;
    
    unsigned char *u;
    int sockfd, size = 1;
    struct ifreq *ifr;
    struct ifconf ifc;
    struct sockaddr_in sa;

    if (0 > (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)))
    {
        fprintf(stderr, "Cannot open socket.\n");
        exit(EXIT_FAILURE);
    }

    ifc.ifc_len = IFRSIZE;
    ifc.ifc_req = NULL;

    do
    {
        ++size;
        /* realloc buffer size until no overflow occurs */
        if (NULL == (ifc.ifc_req = realloc(ifc.ifc_req, IFRSIZE)))
        {
            fprintf(stderr, "Out of memory.\n");
            exit(EXIT_FAILURE);
        }
        ifc.ifc_len = IFRSIZE;

        if (ioctl(sockfd, SIOCGIFCONF, &ifc))
        {
            perror("ioctl SIOCFIFCONF");
            exit(EXIT_FAILURE);
        }
    }
    while (IFRSIZE <= ifc.ifc_len);

    /* this is an alternate way to get info... */
    {
        // struct sockaddr ifa;
        // get_addr(sockfd, "ppp0", &ifa);
        // printf("Address for %s: %s\n", "ppp0", get_addr(sockfd, "ppp0", &ifa));
    }

    ifr = ifc.ifc_req;
    
    for (;(char *) ifr < (char *) ifc.ifc_req + ifc.ifc_len; ++ifr)
    {
        if (ifr->ifr_addr.sa_data == (ifr+1)->ifr_addr.sa_data)
        {
            continue; /* duplicate, skip it */
        }

        if (ioctl(sockfd, SIOCGIFFLAGS, ifr))
        {
            continue; /* failed to get flags, skip it */
        }

        // printf("Interface: %s\n", ifr->ifr_name);
        sprintf(objInformacoesInterfaceRede[intNumeroInterface].Interface, "%s", ifr->ifr_name);
        // printf("IP Address: %s\n", inet_ntoa(inaddrr(ifr_addr.sa_data)));
        sprintf(objInformacoesInterfaceRede[intNumeroInterface].IP_Address, "%s", inet_ntoa(inaddrr(ifr_addr.sa_data)));

        /*
        This won't work on HP-UX 10.20 as there's no SIOCGIFHWADDR ioctl. You'll
        need to use DLPI or the NETSTAT ioctl on /dev/lan0, etc (and you'll need
        to be root to use the NETSTAT ioctl. Also this is deprecated and doesn't
        work on 11.00).

        On Digital Unix you can use the SIOCRPHYSADDR ioctl according to an old
        utility I have. Also on SGI I think you need to use a raw socket, e.g. s
        = socket(PF_RAW, SOCK_RAW, RAWPROTO_SNOOP)

        Dave

        From: David Peter <dave....@eu.citrix.com>
        */

        if (0 == ioctl(sockfd, SIOCGIFHWADDR, ifr)) 
        {
            /* Select which hardware types to process.
            *
            * See list in system include file included from
            * /usr/include/net/if_arp.h (For example, on
            * Linux see file /usr/include/linux/if_arp.h to
            * get the list.)
            */
            switch (ifr->ifr_hwaddr.sa_family) 
            {
                default:
                    // printf("\n");

                    continue;
                case ARPHRD_NETROM:
                case ARPHRD_ETHER:
                case ARPHRD_PPP:
                case ARPHRD_EETHER:
                case ARPHRD_IEEE802:
                    break;
            }

            u = (unsigned char *) &ifr->ifr_addr.sa_data;

            if (u[0] + u[1] + u[2] + u[3] + u[4] + u[5]) 
            {
                // printf("HW Address: %2.2x.%2.2x.%2.2x.%2.2x.%2.2x.%2.2x\n",
                // u[0], u[1], u[2], u[3], u[4], u[5]);
                sprintf(objInformacoesInterfaceRede[intNumeroInterface].HW_Address, "%2.2x.%2.2x.%2.2x.%2.2x.%2.2x.%2.2x", u[0], u[1], u[2], u[3], u[4], u[5]);
            }
        }

        if (0 == ioctl(sockfd, SIOCGIFNETMASK, ifr) && strcmp("255.255.255.255", inet_ntoa(inaddrr(ifr_addr.sa_data)))) 
        {
            // printf("Netmask: %s\n", inet_ntoa(inaddrr(ifr_addr.sa_data)));
            sprintf(objInformacoesInterfaceRede[intNumeroInterface].Netmask, "%s", inet_ntoa(inaddrr(ifr_addr.sa_data)));
        }

        if (ifr->ifr_flags & IFF_BROADCAST)
        {
            if (0 == ioctl(sockfd, SIOCGIFBRDADDR, ifr) && strcmp("0.0.0.0", inet_ntoa(inaddrr(ifr_addr.sa_data)))) 
            {
                // printf("Broadcast: %s\n", inet_ntoa(inaddrr(ifr_addr.sa_data)));
                sprintf(objInformacoesInterfaceRede[intNumeroInterface].Broadcast, "%s", inet_ntoa(inaddrr(ifr_addr.sa_data)));
            }
        }

        if (0 == ioctl(sockfd, SIOCGIFMTU, ifr))
        {
            // printf("MTU: %u\n", ifr->ifr_mtu);
            uinttochar(objInformacoesInterfaceRede[intNumeroInterface].MTU, ifr->ifr_mtu);
        }

        if (0 == ioctl(sockfd, SIOCGIFMETRIC, ifr))
        {
            // printf("Metric: %u\n", ifr->ifr_metric);
            uinttochar(objInformacoesInterfaceRede[intNumeroInterface].Metric, ifr->ifr_metric);
        }
       
        intNumeroInterface++;
    }

    close(sockfd);
}

int getNumeroInterface()
{
    return intNumeroInterface;
}

char *mtdObterNomeInterface(int NumeroInterface)
{
    return objInformacoesInterfaceRede[NumeroInterface].Interface;
}

char *mtdObterEnderecoIPInterface(int NumeroInterface)
{
    return objInformacoesInterfaceRede[NumeroInterface].IP_Address;
}

char *mtdObterEnderecoHWInterface(int NumeroInterface)
{
    return objInformacoesInterfaceRede[NumeroInterface].HW_Address;
}

char *mtdObterNetmaskInterface(int NumeroInterface)
{
    return objInformacoesInterfaceRede[NumeroInterface].Netmask;
}

char *mtdObterBroadcastInterface(int NumeroInterface)
{
    return objInformacoesInterfaceRede[NumeroInterface].Broadcast;
}

char *mtdObterMTUInterface(int NumeroInterface)
{
    return objInformacoesInterfaceRede[NumeroInterface].MTU;
}

char *mtdObterMetricInterface(int NumeroInterface)
{
    return objInformacoesInterfaceRede[NumeroInterface].Metric;
}
