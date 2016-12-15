unsigned char leer8(char reg){
 reg = reg | leer;
  csbajo();
 delayMicroseconds(5);
  SPI.transfer(reg);
 delayMicroseconds(5); 
 return (unsigned char)SPI.transfer(0x00);//cambiar por unsigned byte
 csalto();
}
unsigned int leer16(char reg){
  reg = reg | leer;
  csbajo();
  unsigned char b1;
  unsigned char b0;
   delayMicroseconds(5);
   SPI.transfer(reg);
   delayMicroseconds(5);
   b1=SPI.transfer(0x00);
   delayMicroseconds(5);
   b0=SPI.transfer(0x00);
   return (unsigned int)b1<<8 | (unsigned int)b0;  
   csalto;
}
unsigned long leer24(char reg){
  reg = reg | leer;
  csbajo();
  unsigned char b2,b1,b0;
  delayMicroseconds(10);
  SPI.transfer(reg);
  delayMicroseconds(25);
  b2=SPI.transfer(0x00);
  delayMicroseconds(5);
  b1=SPI.transfer(0x00);
  delayMicroseconds(5);
  b0=SPI.transfer(0x00);
  return (unsigned long)b2<<16 | (unsigned long)b1<<8 | (unsigned long)b0;
  csalto();
}
void escribir8(char reg, char data){
 reg = reg | escribir;
 csbajo();
 delayMicroseconds(10);
 SPI.transfer((unsigned char)reg);          //register selection
 delayMicroseconds(5);
 SPI.transfer((unsigned char)data);
 delayMicroseconds(5);
 csalto();
  
} 
void escribir16(char reg, int data){
    reg = reg | escribir;
    csbajo();
    unsigned char data0=0;
    unsigned char data1=0;
    data0 = (unsigned char)data;
    data1 = (unsigned char)(data>>8);
    delayMicroseconds(10);
    SPI.transfer((unsigned char)reg);    
    delayMicroseconds(5);    
    //envio de dato, MSB primeros
    SPI.transfer((unsigned char)data1);
    delayMicroseconds(5);
    SPI.transfer((unsigned char)data0);  
    delayMicroseconds(5);
    csalto(); 
}

void setMode(int m){
    escribir16(MODE, m);
}
int getMode(){
    return leer16(MODE);
}

int getInterrupts(void){
    return leer16(IRQEN);
}
void setInterrupts(int i){
    escribir16(IRQEN,i);
}
int getsuma(void){
return leer8(CHKSUM);
}

int getStatus(void){
    return leer16(STATUS);
}

int resetStatus(void){
    return leer16(RSTSTATUS);
}
//obtener consumo total
long get_energia_consumida(void){
return leer24(AENERGY);  
}

//obtener potencia activa
long get_pot_act(void){
return leer24(RAENERGY);

}
long get_pot_apa(void){
return leer24(RVAENERGY);   
}
//obteer valor IRMS
long getIRMS(void){
long lastupdate=0;
resetStatus();
lastupdate=millis();
while (! (getStatus() & ZX ))
{//Serial.println("encerrado en irms");
long contando=millis();
long salirse=contando-lastupdate;
if ( salirse > 200) break;
}
return leer24(IRMS);
}
//obtener valor VRMS
long getVRMS(void){
long lastupdate=0;
resetStatus();
lastupdate=millis();
while (! (getStatus() & ZX ))
{//Serial.println("encerrado en vrms");
long contando=millis();
long salirse=contando-lastupdate;
if ( salirse > 200) break;
}
return leer24(VRMS);
}
//LA SIGUIENTE FUNCION
//INGNORA LAS PRIMERAS GETRMS Y SACA EL PROMEDIODE 100 LECTURAS
long vrms(){
	char i=0;
	long v=0;
	getVRMS();//ignora la primera lectura para evitar basura
	getVRMS();
        getVRMS();
        for(i=0;i<50;++i){
		v+=getVRMS();
	}
	return v/50;
}
//ahora para la corriente
long irms(){
	char n=0;
	long i=0;
	getIRMS();//ignora la primera lectura para evitar basura
	getIRMS();
        getIRMS();
          for(n=0;n<50;++n){
		i+=getIRMS();
	}
	return i/50;
}

void guardardatos(float voltaje_rms_corregido, float corriente_irms_corregido, float potencia_activa, float potencia_reactiva) {
  SPI.setDataMode(SPI_MODE0);
  //SPI.setClockDivider(SPI_CLOCK_DIV32);
  //SPI.setBitOrder(MSBFIRST);
  //SPI.begin();
  csalto();
  //para rtc
  DateTime now = RTC.now();
  String dataString = ""; 
  
  
  //ahora hacemos un string y estos valores y los guardamos en la datalogger
  char tmp[10];
  dataString += dtostrf(voltaje_rms_corregido,1,2,tmp);
  dataString += "\t";
  dataString += dtostrf(corriente_irms_corregido,1,2,tmp);
  dataString += "\t"; 
  dataString += dtostrf(potencia_activa,1,2,tmp);
  dataString += "\t";
  dataString += dtostrf(potencia_reactiva,1,2,tmp);
  dataString += "\t";
  
  //le concatenamos hora y fecha
  dataString = dataString+String(now.year(),DEC)+"\t"+String(now.month(),DEC)+"\t"+String(now.day(),DEC)+"\t"+String(now.hour(),DEC)+"\t";
  dataString = dataString+String(now.minute(),DEC)+"\t"+String(now.second(),DEC);
  
  //ahora escribiremos en el archivo
 File miarchivo = SD.open("data.txt", FILE_WRITE);

  // si el archivo esta disponible, escriba en el.
  if (miarchivo) {
    miarchivo.println(dataString);
    miarchivo.close();
    //Serial.println("entro");
  }
  else {
        Serial.println("noooooo entro");
        }
  /* //ahora el siguiente segmento de codigo
  //es solo para leer datos luego de apretar un tecla
  if (Serial.available() > 0){
  char numero=Serial.read();
  if (numero=='c'){
  //File miarchivo = SD.open("data.txt");
  //Serial.println(dataString);
  // si el archivo esta disponible, escribe en el
  if (miarchivo) {
    while (miarchivo.available()) {
      Serial.write(miarchivo.read());
    }
    miarchivo.close();
    Serial.print("outm2");      
              }  
  // si el archivo no se abrio, muestra un error
  else {
        Serial.println("error abriendo archivo de texto");
       }
                }
                  }*/
SPI.setDataMode(SPI_MODE2);

}


//la siguiente funcion es para 
//leer muestriar y mandarlo directamente
//por el USB o XBEE 
//void muestrear (void){
//ahora leemos 
//int estadoIRQ;
//estadoIRQ = digitalRead(IRQ);
//if(estadoIRQ==LOW){
//muestra2=leer24(WAVEFORM);
//long muestra4=muestra2<<8;
//long muestra3=muestra4>>8;
//float muestracorregida=(muestra3-5567);//*0.0751408;
//Serial.print(muestra2,HEX);
//Serial.println("\t muestra2");
//Serial.print(muestra3,HEX);
//Serial.println("\t muestra3");
//resetStatus();
//Serial.println(muestra3,HEX);
//delayMicroseconds(100);
//long muestra1=muestra2<<8;
//muestra1 =muestra1>>8;
/*unsigned long muestra= muestra2 & 0x00800000;
if (muestra==0){
float muestracorregida=muestra2*0.02454873;
Serial.println(muestracorregida,DEC);
//Serial.println("muestracorregida");
}
else {
muestra2=muestra2 | 0xFF000000;
 float muestracorregida=muestra2*0.02454873;
Serial.println(muestracorregida,DEC);
Serial.println("dos");
}
*/
//resetStatus();

//float muestracorregida=muestra1*0.02454873;
//Serial.println("muestra");
//Serial.println(muestra2,HEX);
//Serial.println("muestra corregida");
//Serial.println(muestracorregida,DEC);
//int m=getMode(); 

//}
//}//cierra funcion

void descartarbasura(void){
int n=0;
  for(n=0;n<900;++n){
  getVRMS();
  delay(1);
  getIRMS();
  delay(1);

}
}

//long tpa=millis(); //tpa=tiempo-potencia-activa
  //long itpa=tpa-ut;//intervalo-tiempo-potencia-activa
  //ut=tpa; 
  //pot_act=pot_act/itpa;
