
# Projeto: Trava eletrônica com MQTT

![Projeto montado](https://raw.githubusercontent.com/AndersonK22/Projeto-trava/main/Imagem/ProjetoMontado.jpg)




## Descrição

Este projeto foi desenvolvido a partir de um problema de segurança, mais precisamente a invasão de propriedade.

Sabendo disso, o projeto baseia-se no uso de um sensor ultrassónico e **NodeMCU** (para interações relacionadas ao IoT), o sensor, que utiliza ondas de alta frequências para detectar a distancia de objetos e consequentemente capaz de detectar alguém que passe em frente ao sensor. Neste projeto, o sensor será utilizado como gatilho para acionar o alarme.
Também foi implementado o uso de um micro servo motor, que simula um trinco, para quando o sistema estiver ativo o trinco mantenha a porta trancada.

Assim que o sistema estiver ativo, será necessário apenas que o sensor detecte a presença de uma pessoa, que assim acionará o alarme, fazendo o sistema emitir um aviso ao morador da residência( notificado através do celular como uso do app chamado IoT MQTT Panel) juntamente com o disparo de aviso sonoro para espantar e alertar indivíduos próximos.

## Código
O código foi montado e compilado no [Arduino IDE](https://www.arduino.cc/en/software).
Além deste software, também foram utilizadas 3 bibliotecas externas:

 - _[PubSubClient](https://github.com/knolleary/pubsubclient)_- Permite a conexão e utilização do protocolo MQTT.
 - [SimpleKeypad](https://github.com/maximebohrer/SimpleKeypad) - Permite a conexão e utilização do Keypad 3x4.
 - [LCD_I2C](https://github.com/blackhack/LCD_I2C) - Permite a conexão e utilização de um display LCD 16x2 em conjunto com um módulo I2C.

## Componentes utilizados

 - ESP8266 NodeMcu ESP-12E
 - Arduino Nano
 - Módulo Serial I2C
 - Display Tela Lcd 16x2
 - Keypad 3x4
 - HC-SR04 Sensor ultrassónico 
 - Micro Servomotor
 - Buzzer 95db
 - Led azul
 - Resistor 1kΩ
 - Arduino IDE
 - IoT MQTT Panel( Android )

## Conexão MQTT
Foi utilizado o aplicativo [IoT MQTT Panel](https://play.google.com/store/apps/details?id=snr.lab.iotmqttpanel.prod&hl=pt_BR&gl=US), disponível em dispositivos android, com uma interface simples é possível criar e configurar seu próprio layout, podendo inserir botões e até dar input, a configuração, assim como sua interface é simples.

Para que o **NodeMCU** , conecte-se no WiFi da forma correta é necessário mudar o SSID e sua senha para ter acesso a rede WiFi desejada, no começo do [código](https://github.com/AndersonK22/Projeto-trava/blob/main/Codigo/projetoTrava.ino#:~:text=const%20char*%20SSID%20=%20%22trava%22;%20//%20SSID%20/%20nome%20da%20rede,da%20rede%20WI-FI%20que%20deseja%20se%20conectar), essas informações podem ser alteradas. Caso esteja com problemas com o **NodeMCU** em conectar com a rede WiFi, pode-se utilizar a ancoragem WiFi dos smartphones para criar a conexão entre o dispositivo móvel e o **NodeMCU**

## Sketch final
![Sketch](https://raw.githubusercontent.com/AndersonK22/Projeto-trava/main/Imagem/Sketch_Projeto.png)


## **Integrantes do grupo:**

 - **Anderson Kenzo Arakaki - 31959008**    
 - **Gustavo Lepore -31944671**
