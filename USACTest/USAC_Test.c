/**
 * \file
 *	A simple USAC implementation for adaptive sampling
 *	Using a inbuild simulated array of values (no real sensor output)
 * \author
 *	Alexander Timmermans
 */

#include "contiki.h"
#include "sys/etimer.h"

#include <stdio.h>
#include <math.h>

#define MAXDATAVALUE 1.0f
#define MAXSAMPLERATE 20.0f // 20 times per minute
#define MINSAMPLERATE 1.0f // 1 time per minute
#define NBSAMPLES 25


/* Array to simulate data*/
static int sensReadArr[NBSAMPLES] = {100,100,99,99,99,97,97,97,92,92,92,92,89,89,89,89,88,87,86,81,76,72,68,62,58};


/* Array of sampled values */
static int readVal[25];

/* Adaptive Sampling parameters */
static int time = 0;
static int sampleNb = 0;
static int valueOfData = 0;
static float confidenceInterval[25];
static double sampleRate = MAXSAMPLERATE;
static float alpha = 0.75f;

/* Confidence Param*/
static float allowedDelta = 2.0f;


// Return max of 2 values
int max(int a, int b)
{
	return a > b ? a : b;
}

// Take a sample and return value
int takeSample()
{
	static int sCounter = 0;
	
	if(sCounter >= NBSAMPLES)
		return 0;
	
	readVal[sampleNb] = sensReadArr[sCounter++];
	return 1;
}

// Calculate confidence Interval
void calculateConfInt(int time)
{
	//confidenceInterval[time] = fabs((sensReadArr[time - 1] - sensReadArr[time]));
}

int isInConfInt(int data, int time)
{
	// Simple confidence interval test, data hasnt changed more then allowedDelta
	return ( (data) < (readVal[time-1] + allowedDelta) && (data) > (readVal[time-1] - allowedDelta))? 1: 0;

		
}

/*---------------------------------------------------------------------------*/
PROCESS(simple_usac_process, "USAC Test");
AUTOSTART_PROCESSES(&simple_usac_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(simple_usac_process, ev, data)
{
	static struct etimer et;
	
	PROCESS_BEGIN();
	
	int sCount = 1;
	
	// Set first clock
	etimer_set(&et, CLOCK_SECOND);
	
	while(sCount){
		
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		
		
		// Take sample
		sCount = takeSample();
		sampleNb++;
		
		printf("Samplerate: %d, Data(%d): %d -------- System Time: %d \n", (int)(100*sampleRate) ,sampleNb - 1 ,readVal[sampleNb - 1], clock_time());
		
		switch ( sampleNb ) {
			case 0:
			// If N = 0
				//valueOfData = MAXDATAVALUE;
			break;
			case 1:
			// If N = 1
				//valueOfData = MAXDATAVALUE;
				//calculateConfInt(sampleNb - 1);
			break;
			// If N > 1
			default:
				if(isInConfInt(readVal[sampleNb - 1],sampleNb - 1))
				{
					sampleRate = max( (alpha * sampleRate), MINSAMPLERATE );	
				}
				else
				{
					sampleRate = MAXSAMPLERATE;
					sampleNb = 0;
				}
			break;
		}
		
		
		// Set timer depending on sampleRate
		etimer_set(&et, (CLOCK_SECOND * 60) / sampleRate);
	}
	
	printf("Process ended!");
	PROCESS_END();
	
}
/*---------------------------------------------------------------------------*/
