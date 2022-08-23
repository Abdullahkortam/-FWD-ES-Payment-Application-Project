#include <stdio.h>
#include <stdlib.h>
#include "Card/card.h"
#include "Terminal/terminal.h"
#include "Server/server.h"
#include "Application/app.h"
#include "platform_type.h"

extern ST_transaction_t transactionsDB[DBMAXSIZE];

int main()
{
    fillTransactionDB(transactionDB);
    startup();
    return 0;
}
