#include <stdio.h>
#include "i2c.h"

******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdint.h>         // Needed for unit uint8_t data type
#include <stdlib.h>         // Needed for exit()
const int16_t x_flat_h = 0xFE;
const int16_t x_flat_l =0xE0;
const int16_t  y_flat_h=0xFD;
const int16_t  y_flat_l =0x8C;
const int16_t  z_flat_h=0x41;
const int16_t  z_flat_l= 0x50;
/*
#define x_left_h 0x04
#define x_left_l 0x00
#define y_left_h 0xC1
#define y_lef_l 0x2C
#define z_left_h 0xFC
#define z_left_l 0x90

#define x_right_h 0xFF
#define x_right_l 0x70
#define y_right_h 0x40
#define y_right_l 0x00
#define z_right_h 0x03
#define z_right_l 0xE0
*/
#define accel_res 2.0/32768.0 // 15 bits de resolución
#define accel_res1 4.0/32768.0
#define accel_res2 8.0/32768.0
#define accel_res3 16.0/32768.0

int main()
{
    int16_t accel_count[3]; // x y z de 16 bits
    uint8_t raw_data[6]; // x y z high and low
    int16_t destination[3];
    float accel_x,accel_x1,accel_x2,accel_x3;
    float accel_y,accel_y1,accel_y2,accel_y3;
    float accel_z,accel_z1,accel_z2,accel_z3;
    //1.Leer acc data
    //ptr_i2c->ReadFromMemInto(kMpu6500Addr, kAccelXoutH, 6, &raw_data[0]);
    
    //Concatenación de MSB y LSB
    
    raw_data[0]=x_flat_h;
    raw_data[1]=x_flat_l;
    raw_data[2]=y_flat_h;
    raw_data[3]=y_flat_l;
    raw_data[4]=z_flat_h;
    raw_data[5]=z_flat_l;
    
    destination[0] = (raw_data[0] << 8) | raw_data[1];
    destination[1] = (raw_data[2] << 8) | raw_data[3];
    destination[2] = (raw_data[4] << 8) | raw_data[5];
    
    //Calcular valor en g's*/
    accel_count[0]=destination[0];
    accel_count[1]=destination[1];
    accel_count[2]=destination[2];
    
    accel_x = (float)accel_count[0]*accel_res;
    accel_x1 = (float)accel_count[0]*accel_res1;
    accel_x2 = (float)accel_count[0]*accel_res2;
    accel_x3 = (float)accel_count[0]*accel_res3;
    
    accel_y = (float)accel_count[1]*accel_res;
    accel_y1 = (float)accel_count[1]*accel_res1;
    accel_y2 = (float)accel_count[1]*accel_res2;
    accel_y3 = (float)accel_count[1]*accel_res3;
    
    accel_z = (float)accel_count[2]*accel_res;
    accel_z1 = (float)accel_count[2]*accel_res1;
    accel_z2 = (float)accel_count[2]*accel_res2;
    accel_z3 = (float)accel_count[2]*accel_res3;
    
    /*accel_y = (float)accel_count[1]*accel_res;
    accel_z = (float)accel_count[2]*accel_res;
    */
    
    printf("-----------Valor de x según su resolución-----------\n");
    printf ("El valor es: %0.2f mg con 2 bits de ADC\n",1000*accel_x);
    printf ("El valor es: %0.2f mg con 4 bits de ADC\n",1000*accel_x1);
    printf ("El valor es: %0.2f mg con 8 bits de ADC\n",1000*accel_x2);
    printf ("El valor es: %0.2f mg con 16 bits de ADC\n",1000*accel_x3);
    
    printf("-----------Valor de y según su resolución-----------\n");
    printf ("El valor es: %0.2f mg con 2 bits de ADC\n",1000*accel_y);
    printf ("El valor es: %0.2f mg con 4 bits de ADC\n",1000*accel_y1);
    printf ("El valor es: %0.2f mg con 8 bits de ADC\n",1000*accel_y2);
    printf ("El valor es: %0.2f mg con 16 bits de ADC\n",1000*accel_y3);
    
    printf("-----------Valor de z según su resolución-----------\n");
    printf ("El valor es: %0.2f mg con 2 bits de ADC\n",1000*accel_z);
    printf ("El valor es: %0.2f con 4 bits de ADC\n",1000*accel_z1);
    printf ("El valor es: %0.2f mg con 8 bits de ADC\n",1000*accel_z2);
    printf ("El valor es: %0.2f mg con 16 bits de ADC\n",1000*accel_z3);
    
    
    return 0;
}