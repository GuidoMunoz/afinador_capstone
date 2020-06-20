// Botones: https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/GPIO/GPIOInterrupt/GPIOInterrupt.ino
// Basado en https://randomnerdtutorials.com/esp32-pwm-arduino-ide/
// Código de potenciómetros digitales basado en https://github.com/SteveQuinn1/MCP4561_DIGI_POT/blob/master/examples/MCP4561_GenericPotCommands/MCP4561_GenericPotCommands.ino

#include <stdio.h>
#include <math.h>
#include <Arduino.h>
#include <LiquidCrystal.h>

#include "MCP4561_DIGI_POT.h"
#include <Wire.h>

uint8_t* pot_pins = {34, 35, 32, 33}  // Pines de la dirección de cada potenciómetro

//MCP4561 digitalPot(0x2F);                      // Instance of MCP4561 pot with address supplied. A0 = 1
MCP4561 digitalPot;                              // Instance of MCP4561 pot with default address. A0 = 1

unsigned long debounceTime = 300;
unsigned long lastDetection = 0;
volatile unsigned long estado_leido;

uint8_t pin_motor = 16;
uint8_t motor_in = 0;


// Se configuran los pines
LiquidCrystal lcd(22, 23, 5, 18, 19, 21);
// LiquidCrystal(rs, enable, d4, d5, d6, d7)
// D13 -> rs, D12 -> enable, D14 -> d4, D27 -> d5, D26 -> d6, D25 -> d7

//Define parameter
#define epsilon 0.01  // 0.01
#define MAX 4.76  // 100
#define MIN -4.76  // -100
#define Kp 0.1  // 10
#define Ki 0  // 0.5
#define Kd 0  // -0.5

#define servo_MAX 90
#define servo_MIN 60
#define servo_zero 72


float k[6] = {2631.1, 1782.1, 1400.2, 571, 327, 173};

double error, error_anterior, salida_PID;
double integral, derivada;


int cuerda_actual = 2;

float tabla_frecuencias[32] = {
  65.41, 69.30, 73.42, 77.78, 82.41, 87.31, 92.50, 98.00, 103.83, 110, 116.54, 123.47,
  130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94,
  261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00
};

char *tabla_notas[32] = {
  "C2 ", "C#2", "D2 ", "D#2", "E2 ", "F2 ", "F#2", "G2 ", "G#2", "A2 ", "A#2", "B2 ",
  "C3 ", "C#3", "D3 ", "D#3", "E3 ", "F3 ", "F#3", "G3 ", "G#3", "A3 ", "A#3", "B3 ",
  "C4 ", "C#4", "D4 ", "D#4", "E4 ", "F4 ", "F#4", "G4 "
};

int freq_cuerdas[6] = {28, 23, 19, 14, 9, 4};  // EADGBE -> {28(E4), 23(B3), 19(G3), 14(D3), 9(A2), 4(E2)}

// Se setea la frecuencia de referencia a la correspondiente a la cuerda actual
float frecuencia_actual = 329.93;
float frecuencia_medida = 311.20;

int selector = 0;
int modo = 0;


char numeros_str[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

char string_1[17] = "1234567890123456";

char string_2[17] = "1234567890123456";

volatile char entrada;

volatile uint8_t controlar = 0;
int int_f_medida = 0;

float delta_theta_sim = 0;

void asignar(uint16_t valor):
{
  // Asignar valor al potenciómetro en la dirección actual
  digitalPot.writeVal(MCP4561_VOL_WIPER_0, valor);
}

void asignar_a_pot(uint16_t valor, uint8_t pot):
{
  // Asignar valor solo al potenciómetro pot
  for (uint8_t i = 0; i < 4; i++)
  {
    if (i == pot)  // poner en 1 la dirección del potenciómetro a asignar
    {
      digitalWrite(pot_pins[pot], HIGH);
    }
    else  // poner en 0 todas las direcciones de los potenciómetros que no sean pot
    {
      digitalWrite(pot_pins[i], LOW);
    }
    asignar(valor);  // Asignar el valor dado
  }
}

void asignar_a_todos(uint16_t valor):  // valor es un número entre 0 y 127
{
  // Asignar a cada potenciómetro el mismo valor
  for (uint8_t i = 0; i < 4; i++)
  {
    asignar_a_pot(valor, i);
  }
}

void imprimir_referencia_1(void)
{
  int m = freq_cuerdas[cuerda_actual - 1];
  /*
  |[5]:A3  -> 110Hz|
  */
  string_1[0] = '[';
  string_1[1] = numeros_str[cuerda_actual];
  string_1[2] = ']';
  string_1[3] = ':';
  string_1[4] = tabla_notas[m][0];
  string_1[5] = tabla_notas[m][1];
  string_1[6] = tabla_notas[m][2];
  string_1[7] = ' ';
  string_1[8] = '-';
  string_1[9] = '>';
  string_1[10] = ' ';

  int int_f = tabla_frecuencias[m];
  char str[3];
  sprintf(str, "%d", int_f);

  string_1[11] = str[0];
  string_1[12] = str[1];

  if (m < 8)
  {
    string_1[13] = 'H';
    string_1[14] = 'z';
    string_1[15] = ' ';
  }
  else
  {
    string_1[13] = str[2];
    string_1[14] = 'H';
    string_1[15] = 'z';
  }

}

void imprimir_referencia_2(void)
{
  int m = freq_cuerdas[cuerda_actual - 1];
  /*
  |5:[A3 ] -> 110Hz|
  */

  string_1[0] = numeros_str[cuerda_actual];

  string_1[1] = ':';
  string_1[2] = '[';
  string_1[3] = tabla_notas[m][0];
  string_1[4] = tabla_notas[m][1];
  string_1[5] = tabla_notas[m][2];
  string_1[6] = ']';
  string_1[7] = ' ';
  string_1[8] = '-';
  string_1[9] = '>';
  string_1[10] = ' ';

  int int_f = tabla_frecuencias[m];
  char str[3];
  sprintf(str, "%d", int_f);

  string_1[11] = str[0];
  string_1[12] = str[1];

  if (m < 8)
  {
    string_1[13] = 'H';
    string_1[14] = 'z';
    string_1[15] = ' ';
  }
  else
  {
    string_1[13] = str[2];
    string_1[14] = 'H';
    string_1[15] = 'z';
  }


}

void imprimir_medido(void)
{
  /*
  |Medido: 107Hz   |
  */
  string_2[0] = 'M';
  string_2[1] = 'e';
  string_2[2] = 'd';
  string_2[3] = 'i';
  string_2[4] = 'd';
  string_2[5] = 'o';
  string_2[6] = ':';
  string_2[7] = ' ';

  int int_f_medida = frecuencia_medida;
  char str[3];
  sprintf(str, "%d", int_f_medida);

  if (frecuencia_medida >= 100)
  {
    string_2[8] = str[0];
    string_2[9] = str[1];
    string_2[10] = str[2];
    string_2[11] = 'H';
    string_2[12] = 'z';
    string_2[13] = ' ';
    string_2[14] = ' ';
    string_2[15] = ' ';
  }
	else
  {
    string_2[8] = ' ';
    string_2[9] = str[0];
    string_2[10] = str[1];
    string_2[11] = 'H';
    string_2[12] = 'z';
    string_2[13] = ' ';
    string_2[14] = ' ';
    string_2[15] = ' ';
  }
}

void imprimir_numeros(void)
{
  /*
  |  1 2 3 4[5]6   |
  */
  string_2[0] = ' ';
  string_2[1] = ' ';
  string_2[2] = '1';
  string_2[3] = ' ';
  string_2[4] = '2';
  string_2[5] = ' ';
  string_2[6] = '3';
  string_2[7] = ' ';
  string_2[8] = '4';
  string_2[9] = ' ';
  string_2[10] = '5';
  string_2[11] = ' ';
  string_2[12] = '6';
  string_2[13] = ' ';
  string_2[14] = ' ';
  string_2[15] = ' ';

  int a_1 = 2 * cuerda_actual - 1;

  string_2[a_1] = '[';
  string_2[a_1 + 2] = ']';

}

void imprimir_notas(void)
{
  /*
  |G#3->[A3 ]<-A#3 |  -->   |A3 ->[A#3]<-B3  |
  */
  int n = freq_cuerdas[cuerda_actual - 1];
  if (n == 0)
  {
    string_2[0] = ' ';
    string_2[1] = ' ';
    string_2[2] = ' ';
  }
  else
  {
    string_2[0] = tabla_notas[n-1][0];
    string_2[1] = tabla_notas[n-1][1];
    string_2[2] = tabla_notas[n-1][2];
  }

  string_2[3] = '-';
  string_2[4] = '>';
  string_2[5] = '[';
  string_2[6] = tabla_notas[n][0];
  string_2[7] = tabla_notas[n][1];
  string_2[8] = tabla_notas[n][2];
  string_2[9] = ']';
  string_2[10] = '<';
  string_2[11] = '-';

  if (n == 31)
  {
    string_2[12] = ' ';
    string_2[13] = ' ';
    string_2[14] = ' ';
  }
  else
  {
    string_2[12] = tabla_notas[n+1][0];
    string_2[13] = tabla_notas[n+1][1];
    string_2[14] = tabla_notas[n+1][2];
  }

  string_2[15] = ' ';

}

void imprimir_pantalla(void)
{
  // Se mueve el cursor al (0, 0)
  lcd.home();
  if (selector == 0)
  {
    imprimir_referencia_1();
  }
  else
  {
    imprimir_referencia_2();
  }

  // Se imprime la primera línea
  lcd.print(string_1);

  // Se mueve el cursor al (0, 1)
  lcd.setCursor(0, 1);
  if (modo == 0)
  {
    imprimir_medido();
  }
  else if (modo == 1)
  {
    imprimir_numeros();
  }
  else
  {
    imprimir_notas();
  }

  // Se imprime la segunda línea
  lcd.print(string_2);
}

void control(void)  // Se implementa un control PID, pero dados sus parámetros funciona como control P
{
  float frecuencia_referencia = tabla_frecuencias[freq_cuerdas[cuerda_actual-1]];
  error = frecuencia_referencia - frecuencia_medida;
  if (fabs(error) > epsilon)
  {
    integral = integral + error;
  }
  derivada = error - error_anterior;

  salida_PID = (Kp * error) + (Ki * integral) + (Kd * derivada);

  if (salida_PID < MIN)
  {
    salida_PID = MIN;
  }
  else if (salida_PID > MAX)
  {
    salida_PID = MAX;
  }
}

void motor_drive(delta) {
	if (delta > 0)  // 4.76 -> 18
  {
  	motor_in = 72 + delta/4.76*18;
  }
  else if (delta == 0)
  {
    motor_in = 72;
  }
  else  // -4.76 -> 12
  {
    motor_in = 72 - delta/4.76*12;
  }

  if (motor_in > 90)
  {
    motor_in = 90;
  }
  else if (motor_in < 60)
  {
    motor_in = 60;
  }

  // Se hace girar al motor
  analogWrite(pin_motor, motor_in);
  delay(20);  // revisar si va

  // Simulación de planta
  frecuencia_medida = k[cuerda_actual-1]/frecuencia_medida*(delta*0.02) + frecuencia_medida;

}


void setup()
{
  Wire.begin();
  pinMode(pot_pins[0], OUTPUT);
  digitalWrite(pot_pins[0], LOW);
  pinMode(pot_pins[1], OUTPUT);
  digitalWrite(pot_pins[1], LOW);
  pinMode(pot_pins[2], OUTPUT);
  digitalWrite(pot_pins[2], LOW);
  pinMode(pot_pins[3], OUTPUT);
  digitalWrite(pot_pins[3], LOW);
  entrada = 'a';
  Serial.begin(115200);
  // Configuración de pines
  pinMode(14, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(14), interrupcion1, CHANGE);
  pinMode(27, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(27), interrupcion2, CHANGE);
  pinMode(26, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(26), interrupcion3, CHANGE);
  pinMode(17, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(17), interrupcion4, CHANGE);
  // Se inicializa el display
  lcd.begin(16, 2);
  // Se mueve el cursor al (0, 0)
  lcd.home();
}

void loop()
{
  imprimir_pantalla();

  if (controlar){
    control();
    float frecuencia_referencia = tabla_frecuencias[freq_cuerdas[cuerda_actual-1]];
    error = frecuencia_referencia - frecuencia_medida;
    // Parar si el error es muy chico
    if (fabs(error) < epsilon){
      controlar = 0;
    }
    motor_drive(salida_PID);
    Serial.printf("Salida motor: ");
    Serial.print(salida_PID, DEC);
    Serial.printf("\n");

  }
  else{
    if (entrada == 'm'){
      if (selector == 0 & modo == 0){
        modo = 1;
      }
      else if (selector == 1 & modo == 0){
        modo = 2;
      }
      else if (modo == 2){
        modo = 0;
        int m = freq_cuerdas[cuerda_actual - 1];
  			float frecuencia_referencia = tabla_frecuencias[m];
        // Convertir y cambiar el valor de los potenciómetros de acuerdo a la frecuencia de referencia 'frecuencia_referencia'
      }
      else{
        modo = 0;
      }
    }
    else if (entrada == 'a' | entrada == 'd'){
      if (modo == 0){
        selector = (selector + 1)%2;
      }
      else if (modo == 1){
        if (cuerda_actual == 1 & entrada == 'a'){
        }
        else if (cuerda_actual == 6 & entrada == 'd'){
        }
        else if (entrada == 'a'){
          cuerda_actual --;
        }
        else if (entrada == 'd'){
          cuerda_actual ++;
        }
      }
      else
      {
        int n = freq_cuerdas[cuerda_actual - 1];
        if (n == 0 & entrada == 'a'){}
        else if (n == 31 & entrada == 'd'){}
        else if (entrada == 'a'){
          freq_cuerdas[cuerda_actual - 1] = freq_cuerdas[cuerda_actual - 1] - 1;
        }
        else if (entrada == 'd'){
          freq_cuerdas[cuerda_actual - 1] = freq_cuerdas[cuerda_actual - 1] + 1;
        }
      }
    }
  }
  entrada = ' ';
}

void interrupcion1(){
  if (millis() - lastDetection > debounceTime){
    estado_leido = digitalRead(14);
    if (estado_leido == LOW){
      entrada = 'a';
      lastDetection = millis();
    }
    else if (estado_leido == HIGH){
      lastDetection = millis();
    }
  }
}

void interrupcion2(){
  if (millis() - lastDetection > debounceTime){
    estado_leido = digitalRead(27);
    if (estado_leido == LOW){
      entrada = 'm';
      lastDetection = millis();
    }
    else if (estado_leido == HIGH){
      lastDetection = millis();
    }
  }
}

void interrupcion3(){
  if (millis() - lastDetection > debounceTime){
    estado_leido = digitalRead(26);
    if (estado_leido == LOW){
      entrada = 'd';
      lastDetection = millis();
    }
    else if (estado_leido == HIGH){
      lastDetection = millis();
    }
  }
}

void interrupcion4(){
  if (millis() - lastDetection > debounceTime){
    estado_leido = digitalRead(17);
    if (estado_leido == LOW){
      entrada = 'c';
      if (controlar){
        controlar = 0;
      }
      else{
        controlar = 1;
      }
      lastDetection = millis();
    }
    else if (estado_leido == HIGH){
      lastDetection = millis();
    }
  }
}
