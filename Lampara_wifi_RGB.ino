#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

const char ssid[] = "LED_RGB";
const char password[] = "lamparargb";
ESP8266WebServer server(80);


byte cantidadPixel = 16;
byte pinDatos = 2;//Pin de datos

Adafruit_NeoPixel tira = Adafruit_NeoPixel(cantidadPixel, pinDatos, NEO_GRB + NEO_KHZ800);

bool configAutomatica = false;
bool clienteConectado = false;



void setup() {

  delay(1000);

  tira.begin();//Inicializamos los leds
  tira.show();
  tira.setBrightness(255);//Seteamos el brillo de los leds 
  testNeoPixel(); 

  delay(500);

  Serial.begin(9600);
  server.begin();
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, 1, false, 1); 

  paginaWeb();
}





void loop() {

  server.handleClient();
  comprobacionCliente();


  delay(20);
}






void testNeoPixel(){
  int tiempo = 50;

  for(int i=0;i < cantidadPixel;i++){
    tira.setPixelColor(i, 255, 0, 0);// posicion, R, G, B
    tira.show();
    delay(tiempo);
  }
  
  for(int i=0;i < cantidadPixel;i++){
    tira.setPixelColor(i, 0, 255, 0);// posicion, R, G, B
    tira.show();
    delay(tiempo);
  }

  for(int i=0;i < cantidadPixel;i++){
    tira.setPixelColor(i, 0, 0, 255);// posicion, R, G, B
    tira.show();
    delay(tiempo);
  }

  for(int i=0;i < cantidadPixel;i++){
    tira.setPixelColor(i, 255, 255, 255);// posicion, R, G, B
    tira.show();
    delay(tiempo);
  }
}




bool comprobacionCliente(){
  int espera = 50;

  if(WiFi.softAPgetStationNum() > 0 && clienteConectado == false){

    for(int i=0;i < 8;i++){

      for(int i=0;i < cantidadPixel;i++){
        tira.setPixelColor(i, 0, 255, 0);// posicion, R, G, B
        tira.show();
        delay(espera);
        
        tira.setPixelColor(i, 0, 0, 0);// posicion, R, G, B
        tira.show();
      } 
    }

    clienteConectado = true;
    return true;
  }
  else if(WiFi.softAPgetStationNum() == 0){
    clienteConectado = false;
  }
}








void paginaWeb(){

  server.on("/", []() { 
    String formulario =
      "<!DOCTYPE html>"
      "<html lang='en' dir='ltr'>"
        "<head>"
          "<meta charset='utf-8'>"
          "<title>LED RGB</title>"
          "<style>"
            "body {"
                "font-family:Arial;"
                  "}"
          "</style>"
        "</head>"
        "<body bgcolor='#fff'>"


        "<div style='border: 3px solid red; width:960px; height: 350px; padding:3px; background:#e6fffa;'>"

          "<center>"
            "<h1><font size='7' >Elija el color del LED</font></h1><br/>"
          "</center>"
        
          "<form method='POST'>" 
          "<div style='width: 410px; height: 160px; float:left; margin-left:8px; padding:10px; border:2px solid green;'>"
            "<a class='texto'>Haga click sobre el icono para <br/>seleccionar un color</a><br/><br/>"
            "<input type='color' value='#000000' name='selectorColor' list='listacolores' class='inputColor' /> &nbsp;&nbsp;&nbsp;&nbsp;"
            "<input type='submit' value='Enviar Color' name='enviarColor' class='boton' /><br/>"
          "</div>"
          "</form>"
          
        
        "<div style='width: 460px; height: 160px; float:right; margin-right:8px; padding:10px; border:2px solid blue;'>" 
          "<form method='POST'>"
            "<a class='texto'>O tambien puede seleccionar</a><br/><br/>"
            "<input type='submit' value='Colores automaticos' name='configAuto' class='boton' />&nbsp;&nbsp;"
            "<input type='number' name='modo' min='1' max='4' placeholder='1~4' class='brilloPixel' required/> <a><font size='5'>Modo</font></a> <br/><br/>"
          "</form>"
        
          "<form method='POST'>"
            "<input type='submit' value='Color Blanco Fijo' name='colorBlanco' class='boton' />&nbsp;&nbsp;"
            "<input type='number' name='brilloBlanco' min='0' max='100' placeholder='0~100%' class='brilloPixel' required/> <a><font size='5'>Brillo</font></a>"
          "</form>"
        "</div>"
        "</div>"
        
          "<style type='text/css'>"

            ".inputColor{"
                "border: 1px solid black;"
                "width: 100px;" 
                "height: 29px;"
                  "}"
            
            ".boton{"
                "font-size:25px;"
                "font-family:Arial;"
                "font-weight:bold;"
                "color:black;"
                "background:#b3b3cc;"
                "border:1px solid black;"
                  "}"

            ".brilloPixel{"
                "font-size:25px;"
                "font-family:Arial;"
                "font-weight:bold;"
                "color:black;"
                "background:#fff;"
                "border:2px solid black;"
                  "}"


            ".texto {"
                "font-size:22px;"
                "font-family:Arial;"
                "font-weight:bold;"
                "color: black;"
                  "}"
                  
          "</style>"

            "<datalist id='listacolores'>"

            "<option value='#00ddff'>"
            "<option value='#ff00ff'>"
            "<option value='#ffff00'>"
            "<option value='#ff4b01'>"
            "<option value='#2223ff'>"
            "<option value='#fe0101'>"
            "<option value='#24FE00'>"
            "<option value='#a95a20'>"

            "</datalist>"

        "</body>"
      "</html>";
    
    server.send(200, "text/html", formulario);

    if(server.arg("enviarColor") == "Enviar Color"){
      configAutomatica = false;
      String hexString = server.arg("selectorColor");
      colorEspecifico(hexString);
    }


    if(server.arg("configAuto") == "Colores automaticos"){
      int modo = server.arg("modo").toInt();
      configAutomatica = true;
      coloresAutomaticos(modo);
    }


    if(server.arg("colorBlanco") == "Color Blanco Fijo"){

      if(configAutomatica == false){
        int brilloBlanco = server.arg("brilloBlanco").toInt();
        colorBlancoEstatico(brilloBlanco);
      }
      else{
        configAutomatica = false;
      }
    }
      
  });

}








void colorEspecifico(String hexString){
  int espera = 100;
  int colorRojo;
  int colorVerde;
  int colorAzul; 

  unsigned long hexToDec(String hexString);
  unsigned long decValue = 0;
  int nextInt;


  for (int i = 0; i < hexString.length(); i++) {
      
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);


    switch(i){
      case 1: 
        decValue = (decValue * 16) + nextInt;
      break;

      case 2:
        decValue = (decValue * 16) + nextInt;
        Serial.print(decValue);
        Serial.print(" ");
        colorRojo = decValue;
        decValue = 0;
      break;

      case 3: 
        decValue = (decValue * 16) + nextInt;
      break;

      case 4:
        decValue = (decValue * 16) + nextInt;
        Serial.print(decValue);
        Serial.print(" ");
        colorVerde = decValue;
        decValue = 0;
      break;

      case 5:
        decValue = (decValue * 16) + nextInt;
      break;

      case 6:
        decValue = (decValue * 16) + nextInt;
        Serial.println(decValue);
        Serial.print(" ");
        colorAzul = decValue;
        decValue = 0;
      break;
    
    }
  }
      

  for(int i=0;i < cantidadPixel;i++){
    tira.setBrightness(255);
    tira.setPixelColor(i, colorRojo, colorVerde, colorAzul);// posicion, R, G, B
    tira.show();
    delay(espera); 
  }

}







void coloresAutomaticos(int modo){
 int tiempoDelay = 100;
 tira.setBrightness(255);
 

 while(configAutomatica == true){

    if(modo == 1){
      int veces = 0;

      if(veces == 0)
      {
        for(int i=0;i < cantidadPixel;i++){
          tira.setPixelColor(i, 255, 0, 0);// posicion, R, G, B
        }
        tira.show();
        delay(tiempoDelay);

        veces = 1;
      }


      ///////////////////////////////////Arcoiris////////////////////////////////////// 
      for(int a=0;a < 255;a++){

        for(int i=0;i < cantidadPixel;i++){
          tira.setPixelColor(i, 255, a, 0);// posicion, R, G, B
        }
        tira.show();
        delay(tiempoDelay);
      }

      if(comprobacionCliente() == true){
        break;
      }
      server.handleClient();
      if(configAutomatica == false){
        break;
      }

        
      for(int a=255;a > 0;a--){

        for(int i=0;i < cantidadPixel;i++){
          tira.setPixelColor(i, a, 255, 0);// posicion, R, G, B
        }
        tira.show();
        delay(tiempoDelay);
      }

      if(comprobacionCliente() == true){
        break;
      }
      comprobacionCliente();
      server.handleClient();
      if(configAutomatica == false){
        break;
      }
        
        
      for(int a=0;a < 255;a++){

        for(int i=0;i < cantidadPixel;i++){
          tira.setPixelColor(i, 0, 255, a);// posicion, R, G, B
        }
        tira.show();
        delay(tiempoDelay);
      }

      if(comprobacionCliente() == true){
        break;
      }
      comprobacionCliente();
      server.handleClient();
      if(configAutomatica == false){
        break;
      }

        
      for(int a=255;a > 0;a--){

        for(int i=0;i < cantidadPixel;i++){
          tira.setPixelColor(i, 0, a, 255);// posicion, R, G, B
        }
        tira.show();
        delay(tiempoDelay);
      }

      if(comprobacionCliente() == true){
        break;
      }
      comprobacionCliente();
      server.handleClient();
      if(configAutomatica == false){
        break;
      }
        
        
      for(int a=0;a < 255;a++){

        for(int i=0;i < cantidadPixel;i++){
          tira.setPixelColor(i, a, 0, 255);// posicion, R, G, B
        } 
        tira.show();
        delay(tiempoDelay);
      }


      if(comprobacionCliente() == true){
        break;
      }
      comprobacionCliente();
      server.handleClient();
      if(configAutomatica == false){
        break;
      }
        
        
      for(int a=255;a > 0;a--){

        for(int i=0;i < cantidadPixel;i++){
          tira.setPixelColor(i, 255, 0, a);// posicion, R, G, B
        }
        tira.show();
        delay(tiempoDelay);
      }
    }



    /////////////////////////////Parpadeo//////////////////////////////////////
    if(modo == 2){
      parpadeo();
      server.handleClient();

      if(configAutomatica == false){
        break;
      }
    }



    ////////////////////////////Circunferencia//////////////////////////////////
    if(modo == 3){
      circunferencia();
      server.handleClient();

      if(configAutomatica == false){
        break;
      }
    }




    ////////////////////////////Discoteca//////////////////////////////////
    if(modo == 4){
      cambioRGB();
      server.handleClient();
      
      if(configAutomatica == false){
        break;
      }
    }
  }
}




void colorBlancoEstatico(int brillo){
  float brilloLed = (brillo*255.00)/100.00;

  for(int i=0;i < cantidadPixel;i++){
    tira.setBrightness(brilloLed);
    tira.setPixelColor(i, 255, 255, 255);// posicion, R, G, B
    tira.show();
    delay(100);
  }
}





void parpadeo(){
  int espera = 1000;
  int rojo;
  int verde;
  int azul;
  
  rojo = random(0,255);
  verde = random(0, 255);
  azul = random(0, 255);
    
  for(int i=0;i < cantidadPixel;i++){
    tira.setPixelColor(i, rojo, verde, azul);// posicion, R, G, B 
  }
  tira.show();
    
  delay(espera);
     
  for(int i=0;i < cantidadPixel;i = i + 2){
    tira.setPixelColor(i, 0, 0, 0);// posicion, R, G, B 
  }
  tira.show();
}





void circunferencia(){
  int tiempo = 80;
  int rojo = random(0,255);
  int verde = random(0,255);
  int azul = random(0,255);
    
  for(int i=0;i < cantidadPixel;i++){
    tira.setBrightness(255);
    tira.setPixelColor(i, rojo, verde, azul);// posicion, R, G, B
    tira.show();
    delay(tiempo);
  }

  delay(500);
    
  for(int i=cantidadPixel;i > 0;i--){
    tira.setPixelColor(i, 0, 0, 0);// posicion, R, G, B
    tira.show();
    delay(tiempo);
  }
}





void cambioRGB(){
  int tiempo = 1000;
  
  for(int i=0;i < cantidadPixel;i++){
    tira.setPixelColor(i, 255, 0, 0);// posicion, R, G, B
  }
  tira.show();

  delay(tiempo);

  for(int i=0;i < cantidadPixel;i++){
    tira.setPixelColor(i, 0, 255, 0);// posicion, R, G, B
  }
  tira.show();

  delay(tiempo);

  for(int i=0;i < cantidadPixel;i++){
    tira.setPixelColor(i, 0, 0, 255);// posicion, R, G, B
  }
  tira.show();

  delay(tiempo);
}
