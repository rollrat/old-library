/*************************************************************************
 *
 * FILE NAME : strcpy.h - String Copy
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

char * _strcpy_asc(
	char * dest,
	const char * src
	)
{
	char * dest_ptr = dest;

	while( *dest_ptr++ = *src++ )
		;

	return dest;
}

_wchar_t * _strcpy_w(
	_wchar_t * dest,
	const _wchar_t * src
	)
{
	_wchar_t * dest_ptr = dest;

	while( *dest_ptr++ = *src++ )
		;

	return dest;
}