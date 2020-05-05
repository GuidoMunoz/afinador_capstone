#include <stdio.h>
#include <math.h>

//Define parameter
#define epsilon 0.01  // 0.01
#define MAX 1  // 100
#define MIN -1  // -100
#define Kp 0.1  // 10
#define Ki 0.005  // 0.5
#define Kd -0.005  // -0.5

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
    |[5]:A3  -> 110Hz|
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
    // Seleccionar parte de arriba (0, 0)
  if (selector == 0)
  {
    imprimir_referencia_1();
  }
  else
  {
    imprimir_referencia_2();
  }

  // Seleccionar parte de abajo (0, 1)
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
}

void control(void)
{
  float frecuencia_referencia = tabla_frecuencias[freq_cuerdas[cuerda_actual-1]];

      printf("frecuencia: %f\n", frecuencia_referencia);

      error = frecuencia_referencia - frecuencia_medida;

      printf("error: %f\n", error);


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

      printf("Salida: %f\n", salida_PID);


      frecuencia_medida = k[cuerda_actual-1]/frecuencia_medida* salida_PID + frecuencia_medida;
}

int main(void)
{
    printf("\n");
    printf("\n");

    char entrada;
    printf(">> ");
    scanf("%s", &entrada);
    while (entrada != '0')
    {

      if (entrada == 'j')
      {
        if (cuerda_actual < 6)
        {
          cuerda_actual++;
        }
        else
        {
          cuerda_actual = 1;
        }
        frecuencia_medida = tabla_frecuencias[freq_cuerdas[cuerda_actual-1]]-20/cuerda_actual;

        integral = 0;
      }



      printf("\n");
      imprimir_pantalla();
      printf("|%s|\n", string_1);
      printf("|%s|\n", string_2);
      printf(">> ");
      scanf("%s", &entrada);

      if (entrada == 'm')
      {
        if (selector == 0 & modo == 0)
        {
          modo = 1;
        }
        else if (selector == 1 & modo == 0)
        {
          modo = 2;
        }
        else
        {
          modo = 0;
        }
      }
      else if (entrada == 'a' | entrada == 'd')
      {
        if (modo == 0)
        {
          selector = (selector + 1)%2;
        }
        else if (modo == 1)
        {
          if (cuerda_actual == 1 & entrada == 'a')
          {
            //
          }
          else if (cuerda_actual == 6 & entrada == 'd')
          {
            //
          }
          else if (entrada == 'a')
          {
            cuerda_actual --;
          }
          else if (entrada == 'd')
          {
            cuerda_actual ++;
          }
        }
        else
        {
          int n = freq_cuerdas[cuerda_actual - 1];
          if (n == 0 & entrada == 'a')
          {
            //
          }
          else if (n == 31 & entrada == 'd')
          {
            //
          }
          else if (entrada == 'a')
          {
            freq_cuerdas[cuerda_actual - 1] = freq_cuerdas[cuerda_actual - 1] - 1;
          }
          else if (entrada == 'd')
          {
            freq_cuerdas[cuerda_actual - 1] = freq_cuerdas[cuerda_actual - 1] + 1;
          }
        }
      }
      if (entrada == 'c')
        {
          control();
        }


      // control();


    }


    return 0;

}
