#include <EEPROM.h>
//Variáveis e objetos globais
#include <WiFiUdp.h>
#include <ESP8266WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h>
#define TOPICO_STATUS  "projetoTravaStatus"     //tópico MQTT de escuta
#define TOPICO_SENHA "projetoTravaSenhas"     //tópico MQTT de escuta
#define ID_MQTT  "projetoTrava123"     //id mqtt (para identificação de sessão)
const char* SSID = "trava"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "0123456789"; // Senha da rede WI-FI que deseja se conectar

const char* BROKER_MQTT = "broker.emqx.io"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT

// Importa a Biblioteca PubSubClient

#include <LCD_I2C.h>
LCD_I2C lcd(0x27); // Endereço Display 16x2
#include <SimpleKeypad.h>
#include <Servo.h>
Servo myservo;  // Objeto para controlar o servo
byte blinklocation = 9;
byte wrongCount;
const byte nb_rows = 4;                         // Linhas
const byte nb_cols = 3;                         // Colunas
char key_chars[nb_rows][nb_cols] = {            
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
#define PINO_ALARME 10
#define LIGA_ALARME  digitalWrite(PINO_ALARME,1)
#define DESLIGA_ALARME  digitalWrite(PINO_ALARME,0)
uint16_t listaDeSenhas[10];

byte rowPins[nb_rows] = {D0, D3, D4, D5};         // Pinos conectados as linhas
byte colPins[nb_cols] = {D6, D7, 9};           // Pinos conectados as colunas
SimpleKeypad kp1((char*)key_chars, rowPins, colPins, nb_rows, nb_cols);   // New keypad called kp1

bool DELETAR;
uint32_t Senha = 4444;
bool ADD;
bool OPEN;
bool ALARME;
//const int trigPin = 2;  //D4
//const int echoPin = 0;  //D3
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient


void setup() {

  Serial.begin(9600);
  myservo.attach(15);
  EEPROM.begin(40);
  DESLIGA_ALARME;
  uint16_t  type;
  for (byte i = 0 ; i < 10; i++) {
    listaDeSenhas[i] =  EEPROM.get(i, type);
    Serial.println( listaDeSenhas[i]);
    Serial.println(i);
  }
  //    pinMode(PINO_IMA, INPUT_PULLUP);
  pinMode(PINO_ALARME, OUTPUT);
  //  pinMode(6, INPUT_PULLUP);
  lcd.begin();         
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(2, 0); 
  lcd.print("INSIRA SENHA");
  reconectWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
  MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}

void loop() {


  //Serial.pritnln(lerDistancia())
  //se não há conexão com o WiFI, a conexão é refeita
  reconectWiFi();
  //se não há conexão com o Broker, a conexão é refeita
  reconnectMQTT();
  //envia o status de todos os outputs para o Broker no protocolo esperado
  //keep-alive da comunicação com broker MQTT
  MQTT.loop();

  char key = kp1.getKey();                      
  if (key <= '9' && key >= '0') {
    Serial.print("key "); Serial.println(key);
    byte v = key - 48;
    uint32_t n;
    uint32_t number_to_drop;
    uint32_t newnumber;
    n = ((Senha * 10) + v );
    number_to_drop = n / pow(10, 4);
    Senha = n - (number_to_drop * pow(10, 4));
    lcd.setCursor( blinklocation, 1);
    lcd.print("*");
    blinklocation--;


    if ( blinklocation == 5) { //caso inserimos 4 numeros
      delay(1000);
      blinklocation = 9;
      //verificar se senha existe
      for (int i = 0; i < 10; i++) {
        if (Senha == listaDeSenhas[i])   OPEN = 1;
      }
      if (OPEN == 1) {
        myservo.write(0);
        delay(1000);
        myservo.write(180);
        ALARME = false;
        lcd.clear();
        lcd.print("ABERTO");
        DESLIGA_ALARME;
        delay(1000);
        lcd.clear();
        //while (IMA == 0) {     delay(10);}
        lcd.print("INSIRA SENHA");
        OPEN = 0;
      }
      else {//caso senha seja errada
        wrongCount++;
        lcd.clear();
        lcd.print("ERRADO!");
        delay(1000 * wrongCount);
        lcd.clear();
        lcd.print("INSIRA SENHA");

        if (wrongCount == 3 ) { //depois de 3 erros
          wrongCount = 0;
          MQTT.publish(TOPICO_STATUS, "INVASOR! Envie 8888 para deshabilitar alarme");
          LIGA_ALARME;
          lcd.clear();
          lcd.print("!!!!!!!!!!!!!");
          OPEN = 0;
          delay(1000);
          ALARME = true;
        }
      }

    }
  }


  if (analogRead(A0) > 400) {
    ALARME = true;
    LIGA_ALARME;
    MQTT.publish(TOPICO_STATUS, "INVASOR! insira senha deshabilitar alarme");
    lcd.clear();
    lcd.print("!!!!!!!!!!!!!");
    OPEN = 0;
    delay(1000);
  }

  Serial.println(A0);


}


//
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String hdr;
  for (int i = 0; i < 14; i++)
  {
    char c = (char)topic[i];
    hdr += c;
  }

  String msg;
  //obtem a string do payload recebido
  for (int i = 0; i < length; i++)
  {
    char c = (char)payload[i];
    msg += c;
    Serial.print(c);
  }

  //definir o que fazer com os valores recebidos
  int n = msg.toInt();

  Serial.println(n + 11);

  if (n == 9999) {
    ADD = 1;
    MQTT.publish(TOPICO_STATUS, "ADICIONE NOVA SENHA");
  }

  if (ADD == 1 && n != 9999 && n != 7777) {
    for (int i = 0; i < 10; i++) {
      if (listaDeSenhas[i] == 65535) {
        listaDeSenhas[i] = n;
        EEPROM.put(i, n);
        EEPROM.commit();
        break;
      }

    }
    MQTT.publish(TOPICO_STATUS, "SENHA ADICIONADA!");
    ADD = 0;
  }

  if (n == 7777) {
    DELETAR = 1;
    MQTT.publish(TOPICO_STATUS, "DELETAR PROXIMA SENHA");
  }

  if (DELETAR == 1 && n != 9999 && n != 7777) {
    for (int i = 0; i < 10; i++) {
      if (n == listaDeSenhas[i]) {
        listaDeSenhas[i] = 65535;
        EEPROM.put(i, listaDeSenhas[i]);
        EEPROM.commit();
        break;
      }
    }
    DELETAR = 0;
    MQTT.publish(TOPICO_STATUS, "SENHA DELETADA!");
  }



  if (n != 7777 && n != 9999) {
    for (int i = 0; i < 10; i++) {
      Serial.println( listaDeSenhas[i]);
      if (n == listaDeSenhas[i]) {
        DESLIGA_ALARME;
        MQTT.publish(TOPICO_STATUS, "SENHA ACEITA DESLIGANDO ALARME!");
        break;
      }
    }
  }



}

void reconnectMQTT() {
  while (!MQTT.connected())
  {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT))
    {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe(TOPICO_SENHA);
    }
    else
    {
      Serial.println("Falha ao reconectar no broker.");
      delay(1000);
    }
  }
}
void reconectWiFi() {
  //se já está conectado a rede WI-FI, nada é feito.
  //Caso contrário, são efetuadas tentativas de conexão
  if (WiFi.status() == WL_CONNECTED)    return;
  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(SSID);
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());
}
