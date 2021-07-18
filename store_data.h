#ifndef __store_data_module__
#define __store_data_module__

// ********************************************************
// **   Store Default specifications for 'Default Mode'  **
// ********************************************************

#define DEFAULT_MAX_TEMP  50							// Default Max Temperature   - 50ºC
#define DEFAULT_MIN_HUMID 40							// Default Min Soil Humidity - 40%
#define DEFAULT_MAX_HUMID 70 							// Default Max Soil Humidity - 70%
#define HUMID_TOLERANCE   5 	                                                // Default Humidity Tolerance
#define TEMP_TOLERANCE    2 							// Default Temperature Tolerance

// ********************************************************
// **    Store User specifications for 'Custom Mode'     **
// ********************************************************

typedef struct Sensors_Info
{										
	unsigned char max_temp;					
	unsigned char min_humid;
	unsigned char max_humid;
	unsigned char actual_temp;
	unsigned char actual_humid;
} sensors_info_t;


// ********************************************************
// **     Store User specifications for 'Timer Mode'     **
// ********************************************************

typedef struct Timer_Info				
{
	unsigned char hour;
	unsigned char minutes;
	unsigned char duration;
} timer_info_t;

#endif