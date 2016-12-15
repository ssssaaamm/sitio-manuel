//HECTOR ALCIDES FRANCO PAREDES
//RICARDO JOSE PACHECO MENDEZ
 
  //declarando librerias a utilizar
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
RTC_Millis RTC;

//declarando alcunas constantes principales

const int cs=9;       //selector chip de integrado
const byte     leer=0b00000000;    
const byte escribir=0b10000000;
const unsigned int acupositiva=0b1000000000001100;
const int cssd=10;    //selector chip de dataloger
long muestra2=0;
long lasttiempo=0;
long ut=0; //ultimo-tiempo
void setup(){
  SPI.setDataMode(SPI_MODE2);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  SPI.begin();
  
  Serial.begin(9600);
   
  //codigo para comprobar inicializacion de datalogger
  while (!Serial) {
                 ;}
  Serial.print("Iniciando targeta SD...");
  RTC.begin(DateTime(__DATE__, __TIME__));
  
  // ve si la tarjeta esta preente y puede ser inicializada
  if (!SD.begin(cssd)) {
    Serial.println("latarjeta fallo, o no esta presente");
  
  // para que no realize nada mas
    return;
  }
  Serial.println("Tarjeta inicializada.");
  
 pinMode(10, OUTPUT);          //PIN DE DATAlooger como salida
  //configuracion estandar de la comunicacion SPI del IC.
  
  pinMode(9, OUTPUT);           //pin de integrado
  
  //iniciamos algunas constantes 
 
  iniciarconstantes();
  delay(5);  
  csalto();
  //ahora reseteamos el ADE7753
  setMode(76);
  delay(5);
  Serial.println("ADE7753 reseteado");
  setMode(12);
  delay(1);
  //esto sirve para quitar datos bsura del inicio  long
  descartarbasura();  
  delay(1);
  Serial.println("Espere 3 segundos");
}

void loop (){
 
  int modo=getMode();
  //Serial.println(modo);
  
 //obteniendo potencia activa
  long pot_act=get_pot_act();//potencia activa
  pot_act=pot_act<<8;
  pot_act=pot_act>>8;
  float pot_act1=pot_act*1.5;
  //Serial.println(pot_act);//borrame
  //obteniendo potencia aparente
  long pot_apa=get_pot_apa();
  pot_apa=pot_apa<<8;
  pot_apa=pot_apa>>8;
  float pot_apa1=pot_apa*1.8;
  //Serial.print("aparente: \t");//borrame
  //Serial.println(pot_apa);//borrame
  
  
  //apesar de que no esta en ningun modo estamos guardando capturas
  //capturando voltaje rms en su respectivo registro
  
  long voltaje_rms=vrms();
  float voltaje_rms_corregido= voltaje_rms*0.000264585;
  //Serial.println(voltaje_rms);//borrame solo para calibrar
 
  //ahora obtenemos la corriente rms
  //capturando corriente rms en su respectivo registro
 
  long corriente_irms=irms();
  corriente_irms=corriente_irms-1400;
 //Serial.print("corriente: \t");
 //Serial.println(corriente_irms);//borrame
  float corriente_irms_corregido=corriente_irms*0.000048575;
  //float potencia_activa=voltaje_rms_corregido*corriente_irms_corregido;
  DateTime now = RTC.now();//esto solo es para la rtc
  int segundos=now.second();
  long tiempo=millis();
  long intervalo=tiempo-lasttiempo;
  int multiplos=segundos%5;
  //Serial.println(intervalo);
      if (multiplos==0 & intervalo>4000 ){
        //cssdbajo();
        guardardatos(voltaje_rms_corregido, corriente_irms_corregido, pot_act1, pot_apa1);
        //cssdalto();
        lasttiempo=tiempo;
         
     }
  
  while(Serial.available()){ //¿hay algo que leer? 
  char letra = Serial.read(); //si lo hay, que lo lea.
  
  //entrando en el modo1
 if(letra == 'a'){
    while(letra == 'a'){
      //obteniendo potencia activa
      long pot_act=get_pot_act();//potencia activa
      pot_act=pot_act<<8;
      pot_act=pot_act>>8;
      float pot_act1=pot_act*1.5;
      //Serial.println(pot_act1);//borrame
      //obteniendo potencia aparente
      long pot_apa=get_pot_apa();
      pot_apa=pot_apa<<8;
      pot_apa=pot_apa>>8;
      float pot_apa1=pot_apa*1.8;
      //Serial.print("aparente: \t");//borrame
      //Serial.println(pot_apa1);//borrame      
      long voltaje_rms=vrms();
      float voltaje_rms_corregido= voltaje_rms*0.000264585;
      long corriente_irms=irms();
      corriente_irms=corriente_irms-1400;//offset
      float corriente_irms_corregido=corriente_irms*0.000048575;;  
      Serial.print(voltaje_rms_corregido);
      Serial.print("\t");
      Serial.print(corriente_irms_corregido);//estas move!!!
      Serial.print("\t");
      Serial.print(pot_act1);
      Serial.print("\t");
      Serial.print(pot_apa1);
      Serial.print("\t");
      DateTime now = RTC.now();//esto solo es para la rtc
      String tmpmodo1="";
      tmpmodo1=String(now.hour(),DEC)+"\t"+String(now.minute(),DEC)+"\t"+String(now.second(),DEC);  
      Serial.println(tmpmodo1);
      int segundos=now.second();
      int multiplos=segundos%5;
      long tiempo=millis();
      long intervalo=tiempo-lasttiempo;
      if (multiplos==0 & intervalo>4000 ){
          //cssdbajo();
          guardardatos(voltaje_rms_corregido, corriente_irms_corregido, pot_act1, pot_apa1);
          //cssdalto();
          lasttiempo=tiempo;
          
      }
    char letra = Serial.read();
    if ( letra == 'b') break;
    }  
  }     
  
//entrando en el modo 2
//ahora el siguiente segmento de codigo
//es solo para leer datos luego de apretar un tecla
  
  else if (letra =='c'){
        long energia = get_energia_consumida();
             energia=energia<<8;
             energia=energia>>8;
       float energia2 = energia*0.64;
       
    SPI.setDataMode(SPI_MODE0);
    File miarchivo = SD.open("data.txt");
    //Serial.println(dataString);
    if (miarchivo) {
     while (miarchivo.available()) {
      Serial.write(miarchivo.read());
      }
      miarchivo.close();
            

            }  
    // si el archivo no se abrio, muestra un error
    else {
          Serial.println("error abriendo archivo de texto");
         }
          SPI.setDataMode(SPI_MODE2);  
     Serial.print("Energia\t");
     Serial.println(energia2);
     Serial.print("outm2 \n \n \n \n \n ");  
      }
          
       //la siguiente condicion sirve para borrar archivo cuando se desee
       else if (letra =='s'){
       SPI.setDataMode(SPI_MODE0);  
       SD.remove("data.txt");       
       if (SD.exists("data.txt")){ 
       Serial.println("el archivo existe");
        }
       else {
       //delay(5000);
       Serial.print("El archivo fue borrado");  
            
          }
       SPI.setDataMode(SPI_MODE2);
       setMode(76);
       delay(3); 
      setMode(12);
     delay(1); 
     } 
       //la siguiente condicion sirve para resetear el ADE7753
       else if (letra =='r'){
        setMode(76);
        delay(3);
        setMode(12);
        delay(1);
        Serial.print("El ADE7753 fue reseteado");
    }
      //la siguiente con es para ver si se encuentra dentro del rango de cobertura
      else if (letra =='x'){
        Serial.print("listo");
      }  
 
      //la siguiente condicion es para conocer el consumo total
    else if ( letra == 'e'){
       //obteniendo energia consumida
       long energia = get_energia_consumida();
       energia=energia<<8;
       energia=energia>>8;
       float energia2 = energia*0.64;
       //Serial.print('\n');
       Serial.print(energia2);
       //Serial.print('\n');
   }
}                 



}
