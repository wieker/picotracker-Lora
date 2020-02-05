/*
GPS_NMEA_error_bitfield
	Bit 7 0b10100000 not used - DL-FLDIGI doesn't show ASCII-8 characters																			
	Bit 6 0b01100000 NAVmode error (!6)																											'`'
	Bit 5 0b00100000 default message - NO ERROR (for all ERRORS Bit 5 changes to 0 (0b01011111) - for the character to be shown in DL-FLDIGI)	' '
	Bit 4 0b00110000 no FIX																														'0'
	Bit 3 0b00101000 UBX response timeout																										'('
	Bit 2 0b00100100 GGA checksum error																											'$'
	Bit 1 0b00100010 GGA buffer error																											'"'
	Bit 0 0b00100001 UBLOX response timeout																										'!'

GPS_UBX_error_bitfield
	Bit 7 0b10100000 not used - DL-FLDIGI doesn't show ASCII-8 characters																			
	Bit 6 0b01100000 NAVmode error (!6)																											'`'
	Bit 5 0b00100000 default message - NO ERROR (for all ERRORS Bit 5 changes to 0 (0b01011111) - for the character to be shown in DL-FLDIGI)	' '
	Bit 4 0b00110000 0611 error POWER MODE																										'0'
	Bit 3 0b00101000 0121 error TIME																											'('
	Bit 2 0b00100100 0102 error POSITION DATA / 0107 error POSITION DATA																		'$'
	Bit 1 0b00100010 SSDV communication error																									'"'
	Bit 0 0b00100001 SSDV image buffer overflow																									'!'

	character 'DEL' doesn't show in DL-FLDIGI, needs to be changed ->																			'_'
	character corresponds to ',' which is used in the parser ->																					'L'
	character corresponds to '*' which is used in the parser ->																					'J'
	character corresponds to '$' which is used in the parser ->																					'D'
*/


#ifndef UBLOX_H
#define UBLOX_H



#include "stm32l0xx_hal.h"
#include <stdint.h>




/*
	UBX		60
	NMEA	125
*/
#define GPSBUFFER_SIZE			125
#define FIX				500		// attempts to poll UBX-NAV-PVT
#define SATS				4		// number of satellites required for positional solution
#define UBX_TIMEOUT  5000 // in milliseconds

/*
	CONFIGURATION.
	Response to these UBX messages is ACK/NACK - 10 bytes.
*/
static uint8_t setGPSonly[] = {
		0xB5,0x62,0x06,0x3E,0x3C,0x00,              /* UBX-CFG-GNSS */
		0x00,0x00,0x20,0x07,                        /* use 32 channels, 7 configs following */
		0x00,0x08,0x10,0x00,0x01,0x00,0x01,0x01,    /* GPS enable */
		0x01,0x01,0x03,0x00,0x00,0x00,0x01,0x01,	/* SBAS disable */
		0x02,0x04,0x08,0x00,0x00,0x00,0x01,0x01,	/* Galileo disable */
		0x03,0x08,0x10,0x00,0x00,0x00,0x01,0x01,	/* Beidou disable */
		0x04,0x00,0x08,0x00,0x00,0x00,0x01,0x01,	/* IMES disable */
		0x05,0x00,0x03,0x00,0x01,0x00,0x01,0x01,	/* QZSS enable */
		0x06,0x08,0x0E,0x00,0x00,0x00,0x01,0x01,	/* GLONASS disable */
		0x2D,0x59                                   /* checksum */
};	


static uint8_t setNMEAoff[28]	 = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00,0x00, 0x00, 0x00, 0x00,0x84, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,0x01, 0x00,0x01, 0x00,0x00, 0x00,0x00, 0x00,0xA0, 0x96};
static uint8_t setNMEAon[28]	   		= {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0xB5};
static uint8_t setNAVmode[44]			= {0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0xE8};
static uint8_t setGGArate_1s[16]		= {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28};
static uint8_t setGGArate_off[16]		= {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x23};
static uint8_t setGLLrate_off[16]		= {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A};
static uint8_t setGSArate_off[16]		= {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x31};
static uint8_t setGSVrate_off[16]		= {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x38};
static uint8_t setRMCrate_off[16]		= {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x3F};
static uint8_t setVTGrate_off[16]		= {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46};
static uint8_t restore_default_config[] = {0xB5,0x62,0x06,0x09,0x0D,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x03,0x1B,0x9A};
//static uint8_t set_continueous_mode[] = {0xB5 ,0x62 ,0x06 ,0x11 ,0x02 ,0x00 ,0x08 ,0x00 ,0x21 ,0x91 ,0xB5 ,0x62 ,0x06 ,0x09 ,0x0D ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xFF ,0xFF ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x07 ,0x21 ,0xAF};
//static uint8_t set_power_save_mode[] =  {0xB5 ,0x62 ,0x06 ,0x11 ,0x02 ,0x00 ,0x08 ,0x01 ,0x22 ,0x92 ,0xB5 ,0x62 ,0x06 ,0x09 ,0x0D ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xFF ,0xFF ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x07 ,0x21 ,0xAF};

static uint8_t set_continueous_mode[] = {0xB5 ,0x62 ,0x06 ,0x11 ,0x02 ,0x00 ,0x08 ,0x00 ,0x21 ,0x91};
static uint8_t set_power_save_mode[] =  {0xB5 ,0x62 ,0x06 ,0x11 ,0x02 ,0x00 ,0x08 ,0x01 ,0x22 ,0x92};

	/*
	REQUESTS.
	Response to these messages is message specific.
*/
	
/* Header for every UBX message */
static uint8_t UBX_header[2]	 = {0xB5, 0x62};


/* NAVIGATION ENGINE					response 44 bytes */
static uint8_t request0624[8]			= {0xB5, 0x62, 0x06, 0x24, 0x00, 0x00, 0x2A, 0x84};

/* POWER MODE settings					response 10 bytes */
static uint8_t request0611[8]			= {0xB5, 0x62, 0x06, 0x11, 0x00, 0x00, 0x17, 0x4B};
	
/* POWER MMANAGEMENT					response 56 bytes */
static uint8_t request063B[8]			= {0xB5, 0x62, 0x06, 0x3B, 0x00, 0x00, 0x41, 0xC9};
	
/* GNSS configuration					response 52 bytes */
static uint8_t request063E[8]			= {0xB5, 0x62, 0x06, 0x3E, 0x00, 0x00, 0x44, 0xD2};
	
/* POSITION DATA						response 36 bytes */
static uint8_t request0102[8]			= {0xB5, 0x62, 0x01, 0x02, 0x00, 0x00, 0x03, 0x0A};

/* FIX, SATS							response 60 bytes */
static uint8_t request0106[8]			= {0xB5, 0x62, 0x01, 0x06, 0x00, 0x00, 0x07, 0x16};

/* TIME, DATE							response 28 bytes */
static uint8_t request0121[8]			= {0xB5, 0x62, 0x01, 0x21, 0x00, 0x00, 0x22, 0x67};

/* EVERYTHING							response 100 bytes (UBLOX 8), 92 bytes (UBLOX 7) */
static uint8_t request0107[8]			= {0xB5, 0x62, 0x01, 0x07, 0x00, 0x00, 0x08, 0x19};

/* VERSION								response 48 + 30 * N bytes */
static uint8_t request0A04[8]			= {0xB5, 0x62, 0x0A, 0x04, 0x00, 0x00, 0x0E, 0x34};

/* HARDWARE RESET warm start			no response */
static uint8_t resetReceiver[12]		= {0xB5 ,0x62 ,0x06 ,0x04 ,0x04 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x0F ,0x68};

/* SAVE CURRENT CONFIGURATION CFG-CFG			no response */
static uint8_t saveConfiguration[21]	= {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1B, 0xA9};

/* GPGGA MAX-7C default					response ~75 bytes */
static uint8_t requestGPGGA[15]			= "$EIGPQ,GGA*27\r\n";

/* GPZDA MAX-7C default					response ~36 bytes */
static uint8_t requestGPZDA[15]			= "$EIGPQ,ZDA*39\r\n";

/* GNGGA MAX-M8Q default				response ~75 bytes */
static uint8_t requestGNGGA[15]			= "$EIGNQ,GGA*39\r\n";

/* GNZDA MAX-M8Q default				response ~36 bytes */
static uint8_t requestGNZDA[15]			= "$EIGNQ,ZDA*27\r\n";


/*
	POWER SAVING.
	Response to these UBX messages is ACK/NACK - 10 bytes.
*/

/* To wake up the module. Wait for 1.5s. */
static uint8_t dummyByte[4]								= {0xFF,0xFF,0xFF,0xFF};

/* Switches to the continuous mode. */
static uint8_t setContinuousMode[10]					= {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x00, 0x00, 0x19, 0x81};

/* Switches to the selected power saving configuration. */
static uint8_t setPowerSaveMode[10]						= {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x00, 0x01, 0x1A, 0x82};

/* CYCLIC 1s, doNotEnterOff, update EPH. */
static uint8_t setCyclicOperation_1s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x3C, 0x00, 0x00, 0x90, 0x03, 0x01, 0xE8, 0x03, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0xD3, 0xCC};

/* CYCLIC 2s, doNotEnterOff, update EPH. */
static uint8_t setCyclicOperation_2s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x3C, 0x00, 0x00, 0x90, 0x03, 0x01, 0xD0, 0x07, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0xBF, 0xF8};

/* CYCLIC 3s, doNotEnterOff, update EPH. */
static uint8_t setCyclicOperation_3s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x3C, 0x00, 0x00, 0x90, 0x03, 0x01, 0xB8, 0x0B, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0xAB, 0x24};

/* CYCLIC 4s, doNotEnterOff, update EPH. */
static uint8_t setCyclicOperation_4s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x3C, 0x00, 0x00, 0x90, 0x03, 0x01, 0xA0, 0x0F, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0x97, 0x50};

/* CYCLIC 5s, doNotEnterOff, update EPH. */
static uint8_t setCyclicOperation_5s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x3C, 0x00, 0x00, 0x90, 0x03, 0x01, 0x88, 0x13, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0x83, 0x7C};

/* CYCLIC 6s, doNotEnterOff, update EPH. */
static uint8_t setCyclicOperation_6s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x3C, 0x00, 0x00, 0x90, 0x03, 0x01, 0x70, 0x17, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0x6F, 0xA8};

/* CYCLIC 7s, doNotEnterOff, update EPH. */
static uint8_t setCyclicOperation_7s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x3C, 0x00, 0x00, 0x90, 0x03, 0x01, 0x58, 0x1B, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0x5B, 0xD4};

/* CYCLIC 8s, doNotEnterOff, update EPH. */
static uint8_t setCyclicOperation_8s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x3C, 0x00, 0x00, 0x90, 0x03, 0x01, 0x40, 0x1F, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0x47, 0x00};

/* CYCLIC 9s, doNotEnterOff, update EPH. */
static uint8_t setCyclicOperation_9s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x3C, 0x00, 0x00, 0x90, 0x03, 0x01, 0x28, 0x23, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0x33, 0x2C};

/* CYCLIC 10s, doNotEnterOff, update EPH. */
static uint8_t setCyclicOperation_10s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x3C, 0x00, 0x00, 0x90, 0x03, 0x01, 0x10, 0x27, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0x1F, 0x58};

/* ONOFF 10s, doNotEnterOff, updateEPH. */
static uint8_t setONOFFoperation_10s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 0x90, 0x01, 0x01, 0x10, 0x27, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0xE1, 0x34};

/* ONOFF 20s, doNotEnterOff, updateEPH. */
static uint8_t setONOFFoperation_20s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 0x90, 0x01, 0x01, 0x20, 0x4E, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0x18, 0xC9};

/* ONOFF 30s, doNotEnterOff, updateEPH. */
static uint8_t setONOFFoperation_30s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 0x90, 0x01, 0x01, 0x30, 0x75, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0x4F, 0x5E};

/* ONOFF 60s, doNotEnterOff, updateEPH. */
static uint8_t setONOFFoperation_60s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 0x90, 0x01, 0x01, 0x60, 0xEA, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0xF4, 0x1D};
	
/* ONOFF 120s, doNotEnterOff, updateEPH. */
static uint8_t setONOFFoperation_120s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 0x90, 0x01, 0x01, 0xC0, 0xD4, 0x01, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0x3F, 0xBD};

/* ONOFF update 0s, onTime 3s, doNotEnterOff. */
static uint8_t setONOFFoperation_perm[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 0x80, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0x9D, 0x77};

/* ONOFF update 0s, doNotEnterOff, updateEPH. */
static uint8_t setONOFFoperation_permE[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 0x90, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0xAA, 0x9F};

/* ONOFF update 0s, doNotEnterOff. */
static uint8_t setONOFFoperation_permR[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 0x80, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0x9A, 0x2F};

/* ONOFF update 10s, onTime 3s, doNotEnterOff. */
static uint8_t setONOFFoperation_10s3s[52]				= {0xB5, 0x62, 0x06, 0x3B, 0x2C, 0x00, 0x01, 0x06, 0x00, 0x00, 0x00, 0x80, 0x01, 0x01, 0x10, 0x27, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x4F, 0xC1, 0x03, 0x00, 0x87, 0x02, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x64, 0x40, 0x01, 0x00, 0xD4, 0x0C};

/* Backup mode for infinite duration. */
static uint8_t setBackupMode[16]						= {0xB5, 0x62, 0x02, 0x41, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x4D, 0x3B};

/* Software backup mode. DEPRECIATED! */
static uint8_t setSwBackupMode[]						= {0xB5, 0x62, 0x06, 0x57, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0x50, 0x4B, 0x43, 0x42, 0x86, 0x46};

/* Setup powersave config to use extint0 pin to turn on and turn off GPS */
//static uint8_t powersave_config[]= 
//	
//{
//	0xB5 ,0x62 ,0x06 ,0x3B ,0x30 ,0x00  /* UBX-CFG-PM2 */
//	,0x02 ,0x06 ,0x00 ,0x00 /* v2, reserved 1..3 */
//	,0x60 ,0x90 ,0x42 ,0x01 /* Cyclic tracking mode, update ephemeris */
//	,0xE8 ,0x03 ,0x00 ,0x00 /* update period, 1000 ms, */
//	,0x10 ,0x27 ,0x00 ,0x00 /* search period, 10000 ms */
//	,0x00 ,0x00 ,0x00 ,0x00 /* grid offset */
//	,0x02 ,0x00             /* on-time after first fix */
//	,0x00 ,0x00             /* minimum acquisition time */
//	,0x2C ,0x01 ,0x00 ,0x00 
//	,0x4F ,0xC1 ,0x03 ,0x00 
//	,0x87 ,0x02 ,0x00 ,0x00
//	,0xFF ,0x00 ,0x00 ,0x00 
//	,0x64 ,0x40 ,0x01 ,0x00 
//  ,0x00 ,0x00 ,0x00 ,0x00 
//  ,0x3D ,0x56};


	
static uint8_t powersave_config[]= 
{
	0xB5 ,0x62 ,0x06 ,0x3B ,0x30 ,0x00 /* UBX-CFG-PM2 */
	,0x02 ,0x06 ,0x00 ,0x00 /* v2, reserved 1..3 */
	,0x60 ,0x90 ,0x40 ,0x01 /* ON/OFF tracking mode, update ephemeris */
	,0x00 ,0x00 ,0x00 ,0x00 /* update period, 0 ms(infinity) */
	,0x00 ,0x00 ,0x00 ,0x00 /* search period, 0 ms(infinity) */
	,0x00 ,0x00 ,0x00 ,0x00 /* grid offset */
	,0x02 ,0x00             /* on-time after first fix */
	,0x00 ,0x00             /* minimum acquisition time */
	,0x2C ,0x01 ,0x00 ,0x00
	,0x4F ,0xC1 ,0x03 ,0x00
	,0x87 ,0x02 ,0x00 ,0x00
	,0xFF ,0x00 ,0x00 ,0x00
	,0x64 ,0x40 ,0x01 ,0x00
	,0x00 ,0x00 ,0x00 ,0x00
	,0x19 ,0xB8

};
	

/* GNSS running. */
static uint8_t setGNSSrunning[16]						= {0xB5, 0x62, 0x06, 0x57, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x4E, 0x55, 0x52, 0x7B, 0xC3};

/* GNSS stopped. */
static uint8_t setGNSSstopped[16]						= {0xB5, 0x62, 0x06, 0x57, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00, 0x50, 0x4F, 0x54, 0x53, 0xAC, 0x85};


// VARIABLES
static  uint8_t GPSbuffer[GPSBUFFER_SIZE];

volatile static  uint8_t GPS_UBX_ack_error = 0;
volatile static  uint8_t GPS_UBX_checksum_error = 0;
volatile static  uint8_t GPS_UBX_buffer_error = 0;
static uint8_t parse_success;

static uint8_t GPS_NMEA_checksum_toverify = 0;
static uint8_t GPS_NMEA_checksum_calculated = 0;

static uint8_t GPS_NMEA_error_bitfield;							// default no ERRORs 0b00100000
extern uint8_t GPS_UBX_error_bitfield;							// default no ERRORs 0b00100000, global for APRS use

extern uint16_t GPS_NMEA_latitude_int;							// YYYY, +
extern uint32_t GPS_NMEA_latitude_dec;							// YYYYY, +
extern uint16_t GPS_NMEA_longitude_int;							// XXXXX, +
extern uint32_t GPS_NMEA_longitude_dec;							// XXXXX, +
extern uint8_t GPS_NMEA_NS;										// SOUTH 0	NORTH 1
extern uint8_t GPS_NMEA_EW;										// WEST 0	EAST 1

extern int32_t GPS_UBX_latitude;								// YYYYYYYYY, +/-
extern int32_t GPS_UBX_longitude;								// XXXXXXXXXX, +/-
extern float GPS_UBX_latitude_Float;							// YY.YYYYYYY, in +/- DEGREES (?), also used as output of Coords_DEGtoDEC()
extern float GPS_UBX_longitude_Float;							// XXX.XXXXXXX, in +/- DEGREES (?), also used as output of Coords_DEGtoDEC()

extern int32_t GPSaltitude;										// for both UBX and NMEA, in +/- meters (m)

extern uint8_t GPShour;											// for both UBX and NMEA
extern uint8_t GPSminute;										// for both UBX and NMEA
extern uint8_t GPSsecond;										// for both UBX and NMEA
extern uint8_t GPSday;											// for both UBX and NMEA
extern uint8_t GPSmonth;										// for both UBX and NMEA
extern uint16_t GPSyear;										// for both UBX and NMEA


extern uint8_t GPSsats;											// for both UBX and NMEA
extern uint8_t GPSfix_type;											// for both UBX and NMEA
extern uint8_t GPSfix_OK;										// only UBX source
extern uint8_t GPSvalidity;										// only UBX source

extern uint8_t GPSnavigation;									// only UBX source
extern uint8_t GPSpowermode;									// only UBX source
extern uint8_t GPSpowersavemodestate;							// only UBX source

extern int32_t GPSgroundspeed;									// only UBX source, mm/s
extern int32_t GPSheading;										// only UBX source, degrees (1e-5)

extern uint16_t AD3data;										// raw ADC reading
extern uint16_t AD9data;										// raw ADC reading
extern uint16_t AD15data;										// raw ADC reading
extern uint16_t Si4060Temp;										// raw Si4060's ADC reading of temperature
extern uint32_t telemCount;										// current telemetry ID
extern uint32_t telemetry_len;									// length of the telemetry string

extern int32_t GPS_UBX_latitude_L;								// LAST valid value (in case of lost FIX)
extern int32_t GPS_UBX_longitude_L;								// LAST valid value (in case of lost FIX)
extern int32_t GPSaltitude_L;									// LAST valid value (in case of lost FIX)
extern uint8_t GPS_NMEA_NS_L;									// LAST valid value (in case of lost FIX)
extern uint8_t GPS_NMEA_EW_L;									// LAST valid value (in case of lost FIX)
extern uint16_t GPS_NMEA_latitude_int_L;						// LAST valid value (in case of lost FIX)
extern uint32_t GPS_NMEA_latitude_dec_L;						// LAST valid value (in case of lost FIX)
extern uint16_t GPS_NMEA_longitude_int_L;						// LAST valid value (in case of lost FIX)
extern uint32_t GPS_NMEA_longitude_dec_L;						// LAST valid value (in case of lost FIX)

extern uint8_t ack; // 1 is ack, 0 is nak

extern uint32_t fixAttemptCount;

/* I2C related*/
extern I2C_HandleTypeDef hi2c1;
extern HAL_StatusTypeDef i2c_status;



// FUNCTIONS

void Coords_DEGtoDEC(uint32_t lat_INT, uint32_t lat_DEC, uint32_t lon_INT, uint32_t lon_DEC, uint8_t latNS, uint8_t lonEW);

uint8_t UBLOX_verify_checksum(volatile uint8_t *buffer, uint8_t len);
uint8_t UBLOX_send_message(uint8_t *message, uint8_t len);
uint8_t UBLOX_request_UBX(uint8_t *request, uint8_t len, uint8_t expectlen, uint8_t (*parse)(volatile uint8_t*));
uint8_t UBLOX_receive_UBX(uint8_t *buffer, uint8_t len, uint32_t timeout);
uint8_t UBLOX_parse_0102(volatile uint8_t *buffer);
uint8_t UBLOX_parse_0121(volatile uint8_t *buffer);
uint8_t UBLOX_parse_0106(volatile uint8_t *buffer);
uint8_t UBLOX_parse_0624(volatile uint8_t *buffer);
uint8_t UBLOX_parse_0611(volatile uint8_t *buffer);
uint8_t UBLOX_parse_0107(volatile uint8_t *buffer);
uint8_t UBLOX_parse_ACK(volatile uint8_t *buffer);
uint8_t UBLOX_parse_empty(void);
void UBLOX_powersave_mode_init(uint8_t * mode);
uint32_t UBLOX_get_version(uint8_t *buffer);
uint8_t setup_GPS(void);
uint8_t Backup_GPS(void);
uint8_t Wakeup_GPS(void);
uint8_t get_location_fix(void);

#endif // UBLOX_H
