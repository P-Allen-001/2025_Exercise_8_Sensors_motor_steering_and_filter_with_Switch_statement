/*

This program is an example of a Structured Text (ST) type of Programmable Logic Control (PLC).

As the name implies the mechanism is controlled by simple logic.

See International standard IEC 61131 for Programmable Logic Controllers.

Paul Allen 06 June 2024

*/



#include "mbed.h"
#include "VL53L0X.h"
#include "SRF05.h"

I2C i2c(PB_9,PB_8);

SRF05 srf_Left(PA_10,PB_3); // Output from PA_10 (D2) is connected to the Trigger pin of the SRF05, Input to PB_3 (D3) is connected to the SRF05 Echo_Pin. 
SRF05 srf_Right(PA_8,PA_9); // Output from PA_8 (D7) is connected to the Trigger pin of the SRF05, Input to PA_9 (D8) is connected to the SRF05 Echo_Pin.   

PwmOut Drive_pin_A(PB_0);  //steering servo PWM output pin (CN8,A3)
PwmOut Drive_pin_B(PB_10); //Motor drive PWM output pin D6
PwmOut Drive_pin_C(PB_4);  //Motor drive PWM output pin D5 

VL53L0X sensor(&i2c);
Serial      usb(USBTX, USBRX, 9600);

float Left_Ultrasonic;
float Right_Ultrasonic;
float Time_of_flight_input;             // Not used. See line 42
float Time_of_flight_filtered_value;

float A, B, C, count;

int Command;


void Full_power_forward()               //Procedure
{
   Drive_pin_C.pulsewidth_ms(0);
   Drive_pin_B.pulsewidth_ms(20);
}

void Half_power_forward()                //Procedure
{
   Drive_pin_C.pulsewidth_ms(0);
   Drive_pin_B.pulsewidth_ms(10);
}

void Half_power_reverse()               //Procedure
{
   Drive_pin_C.pulsewidth_ms(10);
   Drive_pin_B.pulsewidth_ms(0);
}

void Straight()                         //Procedure
{
    Drive_pin_A.pulsewidth_us(1500);
}

void Half_left()                         //Procedure
{
    Drive_pin_A.pulsewidth_us(1750);
}

void Full_left()                         //Procedure
{
    Drive_pin_A.pulsewidth_us(2000);
}

void Half_right()                         //Procedure
{
    Drive_pin_A.pulsewidth_us(1250);
}

void Full_right()                         //Procedure
{
    Drive_pin_A.pulsewidth_us(1000);
}


int main() {

    wait(2);

                Drive_pin_A.period_ms(20);   //Steering Servo PWM period
                Drive_pin_B.period_ms(20);   //Motor forward PWM period     
                Drive_pin_C.period_ms(20);   //Motor reverse PWM period 

                Drive_pin_B.pulsewidth_ms(0);        //drive motor off
                Drive_pin_C.pulsewidth_ms(0);        //drive motor off   

               sensor.init();
               sensor.setModeContinuous();
               sensor.startContinuous();
               Time_of_flight_filtered_value=100;


    while (1){

            Time_of_flight_filtered_value = (Time_of_flight_filtered_value*.2)+ (0.8*sensor.getRangeMillimeters());
            Left_Ultrasonic = 10*srf_Left.read() ;
            Right_Ultrasonic = 10*srf_Right.read();                 
    
            if (Left_Ultrasonic < 300) { A=1;} else {A=0;}

            if (Time_of_flight_filtered_value < 250) { B=1;} else {B=0;}

            if (Right_Ultrasonic < 300) { C=1;} else {C=0;}

            count = A+(2*B)+(4*C);
            Command = A+(2*B)+(4*C);

              printf("Time of flight: %.0f \t", Time_of_flight_filtered_value);                                          
              printf("Left Ultrasonic: %.0f  \t"  , Left_Ultrasonic) ;
              printf("Right Ultrasonic: %.0f \t", Right_Ultrasonic);
              printf("Command = %.0f \t", count);
              printf("\n");
              wait(0.05);   
             

    wait(0.1);
    
        switch (Command)  // This must be either an integer or text, not a float
        {
            case 0: Full_power_forward(); Straight(); break;

            case 1: Full_power_forward(); Half_right(); break;

            case 2: Half_power_reverse(); Half_left(); break;

            case 3: Half_power_reverse(); Full_left(); break;

            case 4: Full_power_forward(); Half_left(); break;

            case 5: Half_power_forward(); Straight(); break;

            case 6: Half_power_reverse(); Full_right(); break;

            case 7: Half_power_reverse(); Half_left(); break;    
        }

    }   
        
    }