#include <Arduino.h>
#include <stdio.h>
#include <Tools.h>
#include <BCH.h>
#include <SRAM_CY62256N.h>
#include <SPI.h>
#include <SD.h>
#include <Crypto.h>
#include <SHA3.h>
#include <string.h>
#include <AES.h>
#include <CTR.h>

#define PIN_POWER_ANALOG A8
#define PIN_POWER 9

SRAM_CY62256N sram;

uint8_t helper_data_new[7 * 37];
uint8_t puf_binary_new[8 * 37];

boolean readBit(long location) {
  uint8_t result = sram.read(floor(location / 8));
  return result >> (7 - (location % 8)) & 0x1 == 1;
}

void set() {
  sram = SRAM_CY62256N();

  DDRB = 0b11100000; //Pin 13, 12, 11 set to output
  DDRA = 0b11111111; //Pins 22-29 Set as Output (Lower Byte of Address)
  DDRC = 0b01111111; //Pins 37-31 Set as Output (Upper Byte of Address)

  pinMode(PIN_POWER, OUTPUT);

  Serial.begin(115200);

  delay(1000);
}

void decode(uint8_t *key_32) {
  BCH bch;
  Tools tools;
  bch.initialize();

  uint8_t helper_data_padded_new[8 * 37];
  uint8_t xor_reproduction_new[8 * 37];
  uint8_t reconstructed_key_new[37];

  memset(helper_data_padded_new, 0, sizeof(helper_data_padded_new));
  memset(xor_reproduction_new, 0, sizeof(xor_reproduction_new));
  memset(reconstructed_key_new, 0, sizeof(reconstructed_key_new));

  int row = 37;
  int n = bch.get_n();
  int k = bch.get_key_length();

  int k_length_bit = row;//tools.ceil(k*row,8);
  int n_length_bit = row * 8;//tools.ceil(n*row,8);

  long thisItem[7];

  /************************************
   ****** REPRODUCTION PROCEDURE ******
   ************************************/
  /******************** ASSERT HELPER DATA PADDED**********************/
  for (int i = 0; i < row; i++) {
    memcpy(&helper_data_padded_new[i * 8], &helper_data_new[i * 7], 7 * sizeof(uint8_t));
  }

  /******************** ASSERT XOR REPRODUCTION **********************/
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < 8; j++) {
      xor_reproduction_new[i * 8 + j] = puf_binary_new[i * 8 + j] ^ helper_data_padded_new[i * 8 + j];
    }
  }

  /******************** ASSERT RECONSTRUCTED KEY **********************/
  for (int i = 0; i < row; i++) {
    bch.decode_bch(&xor_reproduction_new[i * 8], &reconstructed_key_new[i]);
  }

  tools.convert_key_back(reconstructed_key_new, key_32);
}


void turn_on_sram() {
  analogWrite(PIN_POWER, 255);
}

void turn_off_sram() {
  analogWrite(PIN_POWER, 0);

  for (int i = 22; i <= 29; i++) {
    digitalWrite(i, LOW);
  }
  for (int i = 31; i <= 37; i++) {
    digitalWrite(i, LOW);
  }
  for (int i = 42; i <= 49; i++) {
    digitalWrite(i, LOW);
  }
  for (int i = 11; i <= 13; i++) {
    digitalWrite(i, LOW);
  }
}

void initializeSD() {
  Serial.print("Initializing SD card...");

  if (!SD.begin(7)) {
    Serial.println("initialization failed!");
    delay(2000);
    exit(0);
  }
  Serial.println("initialization done.");
}

boolean isValidNumber(String str) {
  if (str.length() == 0)
    return false;
  for (byte i = 0; i < str.length(); i++)
  {
    if (!isDigit(str.charAt(i)))
      return false;
  }
  return true;
}

String convertString(String a) {
  String s;
  boolean isNum;
  for (byte i = 0; i < a.length(); i++)
  {
    isNum = isDigit(a.charAt(i));
    if (isNum)
      s += a.charAt(i);
  }
  return s;
}

void readSRAMfromMicroSD() {
  memset(puf_binary_new, 0, sizeof(puf_binary_new));
  uint8_t result;

  // read
  int index = 0, i = 0;

  String name = "c.txt";
  String a, b;
  long thisItem;
  File myFile = SD.open(name);
  if (myFile) {
    while (myFile.available()) {
      a = myFile.readStringUntil('\n');
      b = convertString(a);
      if (isValidNumber(b)) {
        thisItem = b.toInt();
        if (readBit(thisItem))
          puf_binary_new[index] = puf_binary_new[index] | 0x1;
        if ((i + 1) % 8 == 0) {
          index++;
        } else {
          puf_binary_new[index] = puf_binary_new[index] << 1;
        }
        i++;
      }
    }
    myFile.close();
  } else {
    Serial.println("error ");
  }
}

void readHelperDatafromMicroSD() {
  memset(helper_data_new, 0, sizeof(helper_data_new));
  uint8_t result;

  // read
  int index = 0, i = 0;

  String name = "h.txt";
  String a, b;
  long thisItem;
  File myFile = SD.open(name);
  if (myFile) {
    while (myFile.available()) {
      a = myFile.readStringUntil('\n');
      b = convertString(a);
      if (isValidNumber(b)) {
        thisItem = b.toInt();
        helper_data_new[i] = thisItem;
        i++;
      }
    }
    myFile.close();
  } else {
    Serial.println("error ");
  }
}

void writeToSD(String file_name, uint8_t* things_to_write, int sz) {
  File myFile = SD.open(file_name,  O_WRITE | O_CREAT | O_TRUNC);
  if (myFile) {
    myFile.write(things_to_write, sz);
    myFile.close();
  } else {
    Serial.println("error writing");
  }
}

void readFromSD(String file_name, uint8_t* result, int sz) {
  File myFile = SD.open(file_name);
  byte a;

  if (myFile) {
    myFile.read(result, sz);
    myFile.close();
  } else {
    Serial.println("error opening");
  }
}

void print_key(uint8_t* key, uint8_t length) {
  for (int i = 0; i < length; i++) {
    Serial.print("0123456789abcdef"[key[i] >> 4]);
    Serial.print("0123456789abcdef"[key[i] & 0xf]);
  }
  Serial.println();
}

void derive_new_key(String user_password, uint8_t* final_key, uint8_t* key_32) {
  SHA3_256 sha3_256;

  sha3_256.resetHMAC(key_32, 32);
  sha3_256.update(reinterpret_cast<const uint8_t*>(&user_password[0]), user_password.length());
  sha3_256.finalizeHMAC(key_32, 32, final_key, 32);
}

void check_integrity(uint8_t* encrypted, uint8_t* result, uint8_t* key_32) {
  SHA3_256 sha3_256;

  sha3_256.resetHMAC(key_32, 32);
  sha3_256.update(encrypted, sizeof(encrypted));
  sha3_256.finalizeHMAC(key_32, 32, result, 32);
}

void decrypt_test(uint8_t* final_key, uint8_t* iv, uint8_t *cypher, uint8_t *decrypted){
  CTR<AES256> ctraes256;
  ctraes256.clear();
  if (!ctraes256.setKey(final_key, 32)) {
      Serial.print("setKey failed\n");
      return;
  }
  if (!ctraes256.setIV(iv, 16)) {
      Serial.print("setIV failed\n");
      return;
  }

  ctraes256.decrypt(decrypted, cypher, 32);
}

void setup(void)
{
  delay(1000);
  String user_password = "";
  uint8_t final_key[32];
  uint8_t key_32[32];
  memset(key_32, 0, sizeof(key_32));
  memset(final_key, 0, sizeof(final_key));

  set();
  initializeSD();

  turn_on_sram();
  readSRAMfromMicroSD();
  turn_off_sram();
  readHelperDatafromMicroSD();

  /**
     START DECODING - USE THE ERROR CORRECTION - TO CREATE THE PUF KEY
  */
  decode(key_32);
  Serial.println();
  Serial.print("PUF key \t: ");
  print_key(key_32, 32);

  /**
     KEY DERIVATION STAGE
  */
  Serial.println();
  Serial.println("Please enter password to generate the final key");
  while (Serial.available() == 0)
  {
  }
  user_password = Serial.readString();
  Serial.print("User password \t: ");
  Serial.println(user_password);

  derive_new_key(user_password, final_key, key_32);

  Serial.print("Final key \t: ");
  print_key(final_key, 32);

  /**
     DECRYPT USING THE FINAL KEY
  */
  Serial.println();
  uint8_t e[32];
  readFromSD("e.txt", e, 32);
  Serial.print("cyphertext \t: ");
  print_key(e, 32);

  uint8_t h_sd[32];
  readFromSD("h.txt", h_sd, 32);

  uint8_t h_result[32];
  memset(h_result, 0, sizeof(h_result));
  check_integrity(e, h_result, key_32);

  uint8_t iv[16];
  readFromSD("i.txt", iv, 16);
  // Serial.print("IV \t\t: ");
  // print_key(iv, 16);

  uint8_t decrypted[32];
  memset(decrypted, 0, sizeof(decrypted));
  decrypt_test(final_key, iv, e, decrypted);

  if (memcmp(h_sd, h_result, 32) > 0){
      Serial.println("CIPHERTEXT INVALID");
      return;
  }

  Serial.print("decrypted \t: ");
  for (int i=0;i<32;i++)
    Serial.print((char) decrypted[i]);
  Serial.println();
}

void loop() {
  delay(1000);
  exit(0);
}
