#include <time.h>
#include <stdio.h>
#include "terminal.h"

EN_terminalError_t getTransactionAmount(ST_terminalData_t *termData)
{
	printf("Please Enter Transaction Amount : ");
	fflush(stdin);fflush(stdout);
	scanf("%f",&termData->transAmount);

	if(termData->transAmount <=0)
	{
		return INVALID_AMOUNT;
	}
	return OK;
}

EN_terminalError_t isCardExpired(ST_cardData_t cardData, ST_terminalData_t termData)
{
	int expireDateMounth = (int) cardData.cardExpirationDate[0]*10 + (int)cardData.cardExpirationDate[1] ;
	int expireDateYear   = (int) cardData.cardExpirationDate[3]*10 + (int)cardData.cardExpirationDate[4] ;

	int TerminalDateMounth =(int) termData.transactionDate[3]*10 +(int) termData.transactionDate[4] ;
	int TerminalDateYear =(int) termData.transactionDate[8]*10 +(int) termData.transactionDate[9] ;

	if(expireDateYear <=TerminalDateYear)
	{
		if(expireDateMounth < TerminalDateMounth)
			return EXPIRED_CARD;
	}
	return OK ;
}

EN_terminalError_t isBelowMaxAmount(ST_terminalData_t *termData)
{
	if(termData->transAmount > termData->maxTransAmount)
	{
		printf("Exceeded maximum amount\n");
		return EXCEED_MAX_AMOUNT;
	}
	return OK ;
}

EN_terminalError_t setMaxAmount(ST_terminalData_t *termData,flaot num);
{
	if(num<=0)
	{
		return INVALID_MAX_AMOUNT ;
	}
	termData->maxTransAmount = num ;
	return OK ;
}

EN_terminalError_t getTransactionDate(ST_terminalData_t *termData)
{
	time_t rawtime;
	struct tm* date;
	time(&rawtime);
	date = localtime(&rawtime);
	sprintf(termData->transactionDate, "%02d/%02d/%04d", date->tm_mday, date->tm_mon + 1, date->tm_year + 1900);
	return OK ;
}
