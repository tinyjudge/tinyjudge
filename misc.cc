#include "misc.h"

char* itoa(int value, char *str, int base) {
	// positive numbers only
	char *strbackup = str;
	do {
		int cur = value % base;
		if(cur < 10)
			*str = cur + '0';
		else
			*str = cur - 10 + 'a';
		str++; value /= base;
	} while(value != 0);
	*str = '\0';
	char *f;
	for(f = strbackup, str--; f < str; f++, str--) {
		char t = *f;
		*f = *str;
		*str = t;
	}
	return strbackup;
}
