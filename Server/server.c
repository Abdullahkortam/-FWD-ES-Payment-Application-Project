#include "server.h"
#include <string.h>

#define DBMAXSIZE 255

uint32_t key = 0;
uint32_t flag = 0;
uint32_t sequence = 0;

ST_accountsDB_t accountsDB[DBMAXSIZE]={
		{10000,	5105105105105100},
		{3320.5,4012888888881881},
		{100,3530111333300000},
		{100500,3566002020360505},
		{800,4012888888881881}
};


EN_serverError_t isValidAccount(ST_cardData_t *cardData)
{
	uint8_t isValidName;
	uint8_t isValidPAN;
	uint8_t isValidDate;

	isValidName = getCardHolderName(cardData);
	while(isValidName == WRONG_NAME)
		isValidName = getCardHolderName(cardData);

	isValidPAN = getCardPAN(cardData);
	while(isValidPAN == WRONG_PAN)
		isValidPAN = getCardPAN(cardData);

	isValidDate = getCardExpiryDate(cardData);
	while(isValidDate == WRONG_EXP_DATE)
		isValidDate = getCardExpiryDate(cardData);

	if(isValidName==OK &&  isValidPAN == OK && isValidDate==OK)
	{
		uint32_t i ;
		for(i=0;i<DBMAXSIZE;i++)
		{
			if(strcmp(cardData->primaryAccountNumber,accountsDB[i].primaryAccountNumber)==0)
			{
				flag= i ;
				return OK;
			}
		}
		flag =255 ;
	}
	return ACCOUNT_NOT_FOUND;
}

EN_transState_t recieveTransactionData(ST_transaction_t *transData)
{
	uint8_t isVcard ;
	uint8_t isVamount ;
	uint8_t isVsaved ;

	isVcard = isValidAccount(&(transData->cardHolderData));
	isVamount = isAmountAvailable(&(transData->terminalData));

	if (isVcard == ACCOUNT_NOT_FOUND)
	{
		transData->transState = DECLINED_STOLEN_CARD;
	}
	else if (isVamount == LOW_BALANCE)
	{
		transData->transState = DECLINED_INSUFFECIENT_FUND;
	}
	else
	{
		transData->transState = APPROVED;
	}

	if (isCardExpired(transData->cardHolderData, transData->terminalData) == EXPIRED_CARD)
	{
		printf("Expired Card");
		return DECLINED_STOLEN_CARD;
	}

	transData->transactionSequenceNumber = sequence;
	isVsaved = saveTransaction(transData);

	if ((isVcard == ACCOUNT_NOT_FOUND) || flag == 255)
	{
		printf("Stolen Card\n");

		return DECLINED_STOLEN_CARD;
	}
	else if ((isVcard == OK) && (flag != 255))
	{
		if (isVamount == LOW_BALANCE)
		{
			printf("Low balance\n");
			return DECLINED_INSUFFECIENT_FUND;
		}
		else if (isVsaved == SAVING_FAILED)
		{
			printf("Internal Server Error\n");
			return INTERNAL_SERVER_ERROR;
		}
		else if ((isBelowMaxAmount(&(transData->terminalData)) == OK) && (isVcard == OK))
		{

			accountsDB[flag].balance -= transData->terminalData.transAmount;
			printf("Succeeded\n");
		}
	}
	return APPROVED;
}

ST_transaction_t transactionsDB[DBMAXSIZE];
void fillTransactionDB(ST_transaction_t *transactionDB)
{
	uint32_t i ;
	for(i=0;i<DBMAXSIZE;i++)
	{
		strcpy(transactionDB[i].cardHolderData.cardHolderName,"00000");
		strcpy(transactionDB[i].cardHolderData.cardExpirationDate,"00000");
		strcpy(transactionDB[i].cardHolderData.primaryAccountNumber,"00000");

		transactionDB[i].terminalData.maxTransAmount =0 ;
		transactionDB[i].terminalData.transAmount= 0 ;
		strcpy(transactionDB[i].terminalData.transactionDate,"000000");

		transactionDB[i].transState = 0 ;
		transactionDB[i].transactionSequenceNumber = 0 ;
	}
}

EN_serverError_t isAmountAvailable(ST_terminalData_t* termData)
{
    uint8_t isValidAmount;

    isValidAmount = getTransactionAmount(termData);

    if (isValidAmount == OK)
    {
        if (termData->transAmount > accountsDB[flag].balance)
        {
            return LOW_BALANCE;
        }

        return OK;
    }
}

EN_serverError_t saveTransaction(ST_transaction_t* transData)
{
    if (sequence > 255)
    {
        return SAVING_FAILED;
    }

    strcpy(transactionsDB[sequence].cardHolderData.cardExpirationDate, transData->cardHolderData.cardExpirationDate);
    strcpy(transactionsDB[sequence].cardHolderData.cardHolderName, transData->cardHolderData.cardHolderName);
    strcpy(transactionsDB[sequence].cardHolderData.primaryAccountNumber, transData->cardHolderData.primaryAccountNumber);

    strcpy(transactionsDB[sequence].terminalData.transactionDate, transData->terminalData.transactionDate);

    transactionsDB[sequence].terminalData.maxTransAmount = transData->terminalData.maxTransAmount;
    transactionsDB[sequence].terminalData.transAmount = transData->terminalData.transAmount;

    transactionsDB[sequence].transactionSequenceNumber = transData->transactionSequenceNumber;
    transactionsDB[sequence].transState = transData->transState;

    sequence++;

    return OK;
}


EN_serverError_t getTransaction(uint32_t transactionSequenceNumber, ST_transaction_t* transData)
{
    uint32_t i = 0;
    while (transData[i].transactionSequenceNumber != 0)
    {
        if (transData[i].transactionSequenceNumber == transactionSequenceNumber)
        {
            puts("-------------------------");
            puts("Transaction Details:");
            printf("Card Holder Name          :\t\t%s\n", transData[i].cardHolderData.cardHolderName);
            printf("Card PAN                  :\t\t%s\n", transData[i].cardHolderData.primaryAccountNumber);
            printf("Card Expiration Date      :\t\t%s\n", transData[i].cardHolderData.cardExpirationDate);

            printf("Transaction Date          :\t\t%s\n", transData[i].terminalData.transactionDate);
            printf("Transaction Amount        :\t\t%f\n", transData[i].terminalData.transAmount);
            printf("Transaction Maximum Amount:\t\t%f\n", transData[i].terminalData.maxTransAmount);

            printf("Sequence Number           :\t\t%d\n", transData[i].transactionSequenceNumber);
            printf("Transaction State         :\t\t%d\n", transData[i].transState);

            return OK;
        }
        i++;
    }
    puts("Transaction Not Found");
    return TRANSACTION_NOT_FOUND;
}
