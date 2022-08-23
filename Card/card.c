#include "card.h"
#include <string.h>

EN_cardError_t getCardHolderName(ST_cardData_t *cardData)
{
	char name[100];
	printf("Please Enter Card Holder Name : ");
	gets(name);
	if(name=='\0' || strlen(name)<20)
	{
		return WRONG_NAME ;
	}
	strcpy(cardData->cardHolderName,name);
	cardData->cardHolderName[24] = '\0';
	return OK;
}

EN_cardError_t getCardPAN(ST_cardData_t *cardData)
{
	char num[20];
	printf("Please Enter Account number : ");
	gets(num);
	if(num == '\0' || strlen(num)<16 || strlen(num) > 19)
		return WRONG_PAN ;
	else
	{
		strcpy(cardData->primaryAccountNumber,num);
		cardData->cardHolderName[19] = '\0';
		return OK;
	}
}

EN_cardError_t getCardExpiryDate(ST_cardData_t *cardData)
{
	char exp[10];
	printf("Please enter card Expire Date (MM/YY) : ");
	gets(exp);
	if(exp == '\0' || strlen(exp) < 5 || strlen(exp) > 5 || exp[2] != '/' || (exp[0] + exp[1]) < 97 ||(exp[0] + exp[1]) > 105 || (exp[0] =='0'&& exp[1]>'9' )||( exp[0]=='1' && exp[1] >'2'))
	{
			return WRONG_EXP_DATE ;
	}
	else
	{
		strcpy(cardData->cardExpirationDate,exp);
		cardData->cardExpirationDate[5] = '\0';
		return OK;
	}

}
