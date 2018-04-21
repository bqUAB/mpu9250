#include <stdio.h>
#include <math.h>
#include <stdbool.h>

bool get_Roll(float accel_x, float accel_y, float accel_z, float* roll)
{
    
    bool success;
    *roll=atan2(accel_y , accel_z) * 57.3;
    if(roll){
      success=1;

    }else success=0;

    return success;
}

//Ratios de conversion
#define A_R 16384.0
#define G_R 131.0
#define PI 3.1416
int main()
{
    float accel_x=134.52;
    float accel_y=-955.99;
    float accel_z=-287.11;
    int Conv=180/PI;
    
    float myroll;
    
    if (get_Roll(accel_x, accel_y, accel_z, &myroll))
    {
        printf("Get roll succesful\n");
        printf("Y-degrees: %0.2fº\n", myroll);
     
    }

    return 0;
    
}

bool get_Roll(float accel_x, float accel_y, float accel_z, float*roll);

