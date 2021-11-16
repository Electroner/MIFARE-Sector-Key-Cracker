/*
PINOUT:
RC522 MODULE        Uno/Nano     MEGA
SDA                 D10          D9
SCK                 D13          D52
MOSI                D11          D51
MISO                D12          D50
IRQ                 N/A          N/A
GND                 GND          GND
RST                 D9           D8
3.3V                3.3V         3.3V
*/

#include <SPI.h>
#include <MFRC522.h>
#include <SD.h>

// Pines SPI
#define SS_RF 10
#define RST_PIN 9
#define SS_SD 8

// Instancia a la clase MFRC522
MFRC522 mfrc522(SS_RF, RST_PIN);

const bool Testing_A = true;  //Only testing key A
const bool Testing_B = false; //Only testing key B

//Posicion iniciales de la llave A para empezar a buscar
unsigned char keyA1 = 0xA0;
unsigned char keyA2 = 0xA1;
unsigned char keyA3 = 0xA2;
unsigned char keyA4 = 0xA3;
unsigned char keyA5 = 0xA4;
unsigned char keyA6 = 0x55;

//Posicion iniciales de la llave B para empezar a buscar
unsigned char keyB1 = 0xB0;
unsigned char keyB2 = 0xB1;
unsigned char keyB3 = 0xB2;
unsigned char keyB4 = 0xB3;
unsigned char keyB5 = 0xB4;
unsigned char keyB6 = 0xB5;

//claves de cifrado
MFRC522::MIFARE_Key keyA = {keyByte : {keyA1, keyA2, keyA3, keyA4, keyA5, keyA6}};
MFRC522::MIFARE_Key keyB = {keyByte : {keyB1, keyB2, keyB3, keyB4, keyB5, keyB6}};

boolean resA; //resultado de la autenticacion
boolean resB; //resultado de la autenticacion

// Datos del sector
byte sector = 15;

const int delayPrint = 50; //delay para imprimir

File Registro;

float tiempo;

String mostrarByteArray(byte *buffer, byte bufferSize)
{
	String cad;
	for (byte i = 0; i < bufferSize; i++)
	{
		cad = cad + (buffer[i] < 0x10 ? " 0" : " ");
		cad = cad + String(buffer[i], HEX);
	}
	return cad;
}

void EscribirArchSer(String cad)
{
	digitalWrite(SS_SD, LOW);
	digitalWrite(SS_RF, HIGH);
	
	Registro = SD.open("status.txt", FILE_WRITE);
	if (Registro)
	{
		Registro.println(cad);
	}
	Registro.close();

	digitalWrite(SS_SD, HIGH);
	digitalWrite(SS_RF, LOW);
}

void setup()
{
	SPI.begin();

	digitalWrite(SS_SD, HIGH);
	digitalWrite(SS_RF, LOW);
	mfrc522.PCD_Init();
	mfrc522.PCD_DumpVersionToSerial();
	digitalWrite(SS_RF, HIGH);
	digitalWrite(SS_SD, LOW);
	if (!SD.begin(SS_SD))
	{
		return;
	}
	digitalWrite(SS_SD, HIGH);
	digitalWrite(SS_RF, LOW);

	EscribirArchSer("#############################################");
	EscribirArchSer("################ RESET PLACA ################");
	EscribirArchSer("#############################################");

	EscribirArchSer(F("Acerca la tarjeta al lector para escanear."));
	EscribirArchSer(F("Comprobando las claves:"));
	EscribirArchSer("Key-A: ");
	EscribirArchSer(mostrarByteArray(keyA.keyByte, MFRC522::MF_KEY_SIZE));
	EscribirArchSer("Key-B: ");
	EscribirArchSer(mostrarByteArray(keyB.keyByte, MFRC522::MF_KEY_SIZE));
}

bool statusa = true;

void loop()
{
	if (mfrc522.PICC_IsNewCardPresent())
	{
		if (mfrc522.PICC_ReadCardSerial())
		{
			if (statusa)
			{
				EscribirArchSer("Cracking ...");
				statusa = false;
			}

			CheckKeys(&keyA, &keyB, &resA, &resB, sector);
			if (Testing_A)
			{
				if (resA)
				{
					EscribirArchSer("--------------------- ACIERTO A ----------------------");
					EscribirArchSer(mostrarByteArray(keyA.keyByte, MFRC522::MF_KEY_SIZE));
					EscribirArchSer("------------------------------------------------------");
				}
				else
				{
					updateKeyA();
					if ((int)(millis() / 10) % delayPrint == 0)
					{
						EscribirArchSer("FALLO PARA CLAVE A:");
						EscribirArchSer(mostrarByteArray(keyA.keyByte, MFRC522::MF_KEY_SIZE));
					}
				}
			}

			if (Testing_B)
			{
				if (resB)
				{
					EscribirArchSer("--------------------- ACIERTO B ----------------------");
					EscribirArchSer(mostrarByteArray(keyB.keyByte, MFRC522::MF_KEY_SIZE));
					EscribirArchSer("------------------------------------------------------");
				}
				else
				{
					updateKeyB();
					if ((int)(millis() / 1000) % delayPrint == 0)
					{
						EscribirArchSer("FALLO PARA CLAVE B:");
						EscribirArchSer(mostrarByteArray(keyB.keyByte, MFRC522::MF_KEY_SIZE));
					}
				}
			}
			if (Testing_A)
			{
				if (resA)
				{
					EscribirArchSer("\nTiempo Transcurrido: ");
					EscribirArchSer(String(millis() - tiempo));
				}
			}
			if (Testing_B)
			{
				if (resB)
				{
					EscribirArchSer("\nTiempo Transcurrido: ");
					EscribirArchSer(String(millis() - tiempo));
				}
			}
		}
	}
}

void CheckKeys(MFRC522::MIFARE_Key *KeyA, MFRC522::MIFARE_Key *KeyB, boolean *resAF, boolean *resBF, int sector)
{
	MFRC522::StatusCode estadoA;
	MFRC522::StatusCode estadoB;
	byte bloqueTrailer = sector * 4 + 3; // Cálculo del bloque Trailer
	byte buffer[18];
	byte size = sizeof(buffer);

	if (Testing_A)
	{
		// Autenticar utilizando la clave Key-A
		estadoA = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, bloqueTrailer, KeyA, &(mfrc522.uid));

		// Si no consigue autenticar A que no continúe
		if (estadoA != MFRC522::STATUS_OK)
		{
			resA = false;
		}
		else
		{
			resA = true;
		}
	}

	if (Testing_B)
	{
		// Autenticar utilizando la clave Key-B
		estadoB = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, bloqueTrailer, KeyB, &(mfrc522.uid));

		// Si no consigue autenticar B que no continúe
		if (estadoB != MFRC522::STATUS_OK)
		{
			resB = false;
		}
		else
		{
			resB = true;
		}
	}
}

void updateKeyA()
{
	if (keyA1 == 255)
	{
		EscribirArchSer("\nERROR_MAX_OVERFLOW_A");
	}
	if (keyA2 == 255)
	{
		keyA2 = 0;
		keyA1++;
	}
	if (keyA3 == 255)
	{
		keyA3 = 0;
		keyA2++;
	}
	if (keyA4 == 255)
	{
		keyA4 = 0;
		keyA3++;
	}
	if (keyA5 == 255)
	{
		keyA5 = 0;
		keyA4++;
	}
	if (keyA6 == 255)
	{
		keyA6 = 0;
		keyA5++;
	}

	keyA6 = keyA6 + 1;
	keyA = {keyByte : {keyA1, keyA2, keyA3, keyA4, keyA5, keyA6}};
}

void updateKeyB()
{
	if (keyA1 == 255)
	{
		EscribirArchSer("\nERROR_MAX_OVERFLOW_B");
	}
	if (keyB2 == 255)
	{
		keyB2 = 0;
		keyB1++;
	}
	if (keyB3 == 255)
	{
		keyB3 = 0;
		keyB2++;
	}
	if (keyB4 == 255)
	{
		keyB4 = 0;
		keyB3++;
	}
	if (keyB5 == 255)
	{
		keyB5 = 0;
		keyB4++;
	}
	if (keyB6 == 255)
	{
		keyB6 = 0;
		keyB5++;
	}

	keyB6 = keyB6 + 1;
	keyB = {keyByte : {keyB1, keyB2, keyB3, keyB4, keyB5, keyB6}};
}
