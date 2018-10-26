/*
 * Snootlab
 *-----------------------------------------------------------------------------------------
 * Example of a communication between two Chistera-Pi or between a Chistera-Pi and a sensor
 * with a RFM95 like Snootlab's TeensyWiNo.
 *
 * This receiver gets input from LoRa and pushes it it a local Unix Domain Socket.
 *
 * usage: ./<software_name> <path_name>
 *
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <RH_RF95.h>

RH_RF95 rf95;

/* Setup the hardware */

void setup()
{
    wiringPiSetupGpio();

    if (!rf95.init())
    {
        fprintf(stderr, "Init failed\n");
        exit(1);
    }

    /* Tx Power is from +5 to +23 dbm */
    rf95.setTxPower(23);
    /* There are different configurations
     * you can find in lib/radiohead/RH_RF95.h
     * at line 437
     */
    rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128);
    rf95.setFrequency(868.0); /* MHz */
}

/* Push to the socket what is received wirelessly */

int loop(char *name)
{
    int sock;
    struct sockaddr_un server;

    /* process inbound message, if any */

    if( rf95.available() )
    {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len))
        {

            /* push it to socket */

            char msg[len+1];
            for(int i=0; i<len; i++) { msg[i] = (char)buf[i]; }
            msg[len] =  0x00;

            printf("--> %s\n", msg);

            sock = socket(AF_UNIX, SOCK_STREAM, 0);
            if (sock < 0)
            {
                perror("socket");
                return 1;
            }
            server.sun_family = AF_UNIX;
            strcpy(server.sun_path, name);

            if (connect(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0)
            {
                close(sock);
                perror("connect");
                return 1;
            }

            write(sock, (char*)buf, len);
            printf("written to socket\n");

            close(sock);

          }
    }
    return 0;
}

/* Should we run or stop */

int run = 1;

void sigint_handler(int signal)
{
    run = 0;
}

/* Set everything and start processing */

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("usage:%s <path_name>", argv[0]);
        exit(1);
    }
    printf("using socket %s\n", argv[1]);

    signal(SIGINT, sigint_handler);

    printf("radio initialisation\n");
    setup();

    printf("receiving radio packets until you kill the process\n");
    while( run )
    {
        loop(argv[1]);
        usleep(10000);
    }

    return EXIT_SUCCESS;
}
