// basado en el ejemplo PID_Basic

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
void frecuencia_actual();

// Módulo de control
void control(void)
{
  Input = frecuencia_actual();
  myPID.Compute();
  salida_PID = Output;
}


void setup()
{

  Input = frecuencia_actual();
  Setpoint = tabla_frecuencias[i_freq];

  // Se enciende el PID
  myPID.SetMode(AUTOMATIC);
}

void loop()
{
  // Se realiza el control
  control();
}
