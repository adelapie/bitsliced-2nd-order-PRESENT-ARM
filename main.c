
#include "main.h"

//Local functions
void get_bytes(uint32_t nbytes, uint8_t* ba);
void send_bytes(uint32_t nbytes, uint8_t* ba);
void get_char(uint8_t *ch);
void send_char(uint8_t ch);
void readFromCharArray(uint8_t *ch);
void readByteFromInputBuffer(uint8_t *ch);
void init();
void CrashGracefully();
void usart_init();
void oled_init();
void ecc_init();

unsigned input[4];

//Variables, constants and structures
const uint8_t defaultKeyDES[8] = { 0xca, 0xfe, 0xba, 0xbe, 0xde, 0xad, 0xbe, 0xef };
const uint8_t defaultKeyTDES[24] = { 0xca, 0xfe, 0xba, 0xbe, 0xde, 0xad, 0xbe, 0xef, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 ,0xca, 0xfe, 0xba, 0xbe, 0xde, 0xad, 0xbe, 0xef }; //Default is TDES in 2key mode, DESkey1==DESkey3
const uint8_t defaultKeyAES[16] = { 0xca, 0xfe, 0xba, 0xbe, 0xde, 0xad, 0xbe, 0xef, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
const uint8_t defaultKeyAES256[32] = { 0xca, 0xfe, 0xba, 0xbe, 0xde, 0xad, 0xbe, 0xef, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
									   0xb0, 0x00, 0xc0, 0x00, 0x01, 0xb0, 0x00, 0xc0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
const uint8_t glitched[] = { 0xFA, 0xCC };
const int bootLoopCount=168000;
const uint8_t cmdByteIsWrong[] = { 'B','a','d','C','m','d','\n',0x00};
const uint8_t OLED_bootNormalScreen[] =   { 'B','o','o','t',' ','c','h','e','c','k',' ','o','k',' ',' ',' ',' ',' ',' ',' ',' '};
const uint8_t OLED_bootGlitchedScreen[] = { 'D','E','B','U','G',' ','M','O','D','E',' ','E','N','A','B','L','E','D',' ',' ',' '};
const uint8_t OLED_initScreen[] = { 'T','r','a','i','n','i','n','g',' ','B','o','a','r','d',' ','1','.','0',' ',' ',' ','(','c',')','R','i','s','c','u','r','e',' ','2','0','1','4'};
const uint8_t zeros[16]={'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
volatile uint8_t rxBuffer[RXBUFFERLENGTH] = { };
volatile uint8_t pt_bitslice[256] = { };


volatile uint32_t ticker, downTicker;
volatile uint8_t usbSerialEnabled=0;

//userworkshop
EC_PARAMETER	Base;
EC_KEYPAIR		Signer;
SIGNATURE		signature;
BIGINT			prime_order;
POINT			temp;
INDEX 			i, error;
uint8_t			Message[1024];
uint8_t string1[30] = "5192296858534827627896703833467507";

//
//rand functions

//Get a random number
//while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
//{}
//currSbox=(uint8_t) (RNG_GetRandomNumber() & 0x07);

uint32_t val_ran_array[3];
int counter;

void updateRNG() {

	//for (counter=0; counter<3;counter++){
	//	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	//	{}
	//	val_ran_array[counter]=RNG_GetRandomNumber();
	//}

	unsigned int t[3];

	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[0]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_1, t, 4);


	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[1]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_2, &t[1], 4);
	//memcpy(&val_ran_array[1], &t[1], 4);

	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[2]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_3, &t[2], 4);
	//memcpy(&val_ran_array[2], &t[2], 4);

}

void updateRNG_all() {

	//for (counter=0; counter<3;counter++){
	//	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	//	{}
	//	val_ran_array[counter]=RNG_GetRandomNumber();
	//}

	unsigned int t[3];

	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[0]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_1, t, 4);


	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[1]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_2, &t[1], 4);
	//memcpy(&val_ran_array[1], &t[1], 4);

	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[2]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_3, &t[2], 4);
	//memcpy(&val_ran_array[2], &t[2], 4);

	//new

	//for (counter=0; counter<3;counter++){
	//	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	//	{}
	//	val_ran_array[counter]=RNG_GetRandomNumber();
	//}


	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[0]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_4, t, 4);


	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[1]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_5, &t[1], 4);
	//memcpy(&val_ran_array[1], &t[1], 4);

	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[2]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_6, &t[2], 4);
	//memcpy(&val_ran_array[2], &t[2], 4);

	//for (counter=0; counter<3;counter++){
	//	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	//	{}
	//	val_ran_array[counter]=RNG_GetRandomNumber();
	//}


	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[0]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_7, t, 4);


	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[1]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_8, &t[1], 4);
	//memcpy(&val_ran_array[1], &t[1], 4);

	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[2]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_9, &t[2], 4);
	//memcpy(&val_ran_array[2], &t[2], 4);

	//for (counter=0; counter<3;counter++){
	//	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	//	{}
	//	val_ran_array[counter]=RNG_GetRandomNumber();
	//}


	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[0]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_10, t, 4);


	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[1]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_11, &t[1], 4);
	//memcpy(&val_ran_array[1], &t[1], 4);

	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}


	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);


	t[2]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_12, &t[2], 4);
	//memcpy(&val_ran_array[2], &t[2], 4);

	//for (counter=0; counter<3;counter++){
	//	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	//	{}
	//	val_ran_array[counter]=RNG_GetRandomNumber();
	//}

}


void updateRNG_only_once() {

	unsigned int t[1];

	while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
	{}





	t[0]=RNG_GetRandomNumber();
	memcpy(ADDR_RNG_NUMBER_1, t, 4);
	//memcpy(val_ran_array, t, 4);

}

void updateRNG_fake() {

	unsigned int t[1];

	//unsigned *p = ADDR_RNG_NUMBER_1;

	t[0]=5;
	//val_ran_array[0]=5;
	//memcpy(val_ran_array,);
	memcpy(ADDR_RNG_NUMBER_1, t, 4);



}

//

////////////////////////////////////////////////////
//MAIN FUNCTION: entry point for the board program//
////////////////////////////////////////////////////

uint32_t *rng_addr = NULL;
uint32_t *rng_addr_1 = NULL;
uint32_t *rng_addr_2 = NULL;

int main(void) {
	uint8_t cmd, tmp;
	volatile int payload_len, i,j, glitchedBoot, counter=0;
	ErrorStatus cryptoCompletedOK=ERROR;
	uint32_t keyScheduleAES[MAXAESROUNDS] = { };
	volatile uint8_t keyDES[8];
	volatile uint8_t keyTDES[24];
	volatile uint8_t keyAES[16];
	volatile uint8_t keyAES256[32];
	aes256_context ctx;

	unsigned char a[4];


	//

	unsigned bitsliced[64];

	//unsigned key[] = {4294967295, 4294967295, 65535}; // 0xffff = {65535, 0, 0}
	unsigned key[] = {65535, 65535, 65535};
	//unsigned key[96];

	//unsigned key[96];
	uint32_t val_ran_array[3];



	//for (j = 0; j<96; j++) key[j] = 0;
	key[95] = 1; // AQUI TEST!

	/* Set up the system clocks */
	SystemInit();
	/* Initialize peripherals and select IO interface */
	init();
	RNG_Config();

	// If no jumper between PA9, VBUS
	if(!usbSerialEnabled){
	// Enable USART3 in port gpioC (Pins PC10 TxD,PC11 RxD)
	usart_init();
	}
	// Optional peripherals: enable SPI and GPIO pins for OLED display
	oled_init();
	oled_clear();

	// Initialize & load default cryptographic keys from FLASH memory
	// Load CRT parameters
	rsa_crt_init();
	// Load default keys for DES, TDES, AES from non volatile memory
	for (i = 0; i < 8; i++) keyDES[i] = defaultKeyDES[i];
	for (i = 0; i < 24; i++) keyTDES[i] = defaultKeyTDES[i];
	for (i = 0; i < 16; i++) keyAES[i] = defaultKeyAES[i];
	for (i = 0; i < 32; i++) keyAES256[i] = defaultKeyAES256[i];

	aes256_init(&ctx,keyAES256); //Prepare AES key schedule

	//Loop for trivial Boot glitching; display boot screen with glitched status
	//PC1 can be used as trigger pin for boot glitching
	GPIOC->BSRRL = GPIO_Pin_1; //PC1 3.3V
	glitchedBoot=0;
	for (counter=0;counter<bootLoopCount;){
		counter++;
	}
	GPIOC->BSRRH = GPIO_Pin_1; //PC1 0V
	if(counter!=bootLoopCount){
		glitchedBoot=1;
	}
	if(glitchedBoot){
		oled_sendchars(21,OLED_bootGlitchedScreen);
	}
	else{
		oled_sendchars(21,OLED_bootNormalScreen);
	}
	oled_sendchars(36,OLED_initScreen);





	//////////////////////
	//MAIN FUNCTION LOOP//
	//////////////////////

	//Note: comment all hardware cryptographic commands if using STM32F407IGT6 instead of using STM32F417IGT6
	while (1) {
		//Main loop variable (re)initialization
		for (i = 0; i < RXBUFFERLENGTH; i++) rxBuffer[i] = 0; //Zero the rxBuffer
		for (i = 0; i < MAXAESROUNDS; i++) keyScheduleAES[i] = 0; //Zero the AES key schedule (T-tables AES implementation)
		charIdx = 0; //RSA: Global variable with offset for reading the ciphertext, init to 0
		payload_len = 0x0000; //RSA: Length of the ciphertext; init to zero, expected value for 1024bit RSA=128byte
		cmd=0;

		//Main processing section: select and execute cipher&mode
		get_char(&cmd);
		switch (cmd) {

		/////////Software crypto commands/////////
		    //Software DES - encrypt
		case (0x44):
			get_bytes(8, rxBuffer);

			//reset!

		NVIC_SystemReset();

		// Receive DES plaintext
			//GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			//des(keyDES, rxBuffer, ENCRYPT); // Perform software DES encryption
			//GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
			send_bytes(8, rxBuffer); // Transmit back ciphertext via UART
		break;

			//Software DES - decrypt
		case (0x45):
			get_bytes(8, rxBuffer); // Receive DES ciphertext


			for (j = 0; j<64; j++) bitsliced[j] = 0;
			hex_to_bit_array(rxBuffer, bitsliced);

			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on

			//present

			rng_addr = &val_ran_array;


			present(bitsliced, key, rng_addr);


			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off

			for (i = 0; i < RXBUFFERLENGTH; i++) rxBuffer[i] = 0; //Zero the rxBuffer
			bit_array_to_hex(bitsliced, rxBuffer);

			memcpy(&rxBuffer[8], ADDR_RNG_OUT, 32);

			send_bytes(8+32, rxBuffer); 

			break;

			//Software AES128 - encrypt
		case (0xAE):
			get_bytes(16, rxBuffer); // Receive AES plaintext
			AES128_ECB_encrypt(rxBuffer, keyAES, rxBuffer + AES128LENGTHINBYTES); //Trigger is coded inside aes function
			send_bytes(16, rxBuffer + AES128LENGTHINBYTES); // Transmit back ciphertext via UART
		break;

			//Software AES128 - decrypt
		case (0xEA):
			// LSB 128 BYTES rxBuffer

		    get_bytes(128, rxBuffer); // Receive AES ciphertext
			memcpy(&pt_bitslice[0], rxBuffer, 128);
		break;

			// RSA-CRT 1024bit decryption
		case (0xAA):

				get_bytes(128, rxBuffer); // Receive AES ciphertext
				memcpy(&pt_bitslice[128], rxBuffer, 128);

			/*if (cmd == 0) { //Legacy support of RLV protocol
				get_char(&cmd);
			}
			get_char(&tmp); // Receive payload length, expect MSByte first
			payload_len |= tmp;
			payload_len <<= 8;
			get_char(&tmp);
			payload_len |= tmp;
			//Receive payload_len bytes from the USART (truncating plaintext length if too long)
			if (payload_len > RXBUFFERLENGTH) {
				payload_len = RXBUFFERLENGTH;
			}
			get_bytes(payload_len, rxBuffer);

			input_cipher_text(payload_len); // Fill the cipher text buffer "c" with incoming data bytes, assuming MSByte first and 32-bit alignment
			rsa_crt_decrypt(); // Start RSA CRT procedure, Trigger signal toggling contained within the call
			send_clear_text(); // Send content of clear text buffer "m" back to Host PC, MSByte first 32-bit alignment
			*/
		break;

			//Software AES(Ttables implementation) - encrypt
		case (0x41):
			get_bytes(16, rxBuffer); // Receive AES plaintext
			rijndaelSetupEncrypt(keyScheduleAES, keyAES, 128); //Prepare AES key schedule
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			rijndaelEncrypt(keyScheduleAES, 10, rxBuffer, rxBuffer + AES128LENGTHINBYTES); // Perform software AES encryption
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
			send_bytes(16, rxBuffer + AES128LENGTHINBYTES); // Transmit back ciphertext via UART
		break;

			//Software AES(Ttables implementation) - decrypt
		case (0x50):
			get_bytes(16, rxBuffer); // Receive AES plaintext
			rijndaelSetupDecrypt(keyScheduleAES, keyAES, 128); //Prepare AES key schedule
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			rijndaelDecrypt(keyScheduleAES, 10, rxBuffer, rxBuffer + AES128LENGTHINBYTES); // Perform software AES decryption
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
			send_bytes(16, rxBuffer + AES128LENGTHINBYTES); // Transmit back plaintext via UART
		break;

		//Software AES256 - encrypt
	case (0x60):
		get_bytes(16, rxBuffer); // Receive AES plaintext
		GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
		aes256_encrypt_ecb(&ctx, rxBuffer); // Perform software AES encryption
		GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
		send_bytes(16, rxBuffer); // Transmit back ciphertext via UART
	break;

	//Software AES256 - decrypt
case (0x61):
	get_bytes(16, rxBuffer); // Receive AES plaintext
	GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
	aes256_decrypt_ecb(&ctx, rxBuffer); // Perform software AES encryption
	GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
	send_bytes(16, rxBuffer); // Transmit back ciphertext via UART
break;

		/////////Hardware crypto commands/////////
#ifdef HW_CRYPTO_PRESENT
		    //Hardware AES128 - encrypt
		case (0xCA):
			get_bytes(16, rxBuffer);
			//Trigger pin handling moved to CRYP_AES_ECB function
			cryptoCompletedOK = CRYP_AES_ECB(MODE_ENCRYPT, keyAES, 128,	rxBuffer, (uint32_t) AES128LENGTHINBYTES, rxBuffer + AES128LENGTHINBYTES);
			if (cryptoCompletedOK == SUCCESS) {
				send_bytes(16, rxBuffer + AES128LENGTHINBYTES);
			} else {
				send_bytes(16, zeros);
			}
		break;

			//Hardware AES128 - decrypt
		case (0xFE):
			get_bytes(16, rxBuffer);
			//Trigger pin handling moved to CRYP_AES_ECB function
			cryptoCompletedOK = CRYP_AES_ECB(MODE_DECRYPT, keyAES, 128,	rxBuffer, (uint32_t) AES128LENGTHINBYTES, rxBuffer + AES128LENGTHINBYTES);
			if (cryptoCompletedOK == SUCCESS) {
				send_bytes(16, rxBuffer + AES128LENGTHINBYTES);
			} else {
				send_bytes(16, zeros);
			}
		break;

		    //Hardware DES - encrypt
		case (0xBE):
			get_bytes(8, rxBuffer);
			//Trigger pin handling moved to CRYP_DES_ECB function
			cryptoCompletedOK=CRYP_DES_ECB(MODE_ENCRYPT,keyDES,rxBuffer,(uint32_t)8,rxBuffer+8);
			if (cryptoCompletedOK == SUCCESS) {
				send_bytes(8, rxBuffer + 8);
			} else {
				send_bytes(8, zeros);
			}
		break;

			//Hardware DES - decrypt
		case (0xEF):
			get_bytes(8, rxBuffer);
			//Trigger pin handling moved to CRYP_DES_ECB function
			cryptoCompletedOK=CRYP_DES_ECB(MODE_DECRYPT,keyDES,rxBuffer,(uint32_t)8,rxBuffer+8);
			if (cryptoCompletedOK == SUCCESS) {
				send_bytes(8, rxBuffer + 8);
			} else {
				send_bytes(8, zeros);
			}
		break;

		    //Hardware TDES - encrypt
		case (0xC0):
			get_bytes(8, rxBuffer);
			//Trigger pin handling moved to CRYP_DES_ECB function
			cryptoCompletedOK=CRYP_TDES_ECB(MODE_ENCRYPT,keyTDES,rxBuffer,(uint32_t)8,rxBuffer+8);
			if (cryptoCompletedOK == SUCCESS) {
				send_bytes(8, rxBuffer + 8);
			} else {
				send_bytes(8, zeros);
			}
		break;

			//Hardware TDES - decrypt
		case (0x01):
			get_bytes(8, rxBuffer);
			//Trigger pin handling moved to CRYP_DES_ECB function
			cryptoCompletedOK=CRYP_TDES_ECB(MODE_DECRYPT,keyTDES,rxBuffer,(uint32_t)8,rxBuffer+8);
			if (cryptoCompletedOK == SUCCESS) {
				send_bytes(8, rxBuffer + 8);
			} else {
				send_bytes(8, zeros);
			}
		break;
#endif
		//////Cryptographic keys management//////

			//TDES key change
		case(0xC7):
			get_bytes(24, rxBuffer);
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			for (i = 0; i < 24; i++) keyTDES[i] = rxBuffer[i];
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
			send_bytes(24,rxBuffer);

		break;
			//DES key change
		case(0xD7):
			get_bytes(8, rxBuffer);
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			for (i = 0; i < 8; i++) keyDES[i] = rxBuffer[i];
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
			send_bytes(8,rxBuffer);
		break;
			//AES128 key change
		case(0xE7):
			get_bytes(16, rxBuffer);
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			for (i = 0; i < 16; i++) keyAES[i] = rxBuffer[i];
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
			send_bytes(16,rxBuffer);
		break;
		//AES256 key change
		case(0xF7):
			get_bytes(32, rxBuffer);
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			for (i = 0; i < 32; i++) keyAES256[i] = rxBuffer[i];
			aes256_init(&ctx,keyAES256); //Prepare AES key schedule
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
			send_bytes(32,rxBuffer);
		break;
		
		//userworkshop
		//ECC init
		case(0x90):
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			ecc_init();
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
			send_ecnr_curveparams();
		break;

		//userworkshop
		//ECNR sign message
		case(0x91):
			get_bytes(20, rxBuffer);
			for (i = 15; i < RXBUFFERLENGTH; i++) rxBuffer[i] = 0; //Zero the rxBuffer
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			NR_Signature( rxBuffer, 15, &Base, &Signer.prvt_key, &signature);
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
			send_ecnr_signature();
		break;
		//userworkshop
		//ECNR verify signature
		case(0x92):
			get_bytes(50, rxBuffer); //Message is first 20 bytes, c is next 15 byte, d is next 15 bytes
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			read_c_d_fromRxBuffer();
			for (i = 15; i < RXBUFFERLENGTH; i++) rxBuffer[i] = 0; //Zero the c,d from rxBuffer
			tmp=NR_Verify(rxBuffer, 15, &Base, &Signer.pblc_key, &signature);
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
			send_char(tmp);
		break;
		//userworkshop
		//ECNR read curve details
		case(0x93):
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			send_ec_curveparams();
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
		break;
		//ECNR set curve details
		case(0x94):
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			get_ec_curveparams();
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
		break;
		//userworkshop
		//ECNR set keypair
		case(0x95):
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			get_ecnr_keypair();
			send_ecnr_curveparams();
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
		break;


		//////////////////////

		/////Test commands/////
			//Test command for the OLED screen
		case(0x30):
			oled_reset();
			oled_clear();
			oled_sendchar('H');
			oled_sendchar('i');
			oled_sendchar(' ');
			oled_sendchar('!');
		break;
			//Loop test command
		case(0xDD):{
			volatile int upCounter=0;
			payload_len=0;
			get_char(&tmp); // Receive payload length, expect MSByte first, 16bit counter max
			payload_len |= tmp;
			payload_len <<= 8;
			get_char(&tmp);
			payload_len |= tmp;
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			while(payload_len){
				payload_len--;
				upCounter++;
			}
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
			send_char(0xA5);
			send_char((payload_len>>8)&0x000000FF); //MSB first
			send_char( payload_len    &0x000000FF);
			send_char((upCounter>>8)  &0x000000FF);
			send_char( upCounter      &0x000000FF);
			send_char(0xA5);
			oled_sendchars(21,OLED_bootGlitchedScreen);
			break;
		}
		//Infinite loop
	case(0x99):{
		GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
		while(1){
			oled_sendchar(".");
			sleep_ms(500);
			oled_sendchar(" ");
			sleep_ms(500);

		}
		__asm __volatile__("nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
		);
		GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
		send_char(0xFA);
		send_char(0xCC);
		send_char(0xED);
		break;
	}
		case(0x1D): //Send stm32f4 chip uid via io interface
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
		    uint32_t uidBlock1 = STM32F4ID[0];
		    uint32_t uidBlock2 = STM32F4ID[1];
		    uint32_t uidBlock3 = STM32F4ID[2];
		    GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
			send_char((uidBlock1>>24)&0x000000FF); //MSB first
			send_char((uidBlock1>>16)&0x000000FF);
			send_char((uidBlock1>> 8)&0x000000FF);
			send_char( uidBlock1     &0x000000FF);
			send_char((uidBlock2>>24)&0x000000FF); //MSB first
			send_char((uidBlock2>>16)&0x000000FF);
			send_char((uidBlock2>> 8)&0x000000FF);
			send_char( uidBlock2     &0x000000FF);
			send_char((uidBlock3>>24)&0x000000FF); //MSB first
			send_char((uidBlock3>>16)&0x000000FF);
			send_char((uidBlock3>> 8)&0x000000FF);
			send_char( uidBlock3     &0x000000FF);
		break;
		case(0x1E): //Send stm32f4 chip uid via io interface if boot glitched
		if(glitchedBoot){
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
		    uint32_t uidBlock1 = STM32F4ID[0];
		    uint32_t uidBlock2 = STM32F4ID[1];
		    uint32_t uidBlock3 = STM32F4ID[2];
		    GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
			send_char((uidBlock1>>24)&0x000000FF); //MSB first
			send_char((uidBlock1>>16)&0x000000FF);
			send_char((uidBlock1>> 8)&0x000000FF);
			send_char( uidBlock1     &0x000000FF);
			send_char((uidBlock2>>24)&0x000000FF); //MSB first
			send_char((uidBlock2>>16)&0x000000FF);
			send_char((uidBlock2>> 8)&0x000000FF);
			send_char( uidBlock2     &0x000000FF);
			send_char((uidBlock3>>24)&0x000000FF); //MSB first
			send_char((uidBlock3>>16)&0x000000FF);
			send_char((uidBlock3>> 8)&0x000000FF);
			send_char( uidBlock3     &0x000000FF);
			}
		else{
			send_bytes(12,zeros);
		}
		break;

			//Unknown command byte: return error or 4 times (0x90 0x00) if board was glitched during boot
		default:
			GPIOC->BSRRL = GPIO_Pin_2; //Trigger on
			if(glitchedBoot){
				for(i=0;i<4;i++){
					send_char(0x90);
					send_char(0x00);
				}
			}
			else{
				send_bytes(8, cmdByteIsWrong);
			}
			GPIOC->BSRRH = GPIO_Pin_2; //Trigger off
		break;
		}

	}

	//If we glitch the board out of the main loop, it will end up here (target will loop forever sending bytes 0xFA, 0xCC)
	while (1) {
		send_bytes(2, glitched);
	}
	return 0;
}

////////////////////////////////////////////////////
//            END OF MAIN FUNCTION                //
////////////////////////////////////////////////////



///////////////////////////
//FUNCTION IMPLEMENTATION//
///////////////////////////

//init(): system initialization, pin configuration and system tick configuration for timers
void init() {
	/* STM32F4 GPIO ports */

	GPIO_InitTypeDef GPPortA,GPPortC,GPPortF, GPPortH;

	//PA9: IO configuration pin. Jumper between VBUS, PA9
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPPortA.GPIO_Pin =  GPIO_Pin_9;
	GPPortA.GPIO_Mode = GPIO_Mode_IN;
	GPPortA.GPIO_OType = GPIO_OType_PP;
	GPPortA.GPIO_Speed = GPIO_Speed_50MHz;
	GPPortA.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPPortA);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	GPPortF.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_8| GPIO_Pin_9;
	GPPortF.GPIO_Mode = GPIO_Mode_OUT;
	GPPortF.GPIO_OType = GPIO_OType_PP;
	GPPortF.GPIO_Speed = GPIO_Speed_100MHz;
	GPPortF.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPPortF);

	//DEFAULT TRIGGER PIN IS PC2; utility functions defined in stm32f4xx_gpio.c in functions set_trigger() and clear_trigger() functions
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPPortC.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPPortC.GPIO_Mode = GPIO_Mode_OUT;
	GPPortC.GPIO_OType = GPIO_OType_PP;
	GPPortC.GPIO_Speed = GPIO_Speed_100MHz;
	GPPortC.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPPortC);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
	GPPortH.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPPortH.GPIO_Mode = GPIO_Mode_OUT;
	GPPortH.GPIO_OType = GPIO_OType_PP;
	GPPortH.GPIO_Speed = GPIO_Speed_100MHz;
	GPPortH.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOH, &GPPortH);
	/* Setup SysTick or crash */
	if (SysTick_Config(SystemCoreClock / 1000)) {
		CrashGracefully();
	}

	/* Enable CRYP clock for hardware crypto; disable this for STM32F407IGT6 by commenting HW_CRYPTO_PRESENT in main.h*/
#ifdef HW_CRYPTO_PRESENT
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_CRYP, ENABLE);
#endif

	/* Setup USB virtual COM port if enabled; otherwise disable as it generates noise in the power lines */
	usbSerialEnabled=GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9);
	if(usbSerialEnabled){
	USBD_Init(&USB_OTG_dev,
			USB_OTG_FS_CORE_ID,
			&USR_desc,
			&USBD_CDC_cb,
			&USR_cb);
	}

}

//usart_init: configures the usart3 interface
void usart_init(void) {
	/* USART3 configured as follows:
	 - BaudRate = 115200 baud
	 - Word Length = 8 Bits
	 - One Stop Bit
	 - No parity
	 - Hardware flow control disabled (RTS and CTS signals)
	 - Receive and transmit enabled
	 - PC10 TX pin, PC11 RX pin
	 */
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* Enable UART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);

	/* Connect PXx to USARTx_Rx*/
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

	/* Configure USART Tx as alternate function  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* USART configuration */
	USART_Init(USART3, &USART_InitStructure);

	/* Enable USART */
	USART_Cmd(USART3, ENABLE);

}

//oled_init: configures the SPI2 interface with associated GPIO pins for SS, data/cmd# and reset lines
void oled_init(){
	/* Pins used by SPI2 & GPIOs for SSD1306 OLED display
	 * PB13 = SCK == blue wire to SSD1306 OLED display
	 * PB14 = MISO == nc
	 * PB15 = MOSI == green wire to SSD1306 OLED display
	 * PF4  = SS == white wire to SSD1306 OLED display
	 * PF5 = data/cmd# line of SSD1306 OLED display == yellow wire to SSD1306 OLED display
	 * PF8 = reset line of SSD1306 OLED display == orange wire to SSD1306 OLED display
	 *
	 */
	//Enable clock for GPIO pins for SPI
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	SPI_InitTypeDef SPI_InitTypeDefStruct;

	SPI_InitTypeDefStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //APB1 bus speed=(168/4)=42MHz; SPI speed with prescaler 2-> (42/4)=21MHz
	SPI_InitTypeDefStruct.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitTypeDefStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitTypeDefStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitTypeDefStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitTypeDefStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitTypeDefStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitTypeDefStruct.SPI_CPHA = SPI_CPHA_1Edge;
	// connect SPI1 pins to SPI alternate function
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13 , GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15 , GPIO_AF_SPI2);
	SPI_Init(SPI2, &SPI_InitTypeDefStruct);
	SPI_Cmd(SPI2, ENABLE);

	//SPI interface and GPIO pins are configured: reset the OLED display
	oled_reset();
}

//////Interrupt Handlers/////////

void SysTick_Handler(void) {
	ticker++;
	if (downTicker > 0) {
		downTicker--;
	}
}
//Debugging: Hard error management
void HardFault_Handler(void) {CrashGracefully();}
void MemManage_Handler(void) {CrashGracefully();}
void BusFault_Handler(void) {CrashGracefully();}
void UsageFault_Handler(void) {CrashGracefully();}


////////I/O utility functions (UART, serial over USB)////////////

//Wrapper functions for UART / serial over USB
//get_bytes: get an amount of nbytes bytes from IO interface into byte array ba
void get_bytes(uint32_t nbytes, uint8_t* ba) {
	if(usbSerialEnabled){
		get_bytes_usb(nbytes,ba);
	}
	else {
		get_bytes_uart(nbytes,ba);
	}
}
//send_bytes: send an amount of nbytes bytes from byte array ba via IO interface
void send_bytes(uint32_t nbytes, uint8_t* ba) {
	if(usbSerialEnabled){
		send_bytes_usb(nbytes,ba);
	}
	else {
		send_bytes_uart(nbytes,ba);
	}
}
//get_char: receive a byte via IO interface
void get_char(uint8_t *ch) {
	if(usbSerialEnabled){
		get_char_usb(ch);
	}
	else {
		get_char_uart(ch);
	}
}
//send_char: send a byte via IO interface
void send_char(uint8_t ch) {
	if(usbSerialEnabled){
		send_char_usb(ch);
	}
	else {
		send_char_uart(ch);
	}
}

//UART IO
//get_bytes: get an amount of nbytes bytes from uart into byte array ba
void get_bytes_uart(uint32_t nbytes, uint8_t* ba) {
	int i;
	for (i = 0; i < nbytes; i++) {
		while ((USART3->SR & USART_SR_RXNE) == 0) {
		}

		ba[i] = (uint8_t) USART_ReceiveData(USART3);
	}
}
//send_bytes: send an amount of nbytes bytes from byte array ba via uart
void send_bytes_uart(uint32_t nbytes, uint8_t* ba) {
	int i;
	for (i = 0; i < nbytes; i++) {
		while (!(USART3->SR & USART_SR_TXE)) {
		}
		USART_SendData(USART3, ba[i]);
	}
}
//get_char: receive a byte via uart
void get_char_uart(uint8_t *ch) {
	while ((USART3->SR & USART_SR_RXNE) == 0) {
	}
	*ch = (uint8_t) USART_ReceiveData(USART3);
}
//send_char: send a byte via uart
void send_char_uart(uint8_t ch) {
	while (!(USART3->SR & USART_SR_TXE)) {
	}
	USART_SendData(USART3, ch);
}

//Serial over USB communication functions
//get_bytes: get an amount of nbytes bytes into byte array ba via usb com port
void get_bytes_usb(uint32_t nbytes, uint8_t* ba) {
	int i;
	uint8_t tmp;
	for (i = 0; i < nbytes; i++) {
		tmp=0;
		while(!VCP_get_char(&tmp)){};
		ba[i] = tmp;
	}
}
//send_bytes: send an amount of nbytes bytes from byte array ba via usb com port
void send_bytes_usb(uint32_t nbytes, uint8_t* ba) {
	int i;
	for (i = 0; i < nbytes; i++) {
		VCP_put_char(ba[i]);
	}

}
//get_char: receive a byte over usb com port
void get_char_usb(uint8_t *ch) {
	uint8_t tmp=0;
	while(!VCP_get_char(&tmp)){};
	*ch = tmp;
}
//send_char: send a byte over usb com port
void send_char_usb(uint8_t ch) {
	VCP_put_char(ch);
}

//USB IRQ handlers
void OTG_FS_IRQHandler(void)
{
	if(usbSerialEnabled){
		USBD_OTG_ISR_Handler (&USB_OTG_dev);
	}
}

void OTG_FS_WKUP_IRQHandler(void)
{
	if(usbSerialEnabled){
		if(USB_OTG_dev.cfg.low_power)
		{
			*(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
			SystemInit();
			USB_OTG_UngateClock(&USB_OTG_dev);
		}
		EXTI_ClearITPendingBit(EXTI_Line18);
	}
}

//TRNG init
void RNG_Config(void)
{
	  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

	  /* RNG Peripheral enable */
	  RNG_Cmd(ENABLE);
}

/////Debug functions for your own code (e.g. RSA implementations)////////
void readByteFromInputBuffer(uint8_t *ch) {
	*ch = rxBuffer[charIdx]; //charIdx is a global variable, defined in rsacrt.c/.h
	charIdx++;
}
void CrashGracefully(void) {
	//Put anything you would like here to happen on a hard fault
	GPIOF->BSRRH = GPIO_Pin_6; //Example handler: PF6 enabled
}

void ecc_init(){
	init_opt_math();
	random_seed = 0xFEEDFACE;

	/*  compute curve order from Koblitz data  */

	ascii_to_bigint(&string1, &prime_order);
	int_to_field( &prime_order, &Base.pnt_order);
	null( &Base.cofactor);
	Base.cofactor.e[NUMWORD] = 2;

	/*  create Koblitz curve  */

	Base.crv.form = 1;
	one(&Base.crv.a2);
	one(&Base.crv.a6);

	/*  create base point of known order with no cofactor  */

	rand_point( &temp, &Base.crv);
	edbl( &temp, &Base.pnt, &Base.crv);


	/*  create a secret key for testing. Note that secret key must be less than order.
		The standard implies that the field size which can be used is one bit less than
		the length of the public base point order.
	 */

	ECKGP( &Base, &Signer);
}

void send_ecnr_curveparams(){
	//Send the numbers MSB to LSB
	//Public point x
	send_char((Signer.pblc_key.x.e[0]&0xFF000000) >> 24);
	send_char((Signer.pblc_key.x.e[0]&0x00FF0000) >> 16);
	send_char((Signer.pblc_key.x.e[0]&0x0000FF00) >> 8);
	send_char((Signer.pblc_key.x.e[0]&0x000000FF));
	send_char((Signer.pblc_key.x.e[1]&0xFF000000) >> 24);
	send_char((Signer.pblc_key.x.e[1]&0x00FF0000) >> 16);
	send_char((Signer.pblc_key.x.e[1]&0x0000FF00) >> 8);
	send_char((Signer.pblc_key.x.e[1]&0x000000FF));
	send_char((Signer.pblc_key.x.e[2]&0xFF000000) >> 24);
	send_char((Signer.pblc_key.x.e[2]&0x00FF0000) >> 16);
	send_char((Signer.pblc_key.x.e[2]&0x0000FF00) >> 8);
	send_char((Signer.pblc_key.x.e[2]&0x000000FF));
	send_char((Signer.pblc_key.x.e[3]&0xFF000000) >> 24);
	send_char((Signer.pblc_key.x.e[3]&0x00FF0000) >> 16);
	send_char((Signer.pblc_key.x.e[3]&0x0000FF00) >> 8);
	send_char((Signer.pblc_key.x.e[3]&0x000000FF));
	//Public point y
	send_char((Signer.pblc_key.y.e[0]&0xFF000000) >> 24);
	send_char((Signer.pblc_key.y.e[0]&0x00FF0000) >> 16);
	send_char((Signer.pblc_key.y.e[0]&0x0000FF00) >> 8);
	send_char((Signer.pblc_key.y.e[0]&0x000000FF));
	send_char((Signer.pblc_key.y.e[1]&0xFF000000) >> 24);
	send_char((Signer.pblc_key.y.e[1]&0x00FF0000) >> 16);
	send_char((Signer.pblc_key.y.e[1]&0x0000FF00) >> 8);
	send_char((Signer.pblc_key.y.e[1]&0x000000FF));
	send_char((Signer.pblc_key.y.e[2]&0xFF000000) >> 24);
	send_char((Signer.pblc_key.y.e[2]&0x00FF0000) >> 16);
	send_char((Signer.pblc_key.y.e[2]&0x0000FF00) >> 8);
	send_char((Signer.pblc_key.y.e[2]&0x000000FF));
	send_char((Signer.pblc_key.y.e[3]&0xFF000000) >> 24);
	send_char((Signer.pblc_key.y.e[3]&0x00FF0000) >> 16);
	send_char((Signer.pblc_key.y.e[3]&0x0000FF00) >> 8);
	send_char((Signer.pblc_key.y.e[3])&0x000000FF);
	//Private key
	send_char((Signer.prvt_key.e[0]&0xFF000000) >> 24);
	send_char((Signer.prvt_key.e[0]&0x00FF0000) >> 16);
	send_char((Signer.prvt_key.e[0]&0x0000FF00) >> 8);
	send_char((Signer.prvt_key.e[0]&0x000000FF));
	send_char((Signer.prvt_key.e[1]&0xFF000000) >> 24);
	send_char((Signer.prvt_key.e[1]&0x00FF0000) >> 16);
	send_char((Signer.prvt_key.e[1]&0x0000FF00) >> 8);
	send_char((Signer.prvt_key.e[1]&0x000000FF));
	send_char((Signer.prvt_key.e[2]&0xFF000000) >> 24);
	send_char((Signer.prvt_key.e[2]&0x00FF0000) >> 16);
	send_char((Signer.prvt_key.e[2]&0x0000FF00) >> 8);
	send_char((Signer.prvt_key.e[2]&0x000000FF));
	send_char((Signer.prvt_key.e[3]&0xFF000000) >> 24);
	send_char((Signer.prvt_key.e[3]&0x00FF0000) >> 16);
	send_char((Signer.prvt_key.e[3]&0x0000FF00) >> 8);
	send_char((Signer.prvt_key.e[3])&0x000000FF);
}

void send_ecnr_signature(){
	//Send the numbers MSB to LSB
	send_char(((signature.c.e[0]) &0xFF000000) >> 24);
	send_char(((signature.c.e[0])&0x00FF0000) >> 16);
	send_char(((signature.c.e[0])&0x0000FF00) >> 8);
	send_char(((signature.c.e[0])&0x000000FF));
	send_char(((signature.c.e[1])&0xFF000000) >> 24);
	send_char(((signature.c.e[1])&0x00FF0000) >> 16);
	send_char(((signature.c.e[1])&0x0000FF00) >> 8);
	send_char(((signature.c.e[1])&0x000000FF));
	send_char(((signature.c.e[2])&0xFF000000) >> 24);
	send_char(((signature.c.e[2])&0x00FF0000) >> 16);
	send_char(((signature.c.e[2])&0x0000FF00) >> 8);
	send_char(((signature.c.e[2])&0x000000FF));
	send_char(((signature.c.e[3])&0xFF000000) >> 24);
	send_char(((signature.c.e[3])&0x00FF0000) >> 16);
	send_char(((signature.c.e[3])&0x0000FF00) >> 8);
	send_char(((signature.c.e[3])&0x000000FF));
	send_char(((signature.d.e[0])&0xFF000000) >> 24);
	send_char(((signature.d.e[0])&0x00FF0000) >> 16);
	send_char(((signature.d.e[0])&0x0000FF00) >> 8);
	send_char(((signature.d.e[0])&0x000000FF));
	send_char(((signature.d.e[1])&0xFF000000) >> 24);
	send_char(((signature.d.e[1])&0x00FF0000) >> 16);
	send_char(((signature.d.e[1])&0x0000FF00) >> 8);
	send_char(((signature.d.e[1])&0x000000FF));
	send_char(((signature.d.e[2])&0xFF000000) >> 24);
	send_char(((signature.d.e[2])&0x00FF0000) >> 16);
	send_char(((signature.d.e[2])&0x0000FF00) >> 8);
	send_char(((signature.d.e[2])&0x000000FF));
	send_char(((signature.d.e[3])&0xFF000000) >> 24);
	send_char(((signature.d.e[3])&0x00FF0000) >> 16);
	send_char(((signature.d.e[3])&0x0000FF00) >> 8);
	send_char(((signature.d.e[3])&0x000000FF));
}

void send_ec_curveparams(){
	//Send the numbers MSB to LSB, 98 bytes in total (2 = 6*16)
	//Base.crv.form == short int (2 bytes)
	send_char(((Base.crv.form) &0xFF00) >> 8);
	send_char(((Base.crv.form) &0x00FF));
	//Base.crv.a2 (4x uint32)
	send_char(((Base.crv.a2.e[0])&0xFF000000) >> 24);
	send_char(((Base.crv.a2.e[0])&0x00FF0000) >> 16);
	send_char(((Base.crv.a2.e[0])&0x0000FF00) >> 8);
	send_char(((Base.crv.a2.e[0])&0x000000FF));
	send_char(((Base.crv.a2.e[1])&0xFF000000) >> 24);
	send_char(((Base.crv.a2.e[1])&0x00FF0000) >> 16);
	send_char(((Base.crv.a2.e[1])&0x0000FF00) >> 8);
	send_char(((Base.crv.a2.e[1])&0x000000FF));
	send_char(((Base.crv.a2.e[2])&0xFF000000) >> 24);
	send_char(((Base.crv.a2.e[2])&0x00FF0000) >> 16);
	send_char(((Base.crv.a2.e[2])&0x0000FF00) >> 8);
	send_char(((Base.crv.a2.e[2])&0x000000FF));
	send_char(((Base.crv.a2.e[3])&0xFF000000) >> 24);
	send_char(((Base.crv.a2.e[3])&0x00FF0000) >> 16);
	send_char(((Base.crv.a2.e[3])&0x0000FF00) >> 8);
	send_char(((Base.crv.a2.e[3])&0x000000FF));
	//Base.crv.a6 (4x uint32)
	send_char(((Base.crv.a6.e[0])&0xFF000000) >> 24);
	send_char(((Base.crv.a6.e[0])&0x00FF0000) >> 16);
	send_char(((Base.crv.a6.e[0])&0x0000FF00) >> 8);
	send_char(((Base.crv.a6.e[0])&0x000000FF));
	send_char(((Base.crv.a6.e[1])&0xFF000000) >> 24);
	send_char(((Base.crv.a6.e[1])&0x00FF0000) >> 16);
	send_char(((Base.crv.a6.e[1])&0x0000FF00) >> 8);
	send_char(((Base.crv.a6.e[1])&0x000000FF));
	send_char(((Base.crv.a6.e[2])&0xFF000000) >> 24);
	send_char(((Base.crv.a6.e[2])&0x00FF0000) >> 16);
	send_char(((Base.crv.a6.e[2])&0x0000FF00) >> 8);
	send_char(((Base.crv.a6.e[2])&0x000000FF));
	send_char(((Base.crv.a6.e[3])&0xFF000000) >> 24);
	send_char(((Base.crv.a6.e[3])&0x00FF0000) >> 16);
	send_char(((Base.crv.a6.e[3])&0x0000FF00) >> 8);
	send_char(((Base.crv.a6.e[3])&0x000000FF));
	//Base.point.x (4x uint32 = 16 bytes)
	send_char(((Base.pnt.x.e[0])&0xFF000000) >> 24);
	send_char(((Base.pnt.x.e[0])&0x00FF0000) >> 16);
	send_char(((Base.pnt.x.e[0])&0x0000FF00) >> 8);
	send_char(((Base.pnt.x.e[0])&0x000000FF));
	send_char(((Base.pnt.x.e[1])&0xFF000000) >> 24);
	send_char(((Base.pnt.x.e[1])&0x00FF0000) >> 16);
	send_char(((Base.pnt.x.e[1])&0x0000FF00) >> 8);
	send_char(((Base.pnt.x.e[1])&0x000000FF));
	send_char(((Base.pnt.x.e[2])&0xFF000000) >> 24);
	send_char(((Base.pnt.x.e[2])&0x00FF0000) >> 16);
	send_char(((Base.pnt.x.e[2])&0x0000FF00) >> 8);
	send_char(((Base.pnt.x.e[2])&0x000000FF));
	send_char(((Base.pnt.x.e[3])&0xFF000000) >> 24);
	send_char(((Base.pnt.x.e[3])&0x00FF0000) >> 16);
	send_char(((Base.pnt.x.e[3])&0x0000FF00) >> 8);
	send_char(((Base.pnt.x.e[3])&0x000000FF));
	//Base.point.y (4x uint32 = 16 bytes)
	send_char(((Base.pnt.y.e[0])&0xFF000000) >> 24);
	send_char(((Base.pnt.y.e[0])&0x00FF0000) >> 16);
	send_char(((Base.pnt.y.e[0])&0x0000FF00) >> 8);
	send_char(((Base.pnt.y.e[0])&0x000000FF));
	send_char(((Base.pnt.y.e[1])&0xFF000000) >> 24);
	send_char(((Base.pnt.y.e[1])&0x00FF0000) >> 16);
	send_char(((Base.pnt.y.e[1])&0x0000FF00) >> 8);
	send_char(((Base.pnt.y.e[1])&0x000000FF));
	send_char(((Base.pnt.y.e[2])&0xFF000000) >> 24);
	send_char(((Base.pnt.y.e[2])&0x00FF0000) >> 16);
	send_char(((Base.pnt.y.e[2])&0x0000FF00) >> 8);
	send_char(((Base.pnt.y.e[2])&0x000000FF));
	send_char(((Base.pnt.y.e[3])&0xFF000000) >> 24);
	send_char(((Base.pnt.y.e[3])&0x00FF0000) >> 16);
	send_char(((Base.pnt.y.e[3])&0x0000FF00) >> 8);
	send_char(((Base.pnt.y.e[3])&0x000000FF));
	//Base.pnt_order (4xuint32 = 16 bytes)
	send_char(((Base.pnt_order.e[0])&0xFF000000) >> 24);
	send_char(((Base.pnt_order.e[0])&0x00FF0000) >> 16);
	send_char(((Base.pnt_order.e[0])&0x0000FF00) >> 8);
	send_char(((Base.pnt_order.e[0])&0x000000FF));
	send_char(((Base.pnt_order.e[1])&0xFF000000) >> 24);
	send_char(((Base.pnt_order.e[1])&0x00FF0000) >> 16);
	send_char(((Base.pnt_order.e[1])&0x0000FF00) >> 8);
	send_char(((Base.pnt_order.e[1])&0x000000FF));
	send_char(((Base.pnt_order.e[2])&0xFF000000) >> 24);
	send_char(((Base.pnt_order.e[2])&0x00FF0000) >> 16);
	send_char(((Base.pnt_order.e[2])&0x0000FF00) >> 8);
	send_char(((Base.pnt_order.e[2])&0x000000FF));
	send_char(((Base.pnt_order.e[3])&0xFF000000) >> 24);
	send_char(((Base.pnt_order.e[3])&0x00FF0000) >> 16);
	send_char(((Base.pnt_order.e[3])&0x0000FF00) >> 8);
	send_char(((Base.pnt_order.e[3])&0x000000FF));
	//Base.cofactor (4xuint32 = 16 bytes)
	send_char(((Base.cofactor.e[0])&0xFF000000) >> 24);
	send_char(((Base.cofactor.e[0])&0x00FF0000) >> 16);
	send_char(((Base.cofactor.e[0])&0x0000FF00) >> 8);
	send_char(((Base.cofactor.e[0])&0x000000FF));
	send_char(((Base.cofactor.e[1])&0xFF000000) >> 24);
	send_char(((Base.cofactor.e[1])&0x00FF0000) >> 16);
	send_char(((Base.cofactor.e[1])&0x0000FF00) >> 8);
	send_char(((Base.cofactor.e[1])&0x000000FF));
	send_char(((Base.cofactor.e[2])&0xFF000000) >> 24);
	send_char(((Base.cofactor.e[2])&0x00FF0000) >> 16);
	send_char(((Base.cofactor.e[2])&0x0000FF00) >> 8);
	send_char(((Base.cofactor.e[2])&0x000000FF));
	send_char(((Base.cofactor.e[3])&0xFF000000) >> 24);
	send_char(((Base.cofactor.e[3])&0x00FF0000) >> 16);
	send_char(((Base.cofactor.e[3])&0x0000FF00) >> 8);
	send_char(((Base.cofactor.e[3])&0x000000FF));
}

void get_ec_curveparams(){
	//Read the numbers MSB to LSB, 98 bytes in total (2 = 6*16)
	uint8_t tmp[98];
	uint8_t i;
	i=0;
	get_bytes(98,tmp);
	//Base.crv.form == short int (2 bytes)
	Base.crv.form=0;
	Base.crv.form=tmp[i];
	Base.crv.form=(Base.crv.form<<8) | tmp[++i];

	//Base.crv.a2 (4x uint32)
	Base.crv.a2.e[0]=0;
	Base.crv.a2.e[0]=tmp[++i];
	Base.crv.a2.e[0]=(Base.crv.a2.e[0]<<8) | tmp[++i];
	Base.crv.a2.e[0]=(Base.crv.a2.e[0]<<8) | tmp[++i];
	Base.crv.a2.e[0]=(Base.crv.a2.e[0]<<8) | tmp[++i];
	Base.crv.a2.e[1]=0;
	Base.crv.a2.e[1]=tmp[++i];
	Base.crv.a2.e[1]=(Base.crv.a2.e[1]<<8) | tmp[++i];
	Base.crv.a2.e[1]=(Base.crv.a2.e[1]<<8) | tmp[++i];
	Base.crv.a2.e[1]=(Base.crv.a2.e[1]<<8) | tmp[++i];
	Base.crv.a2.e[2]=0;
	Base.crv.a2.e[2]=tmp[++i];
	Base.crv.a2.e[2]=(Base.crv.a2.e[2]<<8) | tmp[++i];
	Base.crv.a2.e[2]=(Base.crv.a2.e[2]<<8) | tmp[++i];
	Base.crv.a2.e[2]=(Base.crv.a2.e[2]<<8) | tmp[++i];
	Base.crv.a2.e[3]=0;
	Base.crv.a2.e[3]=tmp[++i];
	Base.crv.a2.e[3]=(Base.crv.a2.e[3]<<8) | tmp[++i];
	Base.crv.a2.e[3]=(Base.crv.a2.e[3]<<8) | tmp[++i];
	Base.crv.a2.e[3]=(Base.crv.a2.e[3]<<8) | tmp[++i];
	//Base.crv.a6 (4x uint32)
	Base.crv.a6.e[0]=0;
	Base.crv.a6.e[0]= tmp[++i];
	Base.crv.a6.e[0]=(Base.crv.a6.e[0]<<8) | tmp[++i];
	Base.crv.a6.e[0]=(Base.crv.a6.e[0]<<8) | tmp[++i];
	Base.crv.a6.e[0]=(Base.crv.a6.e[0]<<8) | tmp[++i];
	Base.crv.a6.e[1]=0;
	Base.crv.a6.e[1]=tmp[++i];
	Base.crv.a6.e[1]=(Base.crv.a6.e[1]<<8) | tmp[++i];
	Base.crv.a6.e[1]=(Base.crv.a6.e[1]<<8) | tmp[++i];
	Base.crv.a6.e[1]=(Base.crv.a6.e[1]<<8) | tmp[++i];
	Base.crv.a6.e[2]=0;
	Base.crv.a6.e[2]=tmp[++i];
	Base.crv.a6.e[2]=(Base.crv.a6.e[2]<<8) | tmp[++i];
	Base.crv.a6.e[2]=(Base.crv.a6.e[2]<<8) | tmp[++i];
	Base.crv.a6.e[2]=(Base.crv.a6.e[2]<<8) | tmp[++i];
	Base.crv.a6.e[3]=0;
	Base.crv.a6.e[3]=tmp[++i];
	Base.crv.a6.e[3]=(Base.crv.a6.e[3]<<8) | tmp[++i];
	Base.crv.a6.e[3]=(Base.crv.a6.e[3]<<8) | tmp[++i];
	Base.crv.a6.e[3]=(Base.crv.a6.e[3]<<8) | tmp[++i];
	//Base.pnt.x (4x uint32 = 16 bytes)
	Base.pnt.x.e[0]=0;
	Base.pnt.x.e[0]=tmp[++i];
	Base.pnt.x.e[0]=(Base.pnt.x.e[0]<<8) | tmp[++i];
	Base.pnt.x.e[0]=(Base.pnt.x.e[0]<<8) | tmp[++i];
	Base.pnt.x.e[0]=(Base.pnt.x.e[0]<<8) | tmp[++i];
	Base.pnt.x.e[1]=0;
	Base.pnt.x.e[1]=tmp[++i];
	Base.pnt.x.e[1]=(Base.pnt.x.e[1]<<8) | tmp[++i];
	Base.pnt.x.e[1]=(Base.pnt.x.e[1]<<8) | tmp[++i];
	Base.pnt.x.e[1]=(Base.pnt.x.e[1]<<8) | tmp[++i];
	Base.pnt.x.e[2]=0;
	Base.pnt.x.e[2]=tmp[++i];
	Base.pnt.x.e[2]=(Base.pnt.x.e[2]<<8) | tmp[++i];
	Base.pnt.x.e[2]=(Base.pnt.x.e[2]<<8) | tmp[++i];
	Base.pnt.x.e[2]=(Base.pnt.x.e[2]<<8) | tmp[++i];
	Base.pnt.x.e[3]=0;
	Base.pnt.x.e[3]=tmp[++i];
	Base.pnt.x.e[3]=(Base.pnt.x.e[3]<<8) | tmp[++i];
	Base.pnt.x.e[3]=(Base.pnt.x.e[3]<<8) | tmp[++i];
	Base.pnt.x.e[3]=(Base.pnt.x.e[3]<<8) | tmp[++i];
	//Base.pnt.y (4x uint32 = 16 bytes)
	Base.pnt.y.e[0]=0;
	Base.pnt.y.e[0]=tmp[++i];
	Base.pnt.y.e[0]=(Base.pnt.y.e[0]<<8) | tmp[++i];
	Base.pnt.y.e[0]=(Base.pnt.y.e[0]<<8) | tmp[++i];
	Base.pnt.y.e[0]=(Base.pnt.y.e[0]<<8) | tmp[++i];
	Base.pnt.y.e[1]=0;
	Base.pnt.y.e[1]=tmp[++i];
	Base.pnt.y.e[1]=(Base.pnt.y.e[1]<<8) | tmp[++i];
	Base.pnt.y.e[1]=(Base.pnt.y.e[1]<<8) | tmp[++i];
	Base.pnt.y.e[1]=(Base.pnt.y.e[1]<<8) | tmp[++i];
	Base.pnt.y.e[2]=0;
	Base.pnt.y.e[2]=tmp[++i];
	Base.pnt.y.e[2]=(Base.pnt.y.e[2]<<8) | tmp[++i];
	Base.pnt.y.e[2]=(Base.pnt.y.e[2]<<8) | tmp[++i];
	Base.pnt.y.e[2]=(Base.pnt.y.e[2]<<8) | tmp[++i];
	Base.pnt.y.e[3]=0;
	Base.pnt.y.e[3]=tmp[++i];
	Base.pnt.y.e[3]=(Base.pnt.y.e[3]<<8) | tmp[++i];
	Base.pnt.y.e[3]=(Base.pnt.y.e[3]<<8) | tmp[++i];
	Base.pnt.y.e[3]=(Base.pnt.y.e[3]<<8) | tmp[++i];
	//Base.pnt_order (4xuint32 = 16 bytes)
	Base.pnt_order.e[0]=0;
	Base.pnt_order.e[0]=tmp[++i];
	Base.pnt_order.e[0]=(Base.pnt_order.e[0]<<8) | tmp[++i];
	Base.pnt_order.e[0]=(Base.pnt_order.e[0]<<8) | tmp[++i];
	Base.pnt_order.e[0]=(Base.pnt_order.e[0]<<8) | tmp[++i];
	Base.pnt_order.e[1]=0;
	Base.pnt_order.e[1]=tmp[++i];
	Base.pnt_order.e[1]=(Base.pnt_order.e[1]<<8) | tmp[++i];
	Base.pnt_order.e[1]=(Base.pnt_order.e[1]<<8) | tmp[++i];
	Base.pnt_order.e[1]=(Base.pnt_order.e[1]<<8) | tmp[++i];
	Base.pnt_order.e[2]=0;
	Base.pnt_order.e[2]=tmp[++i];
	Base.pnt_order.e[2]=(Base.pnt_order.e[2]<<8) | tmp[++i];
	Base.pnt_order.e[2]=(Base.pnt_order.e[2]<<8) | tmp[++i];
	Base.pnt_order.e[2]=(Base.pnt_order.e[2]<<8) | tmp[++i];
	Base.pnt_order.e[3]=0;
	Base.pnt_order.e[3]=tmp[++i];
	Base.pnt_order.e[3]=(Base.pnt_order.e[3]<<8) | tmp[++i];
	Base.pnt_order.e[3]=(Base.pnt_order.e[3]<<8) | tmp[++i];
	Base.pnt_order.e[3]=(Base.pnt_order.e[3]<<8) | tmp[++i];
	//Base.cofactor (4xuint32 = 16 bytes)
	Base.cofactor.e[0]=0;
	Base.cofactor.e[0]=tmp[++i];
	Base.cofactor.e[0]=(Base.cofactor.e[0]<<8) | tmp[++i];
	Base.cofactor.e[0]=(Base.cofactor.e[0]<<8) | tmp[++i];
	Base.cofactor.e[0]=(Base.cofactor.e[0]<<8) | tmp[++i];
	Base.cofactor.e[1]=0;
	Base.cofactor.e[1]=tmp[++i];
	Base.cofactor.e[1]=(Base.cofactor.e[1]<<8) | tmp[++i];
	Base.cofactor.e[1]=(Base.cofactor.e[1]<<8) | tmp[++i];
	Base.cofactor.e[1]=(Base.cofactor.e[1]<<8) | tmp[++i];
	Base.cofactor.e[2]=0;
	Base.cofactor.e[2]=tmp[++i];
	Base.cofactor.e[2]=(Base.cofactor.e[2]<<8) | tmp[++i];
	Base.cofactor.e[2]=(Base.cofactor.e[2]<<8) | tmp[++i];
	Base.cofactor.e[2]=(Base.cofactor.e[2]<<8) | tmp[++i];
	Base.cofactor.e[3]=0;
	Base.cofactor.e[3]=tmp[++i];
	Base.cofactor.e[3]=(Base.cofactor.e[3]<<8) | tmp[++i];
	Base.cofactor.e[3]=(Base.cofactor.e[3]<<8) | tmp[++i];
	Base.cofactor.e[3]=(Base.cofactor.e[3]<<8) | tmp[++i];
}

void get_ecnr_keypair(){
	//Read the numbers MSB to LSB, 45 bytes in total (3*15)
	uint8_t tmp[45];
	uint8_t i;
	i=0;
	get_bytes(45,tmp);
	//Signer.pblc_key.x (4x uint32)
	Signer.pblc_key.x.e[0]=0;
	Signer.pblc_key.x.e[0]=(Signer.pblc_key.x.e[0]<<8) | tmp[i];
	Signer.pblc_key.x.e[0]=(Signer.pblc_key.x.e[0]<<8) | tmp[++i];
	Signer.pblc_key.x.e[0]=(Signer.pblc_key.x.e[0]<<8) | tmp[++i];
	Signer.pblc_key.x.e[1]=0;
	Signer.pblc_key.x.e[1]=tmp[++i];
	Signer.pblc_key.x.e[1]=(Signer.pblc_key.x.e[1]<<8) | tmp[++i];
	Signer.pblc_key.x.e[1]=(Signer.pblc_key.x.e[1]<<8) | tmp[++i];
	Signer.pblc_key.x.e[1]=(Signer.pblc_key.x.e[1]<<8) | tmp[++i];
	Signer.pblc_key.x.e[2]=0;
	Signer.pblc_key.x.e[2]=tmp[++i];
	Signer.pblc_key.x.e[2]=(Signer.pblc_key.x.e[2]<<8) | tmp[++i];
	Signer.pblc_key.x.e[2]=(Signer.pblc_key.x.e[2]<<8) | tmp[++i];
	Signer.pblc_key.x.e[2]=(Signer.pblc_key.x.e[2]<<8) | tmp[++i];
	Signer.pblc_key.x.e[3]=0;
	Signer.pblc_key.x.e[3]=tmp[++i];
	Signer.pblc_key.x.e[3]=(Signer.pblc_key.x.e[3]<<8) | tmp[++i];
	Signer.pblc_key.x.e[3]=(Signer.pblc_key.x.e[3]<<8) | tmp[++i];
	Signer.pblc_key.x.e[3]=(Signer.pblc_key.x.e[3]<<8) | tmp[++i];
	//Signer.pblc_key.x (4x uint32)
	Signer.pblc_key.y.e[0]=0;
	Signer.pblc_key.y.e[0]=(Signer.pblc_key.y.e[0]<<8) | tmp[++i];
	Signer.pblc_key.y.e[0]=(Signer.pblc_key.y.e[0]<<8) | tmp[++i];
	Signer.pblc_key.y.e[0]=(Signer.pblc_key.y.e[0]<<8) | tmp[++i];
	Signer.pblc_key.y.e[1]=0;
	Signer.pblc_key.y.e[1]=tmp[++i];
	Signer.pblc_key.y.e[1]=(Signer.pblc_key.y.e[1]<<8) | tmp[++i];
	Signer.pblc_key.y.e[1]=(Signer.pblc_key.y.e[1]<<8) | tmp[++i];
	Signer.pblc_key.y.e[1]=(Signer.pblc_key.y.e[1]<<8) | tmp[++i];
	Signer.pblc_key.y.e[2]=0;
	Signer.pblc_key.y.e[2]=tmp[++i];
	Signer.pblc_key.y.e[2]=(Signer.pblc_key.y.e[2]<<8) | tmp[++i];
	Signer.pblc_key.y.e[2]=(Signer.pblc_key.y.e[2]<<8) | tmp[++i];
	Signer.pblc_key.y.e[2]=(Signer.pblc_key.y.e[2]<<8) | tmp[++i];
	Signer.pblc_key.y.e[3]=0;
	Signer.pblc_key.y.e[3]=tmp[++i];
	Signer.pblc_key.y.e[3]=(Signer.pblc_key.y.e[3]<<8) | tmp[++i];
	Signer.pblc_key.y.e[3]=(Signer.pblc_key.y.e[3]<<8) | tmp[++i];
	Signer.pblc_key.y.e[3]=(Signer.pblc_key.y.e[3]<<8) | tmp[++i];
	//Private key Signer.prvt_key
	Signer.prvt_key.e[0]=0;
	Signer.prvt_key.e[0]=(Signer.prvt_key.e[0]<<8) | tmp[++i];
	Signer.prvt_key.e[0]=(Signer.prvt_key.e[0]<<8) | tmp[++i];
	Signer.prvt_key.e[0]=(Signer.prvt_key.e[0]<<8) | tmp[++i];
	Signer.prvt_key.e[1]=0;
	Signer.prvt_key.e[1]=tmp[++i];
	Signer.prvt_key.e[1]=(Signer.prvt_key.e[1]<<8) | tmp[++i];
	Signer.prvt_key.e[1]=(Signer.prvt_key.e[1]<<8) | tmp[++i];
	Signer.prvt_key.e[1]=(Signer.prvt_key.e[1]<<8) | tmp[++i];
	Signer.prvt_key.e[2]=0;
	Signer.prvt_key.e[2]=tmp[++i];
	Signer.prvt_key.e[2]=(Signer.prvt_key.e[2]<<8) | tmp[++i];
	Signer.prvt_key.e[2]=(Signer.prvt_key.e[2]<<8) | tmp[++i];
	Signer.prvt_key.e[2]=(Signer.prvt_key.e[2]<<8) | tmp[++i];
	Signer.prvt_key.e[3]=0;
	Signer.prvt_key.e[3]=tmp[++i];
	Signer.prvt_key.e[3]=(Signer.prvt_key.e[3]<<8) | tmp[++i];
	Signer.prvt_key.e[3]=(Signer.prvt_key.e[3]<<8) | tmp[++i];
	Signer.prvt_key.e[3]=(Signer.prvt_key.e[3]<<8) | tmp[++i];
}

void read_c_d_fromRxBuffer(){
	uint8_t i=20; //Start from Offset 20 in the RX buffer
	//signature.c (15 bytes, leading zero)
	signature.c.e[0]=0;
	signature.c.e[0]=(signature.c.e[0]<<8) | rxBuffer[i];
	signature.c.e[0]=(signature.c.e[0]<<8) | rxBuffer[++i];
	signature.c.e[0]=(signature.c.e[0]<<8) | rxBuffer[++i];
	signature.c.e[1]=0;
	signature.c.e[1]=rxBuffer[++i];
	signature.c.e[1]=(signature.c.e[1]<<8) | rxBuffer[++i];
	signature.c.e[1]=(signature.c.e[1]<<8) | rxBuffer[++i];
	signature.c.e[1]=(signature.c.e[1]<<8) | rxBuffer[++i];
	signature.c.e[2]=0;
	signature.c.e[2]=rxBuffer[++i];
	signature.c.e[2]=(signature.c.e[2]<<8) | rxBuffer[++i];
	signature.c.e[2]=(signature.c.e[2]<<8) | rxBuffer[++i];
	signature.c.e[2]=(signature.c.e[2]<<8) | rxBuffer[++i];
	signature.c.e[3]=0;
	signature.c.e[3]=rxBuffer[++i];
	signature.c.e[3]=(signature.c.e[3]<<8) | rxBuffer[++i];
	signature.c.e[3]=(signature.c.e[3]<<8) | rxBuffer[++i];
	signature.c.e[3]=(signature.c.e[3]<<8) | rxBuffer[++i];

	//signature.d (15 bytes, leading zero)
	signature.d.e[0]=0;
	signature.d.e[0]=(signature.d.e[0]<<8) | rxBuffer[++i];
	signature.d.e[0]=(signature.d.e[0]<<8) | rxBuffer[++i];
	signature.d.e[0]=(signature.d.e[0]<<8) | rxBuffer[++i];
	signature.d.e[1]=0;
	signature.d.e[1]=rxBuffer[++i];
	signature.d.e[1]=(signature.d.e[1]<<8) | rxBuffer[++i];
	signature.d.e[1]=(signature.d.e[1]<<8) | rxBuffer[++i];
	signature.d.e[1]=(signature.d.e[1]<<8) | rxBuffer[++i];
	signature.d.e[2]=0;
	signature.d.e[2]=rxBuffer[++i];
	signature.d.e[2]=(signature.d.e[2]<<8) | rxBuffer[++i];
	signature.d.e[2]=(signature.d.e[2]<<8) | rxBuffer[++i];
	signature.d.e[2]=(signature.d.e[2]<<8) | rxBuffer[++i];
	signature.d.e[3]=0;
	signature.d.e[3]=rxBuffer[++i];
	signature.d.e[3]=(signature.d.e[3]<<8) | rxBuffer[++i];
	signature.d.e[3]=(signature.d.e[3]<<8) | rxBuffer[++i];
	signature.d.e[3]=(signature.d.e[3]<<8) | rxBuffer[++i];
}
