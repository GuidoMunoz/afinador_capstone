#include esp32_config/esp32-hal-timer.h
#include esp32_config/esp32-hal-gpio.h

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

void setup() {
  // put your setup code here, to run once:
  freq_counter_init();
  int oldperiod = 0; //Periodo anterior
  int oldoldperiod = 0; //Periodo anterior anterior
}

void loop() {
  freq = get_frequency();
  oldoldperiod = oldperiod;
  oldperiod = period;
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

