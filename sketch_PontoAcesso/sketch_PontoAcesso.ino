//Bibliotecas
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

//Pinos
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal lcd(6, 7, 5, 4, 3, 2);
const int buzzer_led = 8;

//Variáveis
char st[20];
MFRC522::MIFARE_Key key;

String reportString, codigo;

void setup() {
  //Seta led e buzzer
  pinMode(buzzer_led,OUTPUT);
  digitalWrite (8, LOW);
  
  // Inicia a serial
  Serial.begin(9600);   

  // Inicia  SPI bus
  SPI.begin();      

  // Inicia MFRC522
  mfrc522.PCD_Init();
  
  
  //Define o número de colunas e linhas do LCD:
  lcd.begin(16, 2);

  //Prepara chave - padrao de fabrica = FFFFFFFFFFFFh
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
}

void loop() {

  leitura();
  delay(1000);  
  
  reportString = Serial.readString();
  codigo = reportString.charAt(1);

  
}

void leitura() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  String conteudo = "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  //Printa na serial o número do cartão
  Serial.println(conteudo);
  
  delay(1000);
  
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(Serial.readString());

  delay(1000);
  
  lcd.clear();
  
  digitalWrite(buzzer_led, HIGH);
  delay(500);
   
  //Desligando o buzzer e led
  digitalWrite (buzzer_led, LOW);
}

void escrita() {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Modo gravacao");
    lcd.setCursor(3, 1);
    lcd.print("selecionado");
    delay(3000);
    modo_gravacao();
}

void modo_gravacao() {
  //Aguarda cartao
  while ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(100);
  }
  if ( ! mfrc522.PICC_ReadCardSerial())    return;

  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  
  byte buffer[34];
  byte block;
  MFRC522::StatusCode status;
  byte len;

  
  //Grava no bloco 1
  block = 1;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    return;
  }
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    return;
  }

  //Grava no bloco 2
  block = 2;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    return;
  }
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    return;
  }

  //Grava no bloco 4
  block = 4;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    return;
  }
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    return;
  }

  //Grava no bloco 5
  block = 5;
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    return;
  }
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    //return;
  } else {
    lcd.clear();
    lcd.print("Gravacao OK!");
  }

  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  delay(1000);
}
