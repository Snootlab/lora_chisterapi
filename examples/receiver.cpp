/*
 * Snootlab 
 *-----------------------------------------------------------------------------------------
 * Example of a communication between two Chistera-Pi or between a Chistera-Pi and a sensor
 * with a RFM95 like Snootlab's TeensyWiNo.
 * This is the receiver code, the Chistera-Pi receives a message with an address and a 
 * counter and it prints the informations on the prompt.
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

int run = 1;

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

    /* Tx Power is from +5 to +23 dbm */
    rf95.setTxPower(23);
    /* There are different configurations
     * you can find in lib/radiohead/RH_RF95.h 
     * at line 437 
     */
    rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128);
    rf95.setFrequency(868.0); /* MHz */
}

void loop()
{

    /* If we receive one message we show on the prompt
     * the address of the sender and the Rx power.
     */
    if( rf95.available() ) 
    {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len)) 
        {

            // Log receiving time
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
                      <<  unsigned(buf[0]) << ", number "
                      <<  unsigned(buf[1]) << "\n";
        }
    }
}

int main(int argc, char **argv)
{ 
    std::cout << "Receiver example\n";
    signal(SIGINT, sigint_handler);

    setup();

    while( run )
    {
        loop();
        usleep(1);
    }

    return EXIT_SUCCESS;
}
