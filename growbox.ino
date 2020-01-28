#include <Wire.h>
#include "ds3231.h"
#include "rtc_ds3231.h"
#include <DHT.h>
#include <LCD5110_Graph.h>

LCD5110 myGLCD(8,9,10,12,11);

extern uint8_t SmallFont[];
extern uint8_t arduino_logo[];
extern unsigned char TinyFont[];


#define BUFF_MAX 128

uint8_t time[8];
char recv[BUFF_MAX];
unsigned int recv_size = 0;
unsigned long prev, interval = 1000;

#define DHTPIN A0     // Pin del Arduino al cual esta conectado el pin 2 del sensor

// Descomentar segun el tipo de sensor DHT usado
#define DHTTYPE DHT11   // DHT 11 
int LDRPIN = 7;
int Luz = 0;
char LuzCadena[4];
String str;
int Intensidad = 0;

// Diagrama
// Pin 1 Sensor a +5V de Arduino
// Pin 2 Sensor a HDTPIN (en este sketch es el pin 2)
// Pin 4 Sensor a GROUND de  Arduino
// Resistencia de 10K desde Pin 2 de sensor a Pin 1 de Sensor o +5V

// Inicializa el sensor
DHT dht(DHTPIN, DHTTYPE);

// Configura Arduino 
void setup() {
  Serial.begin(9600);
  dht.begin();
  delay(2000);
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  Wire.begin();
  DS3231_init(DS3231_INTCN);
  memset(recv, 0, BUFF_MAX);
  Serial.println("GET time");
 }
 
void loop() {
  
  // Espera dos segundos para realizar la primera medición.
  myGLCD.clrScr();
  myGLCD.print("PROYECTO", 0, 0);
  myGLCD.print("IE 311", 0, 10);
  
  myGLCD.print("ALUMNO", 0, 20);
  myGLCD.print("HORUS DIAZ", 0, 30);
  myGLCD.update();
  
  delay(2000);
  
  
  myGLCD.clrScr();
  myGLCD.print("PROYECTO", 0, 0);
  myGLCD.print("IE 311", 0, 10);
  
  myGLCD.print("ALUMNO", 0, 20);
  myGLCD.print("HORUS DIAZ", 0, 30);
  myGLCD.update();
  
  delay(2000);
  
  char in;
    char tempF[6]; 
    float temperature;
    char buff[BUFF_MAX];
    unsigned long now = millis();
    struct ts t; 
  
  
    // show time once in a while
    //if ((now - prev > interval) && (Serial.available() <= 0)) {
        DS3231_get(&t); //Get time
        parse_cmd("C",1);
        //temperature = DS3231_get_treg(); //Get temperature
        //dtostrf(temperature, 5, 1, tempF);
        myGLCD.clrScr();
        //myGLCD.print("X", 0, 35);
        
        
        //printMonth(t.mon);
        myGLCD.print("FECHA", 0, 0);
        myGLCD.printNumI(t.mday, 0, 8);

        printMonth(t.mon);
        //myGLCD.printNumI(t.mon, 15, 8);
        
        myGLCD.printNumI(t.year, 35, 8);

        myGLCD.print("HORA", 0, 20);
        if(t.hour<10)
        {
          myGLCD.print("0", 0, 28);
          myGLCD.printNumI(t.hour, 6, 28);
        }
        else
        {
          myGLCD.printNumI(t.hour, 0, 28);
        }
        
        //myGLCD.printNumI(t.hour, 0, 28);
        myGLCD.print(":", 12, 28);
        if(t.min<10)
        {
          myGLCD.print("0", 17, 28);
          myGLCD.printNumI(t.min, 23, 28);
        }
        else
        {
          myGLCD.printNumI(t.min, 17, 28);
        }
        
        myGLCD.print(":", 29, 28);
        if(t.sec<10)
        {
          myGLCD.print("0", 34, 28);
          myGLCD.printNumI(t.sec, 40, 28);
        }
        else
        {
          myGLCD.printNumI(t.sec, 34, 28);
        }

        printDay(t.wday);
        //myGLCD.printNumI(t.wday, 40, 40);
        
        myGLCD.update();

delay(2000);

        
  // Lee los datos entregados por el sensor, cada lectura demora 250 milisegundos
  // El sensor muestrea la temperatura cada 2 segundos}
  // Obtiene la Humedad
  float h = dht.readHumidity();
  // Obtiene la Temperatura en Celsius
  float te = dht.readTemperature();
  
  int humedadtierra = analogRead(A1);

  myGLCD.clrScr();
  myGLCD.print("HUMEDAD: ", 0, 0);
  myGLCD.printNumF(h, 0, 0, 10);
  myGLCD.print(" %", 15, 10);
  
  myGLCD.print("TEMPERATURA: ", 0, 20);
  myGLCD.printNumF(te, 0, 0, 30);
  myGLCD.print(" *C ", 15, 30);
  myGLCD.update();

  delay(2000);

  myGLCD.clrScr();
  myGLCD.print("H. TIERRA: ", 0, 0);
  printHumedad(humedadtierra);
  //myGLCD.printNumI(humedadtierra, 0, 10);
  myGLCD.update();

  delay(2000);
    //}

  myGLCD.clrScr();
  
  Luz = analogRead(LDRPIN);
  Luz = map(Luz, 1010, 20, 0, 100);
  str = String(Luz)+"%";
  str.toCharArray(LuzCadena,4);
  myGLCD.print("LUZ", CENTER, 0);
  myGLCD.print(LuzCadena, CENTER, 15);

  //printBAR();
  //fillBar(Luz);
  myGLCD.update();
  
  delay(2000);

  /*
  myGLCD.clrScr();
  myGLCD.print("Hola Mundo!", CENTER, 25);
  myGLCD.update();
  
  // Control de errores, valida que se obtuvieron valores para los datos medidos
  if (isnan(h) || isnan(te)) {
    myGLCD.print("Falla al leer el sensor DHT!", CENTER, 0);
    delay(1000);
    return;
  }
*/
}

void parse_cmd(char *cmd, int cmdsize)
{
    uint8_t i;
    uint8_t reg_val;
    char buff[BUFF_MAX];
    struct ts t;

    // TssmmhhWDDMMYYYY aka set time
    if (cmd[0] == 84 && cmdsize == 16) {
        //T355720619112011
        t.sec = inp2toi(cmd, 1);
        t.min = inp2toi(cmd, 3);
        t.hour = inp2toi(cmd, 5);
        t.wday = inp2toi(cmd, 7);
        t.mday = inp2toi(cmd, 8);
        t.mon = inp2toi(cmd, 10);
        t.year = inp2toi(cmd, 12) * 100 + inp2toi(cmd, 14);
        DS3231_set(t);
    }

}
void printDay(int wday)
{
  switch(wday)
  {
    
    case 1: myGLCD.print("DOMINGO", 0, 40);break;
    case 2: myGLCD.print("LUNES", 0, 40);break;
    case 3: myGLCD.print("MARTES", 0, 40);break;
    case 4: myGLCD.print("MIERCOLES", 0, 40);break;
    case 5: myGLCD.print("JUEVES", 0, 40);break;
    case 6: myGLCD.print("VIERNES", 0, 40);break;
    case 7: myGLCD.print("SABADO", 0, 40);break;
    default: myGLCD.print("ERROR", 0, 40);break;
  } 
}


void printMonth(int month)
{
  switch(month)
  {
    case 1: myGLCD.print("ENE", 15, 8);break;
    case 2: myGLCD.print("FEB", 15, 8);break;
    case 3: myGLCD.print("MAR", 15, 8);break;
    case 4: myGLCD.print("ABR", 15, 8);break;
    case 5: myGLCD.print("MAY", 15, 8);break;
    case 6: myGLCD.print("JUN", 15, 8);break;
    case 7: myGLCD.print("JUL", 15, 8);break;
    case 8: myGLCD.print("AGO", 15, 8);break;
    case 9: myGLCD.print("SEP", 15, 8);break;
    case 10: myGLCD.print("OCT", 15, 8);break;
    case 11: myGLCD.print("NOV", 15, 8);break;
    case 12: myGLCD.print("DIC", 14, 8);break;
    default: myGLCD.print("Error", 15, 8);break;
  } 
}

void printHumedad(int valor)
{
 if (valor >= 0 && valor <= 256)
  {
    myGLCD.print("MUY HUMEDO", 0, 10);
  }
  if (valor >= 257 && valor <= 512)
  {
    myGLCD.print("HUMEDO", 0, 10);
  }
  if (valor >= 513 && valor <= 768)
  {
    myGLCD.print("SECO", 0, 10);
  }
  if (valor >= 769 && valor <= 1024)
  {
    myGLCD.print("MUY SECO", 0, 10);
  } 
}

