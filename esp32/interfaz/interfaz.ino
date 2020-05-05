#include <LiquidCrystal.h>

// Se configuran los pines
// LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


// Desde C2, hasta G4 (son 32 notas)
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



uint8_t cuerda_actual = 1;

uint8_t freq_cuerdas[6] = {28, 23, 19, 14, 9, 4};  // EADGBE -> {28(E4), 23(B3), 19(G3), 14(D3), 9(A2), 4(E2)}

// Se setea la frecuencia de referencia a la correspondiente a la cuerda actual
float frecuencia_actual = 329.63;
float frecuencia_medida = 311.20;

uint8_t selector = 0;
uint8_t modo = 0;

char numeros_str[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

char string_1[17] = "[6]:E2 -> 77Hz  ";
char string_2[17] = "Medido: 107Hz   ";

void imprimir_referencia_1(void)
{
  /*
  |[5]:A3  -> 110Hz|
  |Medido: 107Hz   |
  */
  uint8_t n = freq_cuerdas[cuerda_actual - 1];
  string_1[0] = '[';
  string_1[1] = numeros_str[cuerda_actual];
  string_1[2] = ']';
  string_1[3] = ':';
  string_1[4] = tabla_notas[n][0];
  string_1[5] = tabla_notas[n][1];
  string_1[6] = tabla_notas[n]][2];
  string_1[7] = ' ';
  string_1[8] = '-';
  string_1[9] = '>';
  string_1[10] = ' ';

  uint16_t int_f = tabla_frecuencias[n];
  char str[3];
  sprintf(str, "%d", int_f);

  string_1[11] = str[0];
  string_1[12] = str[1];

  if (n < 8)
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
  /*
  |5:[A3 ] -> 110Hz|
  */
  uint8_t n = freq_cuerdas[cuerda_actual - 1];
  string_1[0] = numeros_str[cuerda_actual];
  string_1[1] = ':';
  string_1[2] = '[';
  string_1[3] = tabla_notas[n][0];
  string_1[4] = tabla_notas[n][1];
  string_1[5] = tabla_notas[n]][2];
  string_1[6] = ']';
  string_1[7] = ' ';
  string_1[8] = '-';
  string_1[9] = '>';
  string_1[10] = ' ';

  uint16_t int_f = tabla_frecuencias[n];
  char str[3];
  sprintf(str, "%d", int_f);

  string_1[11] = str[0];
  string_1[12] = str[1];

  if (n < 8)
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

  uint16_t int_f_medida = frecuencia_medida;
  char str[3];
  sprintf(str, "%d", int_f_medida);

  string_2[8] = str[0];
  string_2[9] = str[1];
  string_2[10] = str[2];
  string_2[11] = 'H';
  string_2[12] = 'z';
  string_2[13] = ' ';
  string_2[14] = ' ';
  string_2[15] = ' ';
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

  uint8_t a_1 = 2 * cuerda_actual - 1;

  string_2[a_1] = '[';
  string_2[a_1 + 2] = ']';

}

void imprimir_notas(void)
{
  /*
  |G#3->[A3 ]<-A#3 |  -->   |A3 ->[A#3]<-B3  |
  */
  uint8_t n = freq_cuerdas[cuerda_actual - 1];
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
  // lcd.home();
  if (selector == 0)
  {
    imprimir_referencia_1();
  }
  else
  {
    imprimir_referencia_2();
  }
  // Se imprime la primera línea
  // lcd.print(string_1);

  // Se mueve el cursor al (0, 1)
  // lcd.setCursor(0, 1);
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
  // lcd.print(string_2);
}


void setup()
{
  // Se inicializa el display
  // lcd.begin(16, 2);
  // Se mueve el cursor al (0, 0)
  // lcd.home();
}

void loop()
{
  // Mostrar cuerda_actual y frecuencia_actual
  /*
  |[5]:A3  -> 110Hz|        |5:[A3 ] -> 110Hz|
  |Medido: 107Hz   |        |Medido: 107Hz   |

  Modo cambio de cuerda
  |[5]:A3  -> 110Hz|        |[6]:E2  -> 77Hz |
  |  1 2 3 4[5]6   |  -->   |  1 2 3 4 5[6]  |

  Modo cambio de nota
  |5:[A3 ] -> 110Hz|        |5:[A#3] -> 116Hz|
  |G#3->[A3 ]<-A#3 |  -->   |A3 ->[A#3]<-B3  |
  */

  // Obtener datos de interfaz: cuerda actual (cuerda_actual) y frecuencia deseada (se guarda en freq_cuerdas[cuerda_actual])

  // Se setea la frecuencia de referencia a la correspondiente a la cuerda actual
  uint8_t frecuencia_actual = tabla_frecuencias[freq_cuerdas[cuerda_actual]];
  imprimir_pantalla();
}
