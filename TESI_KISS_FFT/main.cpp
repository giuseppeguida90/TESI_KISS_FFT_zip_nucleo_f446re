#include "mbed.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "kiss_fft.h"
#define MAX_DIM 1024

#define M_PI 3.14159265359

Serial pc(USBTX,USBRX,9600);

kiss_fft_cpx array_in[MAX_DIM];
kiss_fft_cpx array_out[MAX_DIM];

int main()
{
  srand(time(NULL));  
  
  float Fs = 5000;
  float T = 1/Fs;
  float t;  
  float S[MAX_DIM];
  float positiveband[(MAX_DIM/2)+1];
  float frequencies[(MAX_DIM/2)];
  
  kiss_fft_cfg cfg = kiss_fft_alloc( MAX_DIM ,0,0,0 );
  
  /*Riempimento del vettore di misure*/
  for(int i = 0; i < MAX_DIM; i++) {
    t = i*T;
    array_in[i].r = 10*sin(2*M_PI*50*t);
    array_in[i].i = 0;    
  }
   
  clock_t begin = clock();
  
  /*Fa la FFT del vettore campionato*/
  kiss_fft( cfg , array_in , array_out );    
  
  /*Fa la normalizzazione dei campioni dividendoli per la lunghezza del vettore*/
  for(int j = 0; j < MAX_DIM; j++){
      array_out[j].r = array_out[j].r/MAX_DIM;
      array_out[j].i = array_out[j].i/MAX_DIM; 
  }  
  
  /*Fa il valore assoluto dei campioni*/
  for(int j = 0; j < MAX_DIM; j++){
      S[j] = sqrt(pow(array_out[j].r,2)+pow(array_out[j].i,2)); 
  }  
  
  /*Prende solo la parte positiva dello spettro*/
  for(int j = 1; j < (MAX_DIM/2)+1; j++){
      positiveband[j] = S[j]; 
  }
  
  /*Raddoppia la parte positiva dello spettro*/
  for(int j = 2; j < (MAX_DIM/2)+1; j++){
      positiveband[j] = 2*positiveband[j]; 
  }
  
  int location;
  float maximum;
  
  maximum = positiveband[0];
  for (int j = 1; j < (MAX_DIM/2)+1; j++){
    if (positiveband[j] > maximum){
        maximum  = positiveband[j];
        location = j;
    }
  }
  
  for(int j = 1; j < (MAX_DIM/2); j++){
      frequencies[j] = (Fs*j)/MAX_DIM; 
  }
  
  int i;
  for(i = 1; i <= 50; i++){
       pc.printf("%d harmonics, amplitude: %f, frequency: %f\n\r",i,positiveband[i*location],frequencies[i*location]);
  }  
  
  float total = 0.0; 
  
  for(i = 2; i <= 50; i++){
       //pc.printf("%d harmonics, amplitude: %f, frequency: %f\n\r",i,positiveband[i*location],frequencies[i*location]);
       total += positiveband[i*location]*positiveband[i*location];
  }  
  
    float thd = ( sqrt(total)/(positiveband[location]) )*100;
    
    pc.printf("THD: %f\n\r",thd);
    
   
   
   /*for(int j = 0; j < (MAX_DIM/2)+1; j++){
      pc.printf("%d, Ampiezza = %f, frequenza = %f\n\r",j,positiveband[j],frequencies[j]); 
  }*/
   
   
  clock_t end = clock();
  float time_spent = (float)(end - begin) / CLOCKS_PER_SEC;
  
  pc.printf("Elapsed time (seconds): %f for %d samples \n\r",time_spent,MAX_DIM);
  
  return 0;
}



