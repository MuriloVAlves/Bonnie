/*****************************************************
                    TCC Bonnie
                    Grupo:1
  Guilherme Pinheiro Pozzi
  Guilherme Santos Coelho
  Murilo Valentim Alves
  Falta terminar:
  Colocar uma tela de data e dia da semana
  Integração com um app de computador
  Arrumar o delay do botão
  codigo exemplo para programar o app Bonnie:
   10:15:05!09@10#19$2 para ajuste de horario
  !00:00/ para hora1
  @00:00/ para hora2
  #00:00/ para hora3
  (00:00/ para hora4
  )00:00/ para hora5
  %1/ para o timer
*****************************************************/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#define BTback  6
#define BTenter 5
#define BTfwrd  4
#define DS1307_ADDRESS 0x68

byte zero = 0x00;

Servo myservo;
byte pos;
byte minat;//minuto atual
byte horaat;//hora atual
byte numref = EEPROM.read(0);//variável para auxiliar no numero de refeições
byte hora1 = EEPROM.read(1);//variável hora da refeição 1
byte min1 = EEPROM.read(2); //variável minuto da refeição 1
byte hora2 = EEPROM.read(3);//variável hora da refeição 2
byte min2 = EEPROM.read(4); //variável minuto da refeição 2
byte hora3 = EEPROM.read(5);//variável hora da refeição 3
byte min3 = EEPROM.read(6); //variável minuto da refeição 3
byte hora4 = EEPROM.read(7);//variável hora da refeição 4
byte min4 = EEPROM.read(8); //variável minuto da refeição 4
byte hora5 = EEPROM.read(9);//variável hora da refeição 5
byte min5 = EEPROM.read(10); //variável minuto da refeição 5
byte tipo = EEPROM.read(11);//variável tipo de cão
byte tela;//variável entre as telas do display
byte estado;//variável auxiliar para os estados de configuração do horário
int BTBackAnt;//estado anterior dos botões
int BTEnterAnt;
int BTFwrdAnt;
byte posmin = 9;
byte poshor = 6;
//Variáveis para o relógio
byte segundos = 0;//Valores de 0 a 59
byte minutos = 0;//Valores de 0 a 59
byte horas = 0; //Valores de 0 a 23
byte diadasemana = 0; //Valores de 0 a 6 - 0=Domingo, 1 = Segunda, etc.
byte diadomes = 12; //Valores de 1 a 31
byte mes = 10; //Valores de 1 a 12
byte ano = 19; //Valores de 0 a 99
byte segundosAntes;
float timer = 0;

//Configurações do display e do I2C
//I2C Adress: 0x27
//Formato do Display:16x2
//Tempo para cães de pequeno porte -- 430
//Tempo para cães de médio porte   -- 530
//Tempo para cães de grande porte  -- 610
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial Bluetooth(10, 11);// RX pino 10 , TX pino 11 -- No Arduino
void SelecionaDataeHora()   //Seta a data e a hora do DS1307
{
  /*byte segundos = 00; //Valores de 0 a 59
    byte minutos = 00; //Valores de 0 a 59
    byte horas = 00; //Valores de 0 a 23
    byte diadasemana = 2; //Valores de 0 a 6 - 0=Domingo, 1 = Segunda, etc.
    byte diadomes = 13; //Valores de 1 a 31
    byte mes = 8; //Valores de 1 a 12
    byte ano = 19; //Valores de 0 a 99
  */
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); //Stop no CI para que o mesmo possa receber os dados

  //As linhas abaixo escrevem no CI os valores de
  //data e hora que foram colocados nas variaveis acima
  Wire.write(ConverteParaBCD(segundos));
  Wire.write(ConverteParaBCD(minutos));
  Wire.write(ConverteParaBCD(horas));
  Wire.write(ConverteParaBCD(diadasemana));
  Wire.write(ConverteParaBCD(diadomes));
  Wire.write(ConverteParaBCD(mes));
  Wire.write(ConverteParaBCD(ano));
  Wire.write(zero); //Start no CI
  Wire.endTransmission();
}
void Mostrarelogio()
{
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
  segundos = ConverteparaDecimal(Wire.read());
  minutos = ConverteparaDecimal(Wire.read());
  horas = ConverteparaDecimal(Wire.read() & 0b111111);
  diadasemana = ConverteparaDecimal(Wire.read());
  diadomes = ConverteparaDecimal(Wire.read());
  mes = ConverteparaDecimal(Wire.read());
  ano = ConverteparaDecimal(Wire.read());
  //Mostra a data no Serial Monitor
  /*Serial.print F("Data: ");
    Serial.print(diadomes);
    Serial.print F("/");
    Serial.print(mes);
    Serial.print F("/");
    Serial.print(ano);
    Serial.print F(" ");
    Serial.print F("Hora : ");
    Serial.print(horas);
    Serial.print F(":");
    Serial.print(minutos);
    Serial.print F(":");
    Serial.print(segundos);
    switch (diadasemana)
    {
    case 0: Serial.println F(", Domingo");
      break;
    case 1: Serial.println F(", Segunda");
      break;
    case 2: Serial.println F(", Terca");
      break;
    case 3: Serial.println F(", Quarta");
      break;
    case 4: Serial.println F(", Quinta");
      break;
    case 5: Serial.println F(", Sexta");
      break;
    case 6: Serial.println F(", Sabado");
    }*/
}
byte ConverteParaBCD(byte val) { //Converte o número de decimal para BCD
  return ( (val / 10 * 16) + (val % 10) );
}

byte ConverteparaDecimal(byte val)  { //Converte de BCD para decimal
  return ( (val / 16 * 10) + (val % 16) );
}
void ModAbertura(byte porte = 0, bool mod = false)
{
  if (mod == true) {
    tipo = porte;
    EEPROM.update(11, porte);
  }
  switch (tipo) {
    case 1: timer = 430;
      break;
    case 2: timer = 530;
      break;
    case 3: timer = 610;
      break;
  }
}
void EstadoBotoes()
{
  BTEnterAnt = digitalRead(BTenter);
  BTBackAnt   = digitalRead(BTback);
  BTFwrdAnt = digitalRead(BTfwrd);
}
void AtualizaDisplay()
{
  if (tela == 0)
  {
    bool unimin;
    lcd.setCursor(5, 0); //configuração da tela de início na primeira linha
    lcd.print F("BONNIE");
    lcd.setCursor(7, 1);
    lcd.print F(":");
    if (minutos <= 9 && posmin <= 8)
    {
      posmin = 9;
      unimin = true;
    }
    if (minutos > 9 && posmin == 9)
    {
      posmin = 8;
      unimin = false;
    }
    if (horas <= 9 && poshor <= 5)
    {
      poshor = 6;
    }
    if (horas > 9 && poshor == 6)
    {
      poshor = 5;
    }
    if (unimin == true)
    {
      lcd.setCursor(8, 1);
      lcd.print F("0");
    }
    lcd.setCursor(poshor, 1);
    lcd.print(horas);
    lcd.setCursor(posmin, 1);
    lcd.print(minutos);
  }
  if (tela == 1)
  {
    lcd.setCursor(3, 0);
    lcd.print F("REFEICOES");
    lcd.setCursor(7, 1);
    lcd.print F("0");
    lcd.print(numref);
  }
  if (tela == 2)
  {
    bool unimin;
    lcd.setCursor(6, 0); //configuração da tela de início na primeira linha
    lcd.print F("HORA");
    lcd.setCursor(7, 1);
    lcd.print F(":");
    bool unihora;
    if (minat <= 9 && posmin <= 8)
    {
      posmin = 9;
      unimin = true;
    }
    if (minat > 9 && posmin == 9)
    {
      posmin = 8;
      unimin = false;
    }
    if (horaat <= 9 && poshor <= 5)
    {
      poshor = 6;
      unihora = true;
    }
    if (horaat > 9 && poshor == 6)
    {
      poshor = 5;
      unihora = false;
    }
    if (unimin == true)
    {
      lcd.setCursor(8, 1);
      lcd.print F("0");
    }
    if (unihora == true)
    {
      lcd.setCursor(5, 1);
      lcd.print F("0");
    }
    lcd.setCursor(poshor, 1);
    lcd.print(horaat);
    lcd.setCursor(posmin, 1);
    lcd.print(minat);
  }

  if (tela == 3)
  {

    bool unimin;
    lcd.setCursor(5, 0); //configuração da tela de início na primeira linha
    lcd.print F("HORA 1");
    lcd.setCursor(7, 1);
    lcd.print F(":");
    bool unihora;
    if (min1 <= 9 && posmin <= 8)
    {
      posmin = 9;
      unimin = true;
    }
    if (min1 > 9 && posmin == 9)
    {
      posmin = 8;
      unimin = false;
    }
    if (hora1 <= 9 && poshor <= 5)
    {
      poshor = 6;
      unihora = true;
    }
    if (hora1 > 9 && poshor == 6)
    {
      poshor = 5;
      unihora = false;
    }
    if (unimin == true)
    {
      lcd.setCursor(8, 1);
      lcd.print F("0");
    }
    if (unihora == true)
    {
      lcd.setCursor(5, 1);
      lcd.print F("0");
    }
    lcd.setCursor(poshor, 1);
    lcd.print(hora1);
    lcd.setCursor(posmin, 1);
    lcd.print(min1);
  }

  if (tela == 4)
  {
    bool unimin;
    bool unihora;
    lcd.setCursor(5, 0); //configuração da tela de início na primeira linha
    lcd.print F("HORA 2");
    lcd.setCursor(7, 1);
    lcd.print F(":");
    if (min2 <= 9 && posmin <= 8)
    {
      posmin = 9;
      unimin = true;
    }
    if (min2 > 9 && posmin == 9)
    {
      posmin = 8;
      unimin = false;
    }
    if (hora2 <= 9 && poshor <= 5)
    {
      poshor = 6;
      unihora = true;
    }
    if (hora2 > 9 && poshor == 6)
    {
      poshor = 5;
      unihora = false;
    }
    if (unimin == true)
    {
      lcd.setCursor(8, 1);
      lcd.print F("0");
    }
    if (unihora == true)
    {
      lcd.setCursor(5, 1);
      lcd.print F("0");
    }
    lcd.setCursor(poshor, 1);
    lcd.print(hora2);
    lcd.setCursor(posmin, 1);
    lcd.print(min2);
  }

  if (tela == 5)
  {
    bool unimin;
    bool unihora;
    lcd.setCursor(5, 0); //configuração da tela de início na primeira linha
    lcd.print F("HORA 3");
    lcd.setCursor(7, 1);
    lcd.print F(":");
    if (min3 <= 9 && posmin <= 8)
    {
      posmin = 9;
      unimin = true;
    }
    if (min3 > 9 && posmin == 9)
    {
      posmin = 8;
      unimin = false;
    }
    if (hora3 <= 9 && poshor <= 5)
    {
      poshor = 6;
      unihora = true;
    }
    if (hora3 > 9 && poshor == 6)
    {
      poshor = 5;
      unihora = false;
    }
    if (unimin == true)
    {
      lcd.setCursor(8, 1);
      lcd.print F("0");
    }
    if (unihora == true)
    {
      lcd.setCursor(5, 1);
      lcd.print F("0");
    }
    lcd.setCursor(poshor, 1);
    lcd.print(hora3);
    lcd.setCursor(posmin, 1);
    lcd.print(min3);
  }
  if (tela == 6)
  {
    bool unimin;
    bool unihora;
    lcd.setCursor(5, 0); //configuração da tela de início na primeira linha
    lcd.print F("HORA 4");
    lcd.setCursor(7, 1);
    lcd.print F(":");
    if (min4 <= 9 && posmin <= 8)
    {
      posmin = 9;
      unimin = true;
    }
    if (min4 > 9 && posmin == 9)
    {
      posmin = 8;
      unimin = false;
    }
    if (hora4 <= 9 && poshor <= 5)
    {
      poshor = 6;
      unihora = true;
    }
    if (hora4 > 9 && poshor == 6)
    {
      poshor = 5;
      unihora = false;
    }
    if (unimin == true)
    {
      lcd.setCursor(8, 1);
      lcd.print F("0");
    }
    if (unihora == true)
    {
      lcd.setCursor(5, 1);
      lcd.print F("0");
    }
    lcd.setCursor(poshor, 1);
    lcd.print(hora4);
    lcd.setCursor(posmin, 1);
    lcd.print(min4);
  }

  if (tela == 7)
  {
    bool unimin;
    bool unihora;
    lcd.setCursor(5, 0); //configuração da tela de início na primeira linha
    lcd.print F("HORA 5");
    lcd.setCursor(7, 1);
    lcd.print F(":");
    if (min5 <= 9 && posmin <= 8)
    {
      posmin = 9;
      unimin = true;
    }
    if (min5 > 9 && posmin == 9)
    {
      posmin = 8;
      unimin = false;
    }
    if (hora5 <= 9 && poshor <= 5)
    {
      poshor = 6;
      unihora = true;
    }
    if (hora5 > 9 && poshor == 6)
    {
      poshor = 5;
      unihora = false;
    }
    if (unimin == true)
    {
      lcd.setCursor(8, 1);
      lcd.print F("0");
    }
    if (unihora == true)
    {
      lcd.setCursor(5, 1);
      lcd.print F("0");
    }
    lcd.setCursor(poshor, 1);
    lcd.print(hora5);
    lcd.setCursor(posmin, 1);
    lcd.print(min5);
  }
  if (tela == 8)
  {
    lcd.setCursor(6, 0); //configuração da tela de início na primeira linha
    lcd.print F("PORTE");
    lcd.setCursor(7, 1);
    String texto;
    switch (tipo)
    {
      case 1:
        texto = "PEQ";
        break;
      case 2:
        texto = "MED";
        break;
      case 3:
        texto = "GRA";
        break;
    }
    lcd.print(texto);
  }
}
byte BTMenu(byte x, bool hora)
{
  EstadoBotoes();
  AtualizaDisplay();
  if ((BTback != BTBackAnt) && (digitalRead(BTback) == LOW)) {
    x = x - 1;
    if (x == 255 && hora == true)
    {
      x = 23;
    }
    if (x == 255 && hora == false)
    {
      x = 59;     
    }
    delay(200);
    AtualizaDisplay();
  }
  if (BTfwrd != BTFwrdAnt && digitalRead(BTfwrd) == LOW) {
    x = x + 1;
    if (hora == false)
    {
      if (x > 59)
      {
        x = 0;
      }
    }
    if (hora == true)
    {
      if (x > 23)
      {
        x = 0;
      }
    }
    delay(200);
    AtualizaDisplay();
  }
  if (BTenter != BTEnterAnt && digitalRead(BTenter) == LOW)
  {
    estado = estado + 1;
    delay(100);
    AtualizaDisplay();
  }
  return x;
}
void racao () {
  if (numref >= 1) {
    if (horas == hora1) {
      if (minutos == min1) {
        if (segundos == 0) {
          //abre a porta
          myservo.write(155);
          delay(15);
          delay (timer);//delay de 2 segundos
          //fecha a porta
          myservo.write(15);
          delay(15);
        }
      }
    }
  }
  if (numref >= 2) {
    if (horas == hora2) {
      if (minutos == min2) {
        if (segundos == 0) {
          //abre a porta
          myservo.write(155);
          delay(15);
          delay(timer);//delay de 2 segundos
          //fecha a porta
          myservo.write(15);
          delay(15);
        }
      }
    }
  }
  if (numref >= 3) {
    if (horas == hora3) {
      if (minutos == min3) {
        if (segundos == 0) {
          //abre a porta
          myservo.write(155);
          delay(15);
          delay(timer);//delay de 2 segundos
          //fecha a porta
          myservo.write(15);
          delay(15);
        }
      }
    }
  }
  if (numref >= 4) {
    if (horas == hora4) {
      if (minutos == min4) {
        if (segundos == 0) {
          //abre a porta
          myservo.write(155);
          delay(15);
          delay(timer);//delay de 2 segundos
          //fecha a porta
          myservo.write(15);
          delay(15);
        }
      }
    }
  }
  if (numref >= 5) {
    if (horas == hora5) {
      if (minutos == min5) {
        if (segundos == 0) {
          //abre a porta
          myservo.write(155);
          delay(15);
          delay (timer);//delay de 2 segundos
          //fecha a porta
          myservo.write(15);
          delay(15);
        }
      }
    }
  }
}

//O código abaixo é um teste para uma integração com o aplicativo no PC
void Hora(String str1, int hora)
{
  String str2;
  String str3;
  str2 = str1.substring(0, 2);
  str3 = str1.substring(3, 5);
  switch (hora)
  {
    case 1:
      numref = hora;
      EEPROM.update(0, hora);
      hora1 = str2.toInt();
      min1 = str3.toInt();
      EEPROM.update(1, hora1);
      EEPROM.update(2, min1);
      break;
    case 2:
      numref = hora;
      EEPROM.update(0, hora);
      hora2 = str2.toInt();
      min2 = str3.toInt();
      EEPROM.update(3, hora2);
      EEPROM.update(4, min2);
      break;
    case 3:
      numref = hora;
      EEPROM.update(0, hora);
      hora3 = str2.toInt();
      min3 = str3.toInt();
      EEPROM.update(5, hora3);
      EEPROM.update(6, min3);
      break;
    case 4:
      numref = hora;
      EEPROM.update(0, hora);
      hora4 = str2.toInt();
      min4 = str3.toInt();
      EEPROM.update(7, hora1);
      EEPROM.update(8, min1);
      break;
    case 5:
      numref = hora;
      EEPROM.update(0, hora);
      hora5 = str2.toInt();
      min5 = str3.toInt();
      EEPROM.update(9, hora2);
      EEPROM.update(10, min2);
      break;
  }
}
String verificar()
{
  String comando;
  comando += numref;
  comando += "!";
  comando += hora1;
  comando += ":";
  comando += min1;
  comando += "@";
  comando += hora2;
  comando += ":";
  comando += min2;
  comando += "#";
  comando += hora3;
  comando += ":";
  comando += min3;
  comando += "(";
  comando += hora4;
  comando += ":";
  comando += min4;
  comando += ")";
  comando += hora5;
  comando += ":";
  comando += min5;
  comando += "%";
  comando += tipo;
  return comando;
}
void SelecionaSerial(String texto)
{
  String hora = texto.substring(0, 2);
  String minuto = texto.substring(3, 5);
  String segundo = texto.substring(6, 8);
  String dia = texto.substring(9, 11);
  String mesS = texto.substring(12, 14);
  String anoS = texto.substring(15, 17);
  String diaS = texto.substring(18, 19);
  horas = hora.toInt();
  minutos = minuto.toInt();
  segundos = segundo.toInt();
  diadasemana = diaS.toInt();
  diadomes = dia.toInt();
  mes = mesS.toInt();
  ano = anoS.toInt();
  SelecionaDataeHora();
}
void serial() {
  if (Bluetooth.available())
  { // Enquanto Bluetooth receber dados
    delay(10);
    while (Bluetooth.available())
    {
      char Read;
      String Texto = "";
      Read = Bluetooth.read();
      if (Read == '*')
      {
        Texto = Serial.readString();
        SelecionaSerial(Texto);
      }
      if (Read == '&')
      {

        Texto = verificar();
        Bluetooth.println(Texto);
        Serial.println(Texto);
      }
      if (Read == '!')
      {

        Texto = Bluetooth.readStringUntil('/');
        int Horarios = 1;
        Hora(Texto, Horarios);
      }

      if (Read == '@')
      {

        Texto = Bluetooth.readStringUntil('/');
        int Horarios = 2;
        Hora(Texto, Horarios);
      }

      if (Read == '#')
      {

        Texto = Bluetooth.readStringUntil('/');
        int Horarios = 3;
        Hora(Texto, Horarios);
      }
      if (Read == '(')
      {

        Texto = Bluetooth.readStringUntil('/');
        int Horarios = 4;
        Hora(Texto, Horarios);
      }

      if (Read == ')')
      {

        Texto = Bluetooth.readStringUntil('/');
        int Horarios = 5;
        Hora(Texto, Horarios);
      }
      if (Read == '%')
      {
        String s = Bluetooth.readStringUntil('/');
        ModAbertura(s.toInt(), true);
      }
    }
  }

  if (Serial.available())
  { // Enquanto a Serial receber dados
    delay(10);
    while (Serial.available())
    {
      char Read;
      String Texto = "";
      Read = Serial.read();
      if (Read == '*')
      {
        Texto = Serial.readString();
        SelecionaSerial(Texto);
      }
      if (Read == '&')
      {
        Texto = verificar();
        Serial.print(Texto);
      }
      if (Read == '!')
      {

        Texto = Serial.readStringUntil('/');
        int Horarios = 1;
        Hora(Texto, Horarios);
      }

      if (Read == '@')
      {

        Texto = Serial.readStringUntil('/');
        int Horarios = 2;
        Hora(Texto, Horarios);
      }

      if (Read == '#')
      {

        Texto = Serial.readStringUntil('/');
        int Horarios = 3;
        Hora(Texto, Horarios);
      }
      if (Read == '(')
      {

        Texto = Serial.readStringUntil('/');
        int Horarios = 4;
        Hora(Texto, Horarios);
      }

      if (Read == ')')
      {
        Texto = Serial.readStringUntil('/');
        int Horarios = 5;
        Hora(Texto, Horarios);
      }
      if (Read == '%')
      {
        String s = Serial.readStringUntil('/');
        ModAbertura(s.toInt(), true);
      }
    }
  }
}
void setup() {
  pinMode(BTback, INPUT_PULLUP); //botão de -
  pinMode(BTenter, INPUT_PULLUP); //botão de enter
  pinMode(BTfwrd, INPUT_PULLUP); //botão de +
  pinMode(13, OUTPUT);
  Wire.begin();
  Serial.begin(38400);
  Bluetooth.begin(38400);
  EstadoBotoes();
  //A linha abaixo pode ser retirada apos setar a data e hora
  //SelecionaDataeHora();
  myservo.attach(9);
  lcd.begin();
  lcd.backlight();
  tela = 0;
  myservo.write(15);
  delay(10);
  ModAbertura();
  Bluetooth.println(F("Bonnie Bluetooth"));

}
void loop()
{
  EstadoBotoes();
  while (tela == 0)
  {
    lcd.noBlink();
    EstadoBotoes();
    segundosAntes = segundos;
    Mostrarelogio();
    if (segundos != segundosAntes)
    {
      AtualizaDisplay();
    }
    racao();
    serial();
    if ((digitalRead(BTenter) != BTEnterAnt) && (digitalRead(BTenter) == LOW))
    {
      tela = 1;
      delay(200);
      EstadoBotoes();
    }
    EstadoBotoes();
  }
  horaat = horas;
  minat = minutos;
  lcd.clear();
  AtualizaDisplay();
  delay(1000);
  lcd.blink();
  while (tela == 1)
  {
    EstadoBotoes();
    if ((BTback != BTBackAnt) && (digitalRead(BTback) == LOW)) {
      numref = numref - 1;
      if (numref == 255)
      {
        numref = 5;
      }
      AtualizaDisplay();
      delay (200);
      EstadoBotoes();
    }
    if (BTfwrd != BTFwrdAnt && digitalRead(BTfwrd) == LOW) {
      numref = numref + 1;
      if (numref > 5)
      {
        numref = 1;
      }
      AtualizaDisplay();
      delay(200);
      EstadoBotoes();
    }
    AtualizaDisplay();
    if ((digitalRead(BTenter) != BTEnterAnt) && (digitalRead(BTenter) == LOW))
    {
      tela = 2;
      delay(200);
      EstadoBotoes();
    }
    EstadoBotoes();
  }
  lcd.noBlink();
  lcd.clear();
  AtualizaDisplay();
  delay(500);
  lcd.blink();
  EEPROM.update(0, numref);
  while (tela == 2)
  {
    EstadoBotoes();
    while (estado == 0)
    {
      horaat = BTMenu(horaat, true);
      lcd.setCursor(6, 1);
      EstadoBotoes();
    }
    EstadoBotoes();
    delay(500);
    while (estado == 1)
    {
      minat = BTMenu(minat, false);
      EstadoBotoes();
      lcd.setCursor(9, 1);
    }
    EstadoBotoes();
    delay(500);
    while (estado == 2)
    {
      horas = horaat;
      minutos = minat;
      segundos = 0;
      SelecionaDataeHora();
      estado = 0;
      tela = tela + 1;
    }
    delay(100);
  }
  lcd.noBlink();
  lcd.clear();
  AtualizaDisplay();
  delay(500);
  lcd.blink();
  while (tela == 3)
  {
    EstadoBotoes();
    AtualizaDisplay();
    while (estado == 0)
    {
      hora1 = BTMenu(hora1, true);
      lcd.setCursor(6, 1);
      EstadoBotoes();
    }
    EstadoBotoes();
    delay(500);
    while (estado == 1)
    {
      min1 = BTMenu(min1, false);
      lcd.setCursor(9, 1);
      EstadoBotoes();
    }
    EstadoBotoes();
    delay(500);
    while (estado == 2)
    {
      estado = 0;
      tela = tela + 1;
      EEPROM.update(1, hora1);
      EEPROM.update(2, min1);
    }
    delay(100);
  }
  lcd.noBlink();
  lcd.clear();
  delay(200);
  lcd.blink();
  while (tela == 4)
  {
    if (numref < 2)
    {
      estado = 2;
    }
    EstadoBotoes();
    while (estado == 0)
    {
      hora2 = BTMenu(hora2, true);
      lcd.setCursor(6, 1);
      EstadoBotoes();
    }
    EstadoBotoes();
    delay(500);
    while (estado == 1)
    {
      min2 = BTMenu(min2, false);
      lcd.setCursor(9, 1);
      EstadoBotoes();
    }
    EstadoBotoes();
    delay(500);
    while (estado == 2)
    {
      estado = 0;
      tela = tela + 1;
      EEPROM.update(3, hora2);
      EEPROM.update(4, min2);
    }
    delay(100);
  }
  lcd.noBlink();
  lcd.clear();
  delay(200);
  lcd.blink();
  while (tela == 5)
  {
    if (numref < 3)
    {
      estado = 2;
    }
    EstadoBotoes();
    while (estado == 0)
    {
      hora3 = BTMenu(hora3, true);
      lcd.setCursor(6, 1);
      EstadoBotoes();
    }
    EstadoBotoes();
    delay(500);
    while (estado == 1)
    {
      min3 = BTMenu(min3, false);
      lcd.setCursor(9, 1);
      EstadoBotoes();
    }
    EstadoBotoes();
    delay(500);
    while (estado == 2)
    {
      estado = 0;
      tela = tela + 1;
      EEPROM.update(5, hora3);
      EEPROM.update(6, min3);
    }
    delay(100);
  }
  lcd.noBlink();
  lcd.clear();
  delay(200);
  lcd.blink();
  while (tela == 6)
  {
    if (numref < 4)
    {
      estado = 2;
    }
    EstadoBotoes();
    while (estado == 0)
    {
      hora4 = BTMenu(hora2, true);
      lcd.setCursor(6, 1);
      EstadoBotoes();
    }
    EstadoBotoes();
    delay(500);
    while (estado == 1)
    {
      min4 = BTMenu(min2, false);
      lcd.setCursor(9, 1);
      EstadoBotoes();
    }
    EstadoBotoes();
    delay(500);
    while (estado == 2)
    {
      estado = 0;
      tela = tela + 1;
      EEPROM.update(7, hora2);
      EEPROM.update(8, min2);
    }
    delay(100);
  }
  lcd.noBlink();
  lcd.clear();
  delay(200);
  lcd.blink();
  while (tela == 7)
  {
    if (numref < 5)
    {
      estado = 2;
    }
    EstadoBotoes();
    AtualizaDisplay();
    while (estado == 0)
    {
      hora5 = BTMenu(hora3, true);
      lcd.setCursor(6, 1);
      EstadoBotoes();
    }
    EstadoBotoes();
    delay(500);
    while (estado == 1)
    {
      min5 = BTMenu(min3, false);
      lcd.setCursor(9, 1);
      EstadoBotoes();
    }
    EstadoBotoes();
    delay(500);
    while (estado == 2)
    {
      estado = 0;
      tela = tela + 1;
      EEPROM.update(9, hora3);
      EEPROM.update(10, min3);
    }
    delay(100);
  }
  lcd.noBlink();
  lcd.clear();
  AtualizaDisplay();
  delay(500);
  lcd.blink();
  while (tela == 8)
  {
    AtualizaDisplay();
    EstadoBotoes();
    if ((BTback != BTBackAnt) && (digitalRead(BTback) == LOW)) {
      tipo = tipo - 1;
      if (tipo == 255)
      {
        tipo = 3;
      }
      AtualizaDisplay();
      delay (200);
      EstadoBotoes();
    }
    if (BTfwrd != BTFwrdAnt && digitalRead(BTfwrd) == LOW) {
      tipo = tipo + 1;
      if (tipo > 3)
      {
        tipo = 1;
      }
      AtualizaDisplay();
      delay(200);
      EstadoBotoes();
    }
    AtualizaDisplay();
    if ((digitalRead(BTenter) != BTEnterAnt) && (digitalRead(BTenter) == LOW))
    {
      tela = 0;
      delay(200);
      EstadoBotoes();
      EEPROM.update(11,tipo);
      ModAbertura(tipo,true);
    }
    EstadoBotoes();
  }
  lcd.noBlink();
  lcd.clear();
}
