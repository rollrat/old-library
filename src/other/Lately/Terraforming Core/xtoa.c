/*************************************************************************
 *
 * FILE NAME : xtoa.h - x to string
 *
 * RollRat ISO Standard C Library
 *
 *
 * CREATION : 2014.12.06
 * MADEBY   : Jeong HyunJun(Nickname : rollrat)
 * E-MAIL   : rollrat@naver.com
 *
 * (C) Copyright 2014 .rollrat. All Rights Reserved
 *
 **************************************************************************/

#include "string.h"
#include <math.h>

char *__itoa(
	int i
	)
{
	static char buf[21];
	char *p = buf + 19 + 1;
	if (i >= 0) {
		while (i != 0) { 
			*--p = '0' + (i % 10); 
			i /= 10; 
		}
		return p;
	} else {
		while (i != 0) { 
			*--p = '0' - (i % 10); 
			i /= 10; 
		}
		*--p = '-';
	}
	return p;
}

static double PRECISION = 0.00000000000001;
static int MAX_NUMBER_STRING_SIZE = 32;

// non std
// stackoverflow.com/questions/2302969/how-to-implement-char-
// ftoafloat-num-without-sprintf-library-function-i
char * __dtoa(
	double n
	)
{
    // handle special cases
	static char s[100];
    int digit, m, m1;
    char *c = s;
    int neg = (n < 0);
    if (neg)
        n = -n;
    // calculate magnitude
    m = log10(n);
    int useExp = (m >= 14 || (neg && m >= 9) || m <= -9);
    if (neg)
        *(c++) = '-';
    // set up for scientific notation
    if (useExp) {
        if (m < 0)
           m -= 1.0;
        n = n / pow(10.0, m);
        m1 = m;
        m = 0;
    }
    if (m < 1.0) {
        m = 0;
    }
    // convert the number
    while (n > PRECISION || m >= 0) {
        double weight = pow(10.0, m);
        if (weight > 0 && !isinf(weight)) {
            digit = floor(n / weight);
            n -= (digit * weight);
            *(c++) = '0' + digit;
        }
        if (m == 0 && n > 0)
            *(c++) = '.';
        m--;
    }
    if (useExp) {
        // convert the exponent
        int i, j;
        *(c++) = 'e';
        if (m1 > 0) {
            *(c++) = '+';
        } else {
            *(c++) = '-';
            m1 = -m1;
        }
        m = 0;
        while (m1 > 0) {
            *(c++) = '0' + m1 % 10;
            m1 /= 10;
            m++;
        }
        c -= m;
        for (i = 0, j = m-1; i<j; i++, j--) {
            // swap without temporary
            c[i] ^= c[j];
            c[j] ^= c[i];
            c[i] ^= c[j];
        }
        c += m;
    }
    *(c) = '\0';
    return s;
}