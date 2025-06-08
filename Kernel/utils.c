#include "utils.h"

int strcmp(const char *str1, const char *str2){
	int i = 0;
	int ret = 0;

	while (str1[i] != 0 && str2[i] != 0)
	{
		if (str1[i] - str2[i] != 0)
		{
			return str1[i] - str2[i];
		}
		i++;
	}

	if (str1[i] != 0 || str2[i] != 0)
	{
		return str1[i] - str2[i];
	}

	return ret;
}

int strlen(const char *str){
	int i = 0;
	while (str[i] != 0)
	{
		i++;
	}

	return i;
}

char *strcpy(char *dest, const char *src, uint64_t size){
	int i = 0;
	while (src[i] != 0){
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;
	return dest;
}

char *strcpyForParam(char *dest, const char *src, const char *src2){
	int i = 0, k = 0;
	while (src[i] != 0)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = ' ';
	i++;
	while (src2[k] != 0)
	{
		dest[i] = src2[k];
		i++;
		k++;
	}
	dest[i] = 0;
	return dest;
}

void intToStr(int value, char * str){
	int i = 0;
	int isNegative = 0;

	if (value == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return;
	}
	if (value < 0) {
		isNegative = 1;
		value = -value;	
	}
	while (value > 0) {
		str[i++] = (value % 10) + '0';
		value /= 10;
	}
	if (isNegative) {
		str[i++] = '-';	
	}
	str[i] = '\0';
	for(int j = 0; j < i / 2; j++) {
		char temp = str[j];
		str[j] = str[i - j - 1];
		str[i - j - 1] = temp;
	}
}

int atoi(const char *str){
	int result = 0;
	int sign = 1;
	int i = 0;

	while(str[i] == ' '){
		i++;
	}

	//primero el signo
	if(str[i] == '-' || str[i] == '+'){
		sign = (str[i] == '-') ? -1 : 1;
	}

	//char a int
	while(str[i] >= '0' && str[i] <= '9'){
		result = result * 10 + (str[i++] - '0');
	}

	return (sign * result);
}