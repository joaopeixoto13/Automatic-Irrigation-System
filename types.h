#ifndef __types__
#define __types__

typedef union break_integer16{
	int value;
	char bits[2];
}break_integer16_t;

typedef union break_integer32{
	long int window32;
	char array[4];
}break_integer32_t;

#endif