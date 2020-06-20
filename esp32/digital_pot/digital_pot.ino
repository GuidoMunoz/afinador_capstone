#include "MCP4561_DIGI_POT.h"
#include <Wire.h>


uint8_t* pot_pins = {34, 35, 32, 33}  // Pines de la dirección de cada potenciómetro

//MCP4561 digitalPot(0x2F);                      // Instance of MCP4561 pot with address supplied. A0 = 1
MCP4561 digitalPot;                              // Instance of MCP4561 pot with default address. A0 = 1

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

void setup(void)
{
  Wire.begin();
  pinMode(pot_pins[0], OUTPUT);
  pinMode(pot_pins[1], OUTPUT);
  pinMode(pot_pins[2], OUTPUT);
  pinMode(pot_pins[3], OUTPUT);
  digitalWrite(pot_pins[0], LOW);
  digitalWrite(pot_pins[1], LOW);
  digitalWrite(pot_pins[2], LOW);
  digitalWrite(pot_pins[3], LOW);
  Serial.begin(115200);
}

void loop(void):
{
  //
}
