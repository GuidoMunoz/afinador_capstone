////////////////////
//FREQ_COUNTER//////
///////////////////

#include "esp32_config/esp32-hal-timer.h"
#include "esp32_config/esp32-hal-gpio.h"

// String frequencies
#define EL 82.41
#define A 110.
#define D 146.83
#define G 196.
#define B 246.94
#define EH 329.63

hw_timer_t * timer = NULL; //Puntero vacio, usado para configurar el timer
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED; //Variable para sincronizar el loop principal y las interrupciones (tal vez no sea necesario)
int freqinput = 8; //Input del acondicionador

volatile bool state = false; //Estado del contador de frecuencia
volatile int period = 0;

void IRAM_ATTR isr(){
  //Inicia el timer si el estado es 0
  if(state == false){
    timerRestart(timer);
    timerStart(timer);
    state = true;
  }
  //Si el estado es 1 termina el timer y guarda el dato
  if(state == true){
    period = timerRead(timer);
    timerStop(timer);
    state = false;
  }
}

void freq_counter_init(){
  pinMode(freqinput, INPUT); //Entrada de freq como input
  attachInterrupt(button1.PIN, isr, RISING); //Interrupcion a ejecutarse en flanco de subida
  timer = timerBegin(0, 80, true); //set up del timer
}

bool get_frequency(){
  if(period < 10^6){
    if((period!= 0)&&(oldperiod!=0)&&(oldoldperiod!=0)){
      return 3/(period+oldperiod+oldoldperiod)
    }
    else{
      return 777.77 //Dummy para hacer saber al sistema que no hay salida aun
    }
  }
}


///////////////////
//PID//////////////
//////////////////

#include <PID_v1.h>

// Setpoint es la referencia, Input es la entrada al PID, Output es la salida del PID
double Setpoint, Input, Output;
double salida_PID;
double Kp = 1, Ki = 2, Kd = -0.05;
uint8_t i_freq = 9;

// Desde C2, hasta G4 (son 32 notas)
float tabla_frecuencias[32] = {
  65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 110, 116.54, 123.47,
  130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94,
  261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00
};

uint16_t frecuencia_referencia = 110;

// Se inicializa el PID
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// Función que obtiene la frecuencia medida
//void frecuencia_actual();

// Módulo de control
double control(bool freq)
{
  Input = freq;
  myPID.Compute();
  salida_PID = Output;
  return salida_PID;
}

////////////////
///SERVO////////
////////////////

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int output = 13; //where should servo be controlled from
int pos = 90; //Initial angle

void motor_drive(delta) {
  myservo.write(pos+delta);
  pos = pos + delta;
}

void setup() {
  ////////////////////
  //PID///////////////
  ////////////////////
  
  Input = frecuencia_actual();
  Setpoint = tabla_frecuencias[i_freq];

  // Se enciende el PID
  myPID.SetMode(AUTOMATIC);

  ///////////////////
  //FREQ_CONT////////
  ///////////////////
  
  freq_counter_init();
  int oldperiod = 0; //Periodo anterior
  int oldoldperiod = 0; //Periodo anterior anterior

  ////////////////
  ///SERVO////////
  ////////////////

  myservo.attach(output); 

}

void loop() {
  ///////////////////
  //FREQ_CONT////////
  ///////////////////
  
  freq = get_frequency();
  oldoldperiod = oldperiod;
  oldperiod = period;
  
  ////////////////////
  //PID///////////////
  ////////////////////
  
  delta = control(freq);

  ////////////////
  ///SERVO////////
  ////////////////

  motor_drive(delta);

}
