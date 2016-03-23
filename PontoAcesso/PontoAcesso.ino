#include <AddicoreRFID.h>
#include <SPI.h>
#define uchar unsigned char
#define uint unsigned int
//4 bytes tag serial number, the first 5 bytes for the checksum byte
uchar serNumA[5];
uchar fifobytes;
uchar fifoValue;

String codConcat; //variável responsável por guardar o cod lido

AddicoreRFID myRFID;  //Cria o Objeto

//Seta os pinos

const int chipSelectPin = 10;
const int NRSTPD = 5;

#define MAX_LEN 16 //Tamanho max do array

void setup() {
  Serial.begin(9600);

  //Iniciando a biblioteca SPI

  SPI.begin();
  pinMode(chipSelectPin, OUTPUT);
  digitalWrite(chipSelectPin, LOW);
  pinMode(NRSTPD, OUTPUT);
  digitalWrite(NRSTPD, HIGH);
  myRFID.AddicoreRFID_Init();
}

void loop() {

  if (Serial.available() > 0) {
    switch(Serial.read()){
      case '1':
        Serial.println("Passe o cartão");
        leitura();
        break;
    }
    
  }

}

//Métodos

//Método de leitura por RFID
void leitura() {
  //Inicia a leitura por rfid
  Serial.println("Aproxime o Cartão ou Tag");
  uchar i, tmp, checksum1;
  uchar status;
  uchar str[MAX_LEN];
  uchar RC_size;
  uchar blockAddr;

  //Selection operation block address 0 to 63
  String mynum = "";
  str[1] = 0x4400;

  //Find tags, return tag type
  status = myRFID.AddicoreRFID_Request(PICC_REQIDL, str);
  if (status == MI_OK) {
    Serial.println("RFID tag detected");
    Serial.print(str[0], BIN);
    //Serial.print(" , ");
    Serial.print(str[1], BIN);
    //Serial.println(" ");
  }

  //Anti-collision, return tag serial number 4 bytes
  status = myRFID.AddicoreRFID_Anticoll(str);
  if (status == MI_OK) {
    checksum1 = str[0] ^ str[1] ^ str[2] ^ str[3];
    Serial.println("O código do cartão ou tag é: ");

    Serial.print(str[0]);
    //Serial.print(" , ");
    Serial.print(str[1], BIN);
    //Serial.print(" , ");
    Serial.print(str[2], BIN);
    //Serial.print(" , ");
    Serial.print(str[3], BIN);
    //Serial.print(" , ");
    Serial.print(str[4], BIN);
    //Serial.print(" , ");
    Serial.println(checksum1, BIN);

    Serial.println();
    delay(1000);

    // codConcat = "" + str[0] + "" + str[1] + "" + str[2] + "" + str[3] + "" + str[4];
  }
  myRFID.AddicoreRFID_Halt(); //Command tag into hibernation
}

//Método de escrita por RFID
void escrita() {
  delay(2000);

  int token = Serial.read();
}

//Método de leitura e verificação
//void leitura_verifica() {
//  delay(2000);
//  int cod = Serial.read();
// leitura();
//delay(1000);
// if (cod == codConcat) {
//   Serial.println("Codigo verificado com sucesso!");
/// } else {
//   Serial.println("Codigos não conferem!");
//  }
//  Serial.print();
//}




