#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

 
#define SS_PIN 10
#define RST_PIN 9
#define relay_out 4


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x3F, 16, 2);


String card_id_1 = "494eb499";
String card_id_2 = "b977acb2";
String card_id_3 = "49cdb3b2";
String card_id_4 = "b935afb2";
int user = 0;
char data;


void setup() 
{
  lcd.begin();
  pinMode(relay_out,OUTPUT);
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  for(int k=0;k<5;k++)
  {
    EEPROM.write(k,0);
  }
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  

}


void loop() 
{
   lcd.setCursor(0,0);
   int value = 0;
   get_card();
   if (user > 0)
   {
      lcd.clear();
      lcd.print("User :");
      lcd.print(user);
      lcd.setCursor(0,1);
      Serial.println("Press R to recharge card \nPress W to activate wireless charging\nPress X to exit.");
      transaction(user);
   }
   user = 0;
   delay(500);
}


void get_card()
{
  lcd.print("WELCOME");
  if(!mfrc522.PICC_IsNewCardPresent()) 
  {
   return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) 
  {
    return ;
  }
   String content= "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) 
   {
     //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     //Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
   }
   content.replace(" ","");
   Serial.print("UID tag :");
   Serial.println(content); 
   comp(content);
}


void comp(String x)
{
  if(x == card_id_1)
  {
    Serial.println("User 1");
    user = 1;
    return;
  }
  else if(x == card_id_2)
  {
    Serial.println("User 2");
    user = 2;
    return;
  }
  else if(x == card_id_3)
  {
    Serial.println("User 3");
    user = 3;
    return;
  }
   else if(x == card_id_4)
  {
    Serial.println("User 4");
    user = 4;
    return;
  }
  else
  {
    Serial.println("User not found.");
    user = 0;
    return;
  }
}


void transaction(int user_id)
{
label0:if(Serial.available())
  {
    data = Serial.read();
    Serial.print("Data entered is : ");
    Serial.println(data);
    if(data == 'X')
    {
      Serial.println("Exited.");
    }
    else if(data == 'R')
    {
      Serial.println("The maximum balance is 255.\nEnter recharge amount:");
      label1:if(Serial.available())
      {
        int temp = 0;
        int amount = Serial.parseInt();
        int value = EEPROM.read(user_id);
        if(value + amount > 255)
        {
          temp = 255;
          Serial.println("Capped at max loadable amount.");
        }
        else
        {
          temp = value+amount;
        }
        EEPROM.write(user_id,temp);
        Serial.print("The balance amount is :");
        int balance = EEPROM.read(user_id);
        Serial.println(balance);
        lcd.print("Balance : ");
        lcd.print(balance);
        delay(3000);
        lcd.clear();
       }
       else
       {
        goto label1;
       }
     }
     else if(data == 'W')
     {
        Serial.println("Entered the time to be charged:");
        label2:if(Serial.available())
        {
          int charging_time = Serial.parseInt();
          int value = EEPROM.read(user_id);
          if(value - charging_time < 0)
          {
            Serial.println("Insufficient amount , Kindly recharge."); 
          }
          else
          {
            Serial.println("Successfull transaction.");
            int temp = value - charging_time;
            Serial.print("Available balance : ");
            Serial.println(temp);
            lcd.print("Balance : ");
            lcd.print(temp);
            delay(3000);
            lcd.clear();
            EEPROM.write(user_id,temp);
            Serial.println("Opening ...");
            digitalWrite(relay_out,HIGH);
            delay(2000);
            Serial.println("Closing..");
            digitalWrite(relay_out,LOW);
            delay(2000);
          }
        }
        else
        {
          goto label2;    
        }
      }
      else
      {
        Serial.println("Invalid Input");
      }
   }
   else
   {
    goto label0;
   }
   admin();
}

void admin()
{
  for(int j=1;j<5;j++)
  {
    Serial.print("User : ");
    Serial.print(j);
    Serial.print(" , Balance :");
    int finl = EEPROM.read(j);
    Serial.println(finl);
  }
}

