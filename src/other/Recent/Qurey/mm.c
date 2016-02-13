/*************************************************************************
  
   Copyright (C) 2015. rollrat. All Rights Reserved.

------
FILE NAME:
   
   mm.c - [Private]
   
Abstract:

   Source file load and save.
   Data Base Management System in dos mode.

------
   AUTHOR: HyunJun Jeong  2015-01-21

***************************************************************************/

#include <stdio.h>
#include <string.h>
#include "type.h"
#include "qyt.h"
#include <malloc.h>
#include <time.h>

#define GetByte(xt) (BYTE)(*xt)
#define GetWord(xt) (WORD)(*xt << 8 | *(xt + 1))
#define GetDWord(xt) (DWORD)((DWORD)(((WORD)(*xt << 8 | *(xt + 1))) << 16) | \
	(DWORD)(((WORD)(*(xt + 2) << 8 | *(xt + 3)))))

#define LEndian4(tx) ((tx & 0xff) << 8) | ((tx & 0xff00) >> 8)
#define LEndian8(tx) ((tx & 0xff) << 24) | ((tx & 0xff00) << 8) \
	| ((tx & 0xff0000) >> 8) | ((tx & 0xff000000) >> 24)

int main(int argc, char *argv[])
{
#ifndef _DEBUG
	FILE *fp;
	int opSL;

	if (argc < 3) {
		return 0;
	}
	
	// 오류 확인용
	fp = NULL;

	// 임시 땜빵이므로 수정금지
	if (!strcmp(argv[1], "/save")) {
		fopen_s(&fp, argv[2], "w");
		opSL = 0;
	} else if (!strcmp(argv[1], "/load")) {
		fopen_s(&fp, argv[2], "r");
		opSL = 1;
	}

	// 열기 오류
	if(fp == NULL) {
		printf("Cannot find source file!");
		return 0;
	}

	switch(opSL) {
	case 0:	{
		}
		break;
	case 1:  {
			PQUREY_ENTRY qe;
			PQUREY_CLUSTER qc;
			LdrEntry(fp, &qe);
			LdrCluster(fp, qe->ClusterStatementAddr, &qc);
		}
		break;
	}
	fclose(fp);

#else
	FILE *fpr, *fpw;
	PQUREY_ENTRY qe;
	PQUREY_CLUSTER qc;
	PQUREY_TABLE qt, qt1;
	QUREY_SECURITY qs;
	PTABLE_LIST tl, ltl;

	fopen_s(&fpw, "C:\\rollrat\\test1.rqy", "w");
	if (!fpw) {
		printf("error!");
	}
	//fopen_s(&fpr, "C:\\rollrat\\test1.rqy", "r");

// Write Test
	InitQureyEntry(&qe, "RollRat");
	SvrEntry(fpw, qe);

	InitQureyCluster(&qc, 1, "cluster");
	qc->NumberOfTable = 2;
	SvrCluster(fpw, qe->ClusterStatementAddr, qc);

	InitTable(&qt, 1, "table1");
	InitTable(&qt1, 2, "table2");
	TListInit(&tl, qt);
	ltl = tl;
	ltl = ltl->next;
	TListPushBack(ltl, qt1);
	qt1->HeadAddrOfSection = qt->HeadAddrOfSection + sizeof(QUREY_TABLE);
	SvrTable(fpw, qc->HeadAddrOfTable, tl);

// Read Test
	/*qe = (QUREY_ENTRY *)malloc(sizeof(QUREY_ENTRY));
	LdrEntry(fpr, &qe);
	qc = (QUREY_CLUSTER *)malloc(sizeof(QUREY_CLUSTER));
	LdrCluster(fpr, qe->ClusterStatementAddr, &qc);*/


	//fclose(fpr);
	fclose(fpw);
#endif


	return 0;
}

/***
 *  - Split: Returns a string array that contains the substrings in this
 *		string that are delimited by elements of a specified String array.
 */
char **Split(const char *_Ptr, const char *_T)
{
	int ic = strlen(_T) + 1;
	char **_ret = (char **)malloc(sizeof(char *)*ic), *s1, *s2, *tmp;
	int i = 0, t = strlen(_T), f;
	char **_ret_;
	
	for (s1 = _Ptr; (s2 = strstr(s1, _T)) != NULL; i++) {
		tmp = (char *)malloc(sizeof(char)*(s2-s1+1));
		memcpy(tmp, s1, s2 - s1);
		tmp[s2 - s1] = 0;
		_ret[i] = tmp;
		s1 += s2 - s1 + t;
	}
	
	if(*s1) {
		for (s2 = s1; *s2; s2++)
			;
		tmp = (char *)malloc(sizeof(char)*(s2-s1+1));
		memcpy(tmp, s1, s2 - s1);
		tmp[s2 - s1] = 0;
		_ret[i] = tmp;
		i++;
	}

	_ret_ = (char **)malloc(sizeof(char *)*i);
	for (f = 0; f < i; f++) {
		_ret_[f] = _ret[i];
	}
	return _ret;
}