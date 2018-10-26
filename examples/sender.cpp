/*
 * Snootlab 
 *-----------------------------------------------------------------------------------------
 * Example of a communication between two Chistera-Pi or between a Chistera-Pi and a sensor
 * with a RFM95 like Snootlab's TeensyWiNo.
 * This is the sender code, the Chistera-Pi sends a message with its address and a counter.
 * The message is sent every 3 seconds.
 *
 * usage: ./<software_name> <address>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ctime>
#include <iostream>
#include <iomanip>

#include <RH_RF95.h>

RH_RF95 rf95;

/* The address of the node which is 10 by default */
uint8_t node_number = 10;
uint8_t msg[2] = {10, 0};
int run = 1;

/* Send a message every 3 seconds */
void sigalarm_handler(int signal)
{
    msg[0] = node_number;
    msg[1]++;
 
    rf95.send(msg, sizeof(msg));
    rf95.waitPacketSent();

    // Log sending time
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    std::cout << '['
              << (now->tm_year + 1900) << '-'
              << (now->tm_mon + 1) << '-'
              <<  now->tm_mday << ' '
              <<  now->tm_hour << ':'
              << std::setfill('0') <<  std::setw(2)
              << now->tm_min << ':'
              << std::setfill('0') <<  std::setw(2)
              << now->tm_sec << "] from "
              <<  unsigned(msg[1]) << " from "
              <<  unsigned(msg[0]) << "\n";

    alarm(3);
}

/* Signal the end of the software */
void sigint_handler(int signal)
{
    run = 0;
}

void setup()
{ 
    wiringPiSetupGpio();

    if (!rf95.init()) 
    {
        fprintf(stderr, "Init failed\n");
        exit(1);
    }

    /* Tx power is from +5 to +23 dBm */
    rf95.setTxPower(23);
    /* There are different configurations
     * you can find in lib/radiohead/RH_RF95.h 
     * at line 437 
     */
    rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128);
    rf95.setFrequency(868.0); /* Mhz */
}

void loop()
{
}

int main(int argc, char **argv)
{
    std::cout << "Sender example\n";
    if( argc == 2 )
        node_number = atoi(argv[1]);
    
    signal(SIGINT, sigint_handler);
    signal(SIGALRM, sigalarm_handler);

    alarm(3);

    setup();

    while( run )
    {
        loop();
        usleep(1);
    }

    return EXIT_SUCCESS;
}
