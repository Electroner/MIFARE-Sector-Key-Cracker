#include <SPI.h>
#include <MFRC522.h>

// Pines SPI
#define RST_PIN 9
#define SS_PIN 10

// Instancia a la clase MFRC522
MFRC522 mfrc522(SS_PIN, RST_PIN);

const bool Testing_A = true;
const bool Testing_B = true;

unsigned char keyA1 = 0x00;
unsigned char keyA2 = 0x00;
unsigned char keyA3 = 0x00;
unsigned char keyA4 = 0x00;
unsigned char keyA5 = 0x00;
unsigned char keyA6 = 0x00;

unsigned char keyB1 = 0x00;
unsigned char keyB2 = 0x00;
unsigned char keyB3 = 0x00;
unsigned char keyB4 = 0x00;
unsigned char keyB5 = 0x00;
unsigned char keyB6 = 0x00;

//claves de cifrado
MFRC522::MIFARE_Key keyA = {keyByte : {keyA1, keyA2, keyA3, keyA4, keyA5, keyA6}};
MFRC522::MIFARE_Key keyB = {keyByte : {keyB1, keyB2, keyB3, keyB4, keyB5, keyB6}};

boolean resA;
boolean resB;

// Datos del sector
byte sector = 15;

const int delayPrint = 50;

void mostrarByteArray(byte *buffer, byte bufferSize)
{
	for (byte i = 0; i < bufferSize; i++)
	{
		Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		Serial.print(buffer[i], HEX);
	}
}

float tiempo;

void setup()
{
	Serial.begin(9600);
	while (!Serial)
		;
	SPI.begin();
	mfrc522.PCD_Init();

	Serial.println(F("Acerca la tarjeta al lector para escanear."));
	Serial.println(F("Comprobando las claves:"));
	Serial.print("Key-A: ");
	mostrarByteArray(keyA.keyByte, MFRC522::MF_KEY_SIZE);
	Serial.println();
	Serial.print("Key-B: ");
	mostrarByteArray(keyB.keyByte, MFRC522::MF_KEY_SIZE);
	Serial.println();
}

bool statusa = true;

void loop()
{
	if (!mfrc522.PICC_IsNewCardPresent())
	{
		return;
	}
	if (!mfrc522.PICC_ReadCardSerial())
	{
		return;
	}
	if (statusa)
	{
		Serial.println("Cracking ...");
		statusa = false;
	}

	CheckKeys(&keyA, &keyB, &resA, &resB, sector);
	if (Testing_A)
	{
		if (resA)
		{
			Serial.println("--------------------- ACIERTO A ----------------------");
			mostrarByteArray(keyA.keyByte, MFRC522::MF_KEY_SIZE);
			Serial.println("\n");
			Serial.println("------------------------------------------------------");
		}
		else
		{
			updateKeyA();
			if ((int)(millis() / 10) % delayPrint == 0)
			{
				Serial.println("FALLO PARA CLAVE A:");
				mostrarByteArray(keyA.keyByte, MFRC522::MF_KEY_SIZE);
				Serial.println("\n");
			}
		}
	}

	if (Testing_B)
	{
		if (resB)
		{
			Serial.println("--------------------- ACIERTO B ----------------------");
			mostrarByteArray(keyB.keyByte, MFRC522::MF_KEY_SIZE);
			Serial.println("\n");
			Serial.println("------------------------------------------------------");
		}
		else
		{
			updateKeyB();
			if ((int)(millis() / 1000) % delayPrint == 0)
			{
				Serial.println("FALLO PARA CLAVE B:");
				mostrarByteArray(keyB.keyByte, MFRC522::MF_KEY_SIZE);
				Serial.println("\n");
			}
		}
	}
	if (Testing_A)
	{
		if (resA)
		{
			Serial.println("\nTiempo Transcurrido: ");
			Serial.println(millis() - tiempo);
		}
	}
	if (Testing_B)
	{
		if (resB)
		{
			Serial.println("\nTiempo Transcurrido: ");
			Serial.println(millis() - tiempo);
		}
	}
	mfrc522.PICC_HaltA();
	mfrc522.PCD_StopCrypto1();
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
		Serial.println("\nERROR_MAX_OVERFLOW_A");
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
		Serial.println("\nERROR_MAX_OVERFLOW_B");
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