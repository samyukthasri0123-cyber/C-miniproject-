#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clientData structure definition
struct clientData {
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void searchByName(FILE *fPtr);
void transactionHistory(unsigned int account);
void commandMode(FILE *fPtr);

int main(int argc, char *argv[]) {
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL) {
        printf("%s: File could not be opened.\n", argv[0]);
        exit(-1);
    }

    while ((choice = enterChoice()) != 9) {
        switch (choice) {
        case 1: textFile(cfPtr); break;
        case 2: updateRecord(cfPtr); break;
        case 3: newRecord(cfPtr); break;
        case 4: deleteRecord(cfPtr); break;
        case 5: searchByName(cfPtr); break;
        case 6: {
            unsigned int acct;
            printf("Enter account number: ");
            scanf("%u", &acct);
            transactionHistory(acct);
            break;
        }
        case 7: commandMode(cfPtr); break;
        default: puts("Incorrect choice"); break;
        }
    }

    fclose(cfPtr);
}

// create formatted text file for printing
void textFile(FILE *readPtr) {
    FILE *writePtr;
    int result;
    struct clientData client = {0, "", "", 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL) {
        puts("File could not be opened.");
    } else {
        rewind(readPtr);
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

        while (!feof(readPtr)) {
            result = fread(&client, sizeof(struct clientData), 1, readPtr);
            if (result != 0 && client.acctNum != 0) {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n",
                        client.acctNum, client.lastName, client.firstName, client.balance);
            }
        }
        fclose(writePtr);
    }
}

// update balance in record
void updateRecord(FILE *fPtr) {
    unsigned int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account to update (1 - 100): ");
    scanf("%d", &account);

    fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account #%d has no information.\n", account);
    } else {
        printf("%-6d%-16s%-11s%10.2f\n\n",
               client.acctNum, client.lastName, client.firstName, client.balance);

        printf("Enter charge (+) or payment (-): ");
        scanf("%lf", &transaction);
        client.balance += transaction;

        printf("%-6d%-16s%-11s%10.2f\n",
               client.acctNum, client.lastName, client.firstName, client.balance);

        fseek(fPtr, -sizeof(struct clientData), SEEK_CUR);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);

        FILE *logPtr = fopen("transactions.txt", "a");
        if (logPtr != NULL) {
            fprintf(logPtr, "Acct %d: Transaction %.2f, New Balance %.2f\n",
                    client.acctNum, transaction, client.balance);
            fclose(logPtr);
        }
    }
}

// delete an existing record
void deleteRecord(FILE *fPtr) {
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0};
    unsigned int accountNum;

    printf("Enter account number to delete (1 - 100): ");
    scanf("%d", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum == 0) {
        printf("Account %d does not exist.\n", accountNum);
    } else {
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
    }
}

// create and insert record
void newRecord(FILE *fPtr) {
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter new account number (1 - 100): ");
    scanf("%d", &accountNum);

    fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
    fread(&client, sizeof(struct clientData), 1, fPtr);

    if (client.acctNum != 0) {
        printf("Account #%d already contains information.\n", client.acctNum);
    } else {
        printf("Enter lastname, firstname, balance\n? ");
        scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance);

        client.acctNum = accountNum;
        fseek(fPtr, (client.acctNum - 1) * sizeof(struct clientData), SEEK_SET);
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    }
}

// search by name
void searchByName(FILE *fPtr) {
    char name[15];
    struct clientData client = {0, "", "", 0.0};
    int found = 0;

    printf("Enter last name to search: ");
    scanf("%14s", name);

    rewind(fPtr);
    while (fread(&client, sizeof(struct clientData), 1, fPtr)) {
        if (client.acctNum != 0 && strcmp(client.lastName, name) == 0) {
            printf("%-6d%-16s%-11s%10.2f\n",
                   client.acctNum, client.lastName, client.firstName, client.balance);
            found = 1;
        }
    }
    if (!found) {
        printf("No account found with last name %s\n", name);
    }
}

// view transaction history
void transactionHistory(unsigned int account) {
    FILE *logPtr = fopen("transactions.txt", "r");
    char line[200];
    if (logPtr == NULL) {
        puts("No transaction history available.");
        return;
    }
    printf("Transaction history for account %d:\n", account);
    while (fgets(line, sizeof(line), logPtr)) {
        char acctStr[20];
        sprintf(acctStr, "Acct %d", account);
        if (strstr(line, acctStr)) {
            printf("%s", line);
        }
    }
    fclose(logPtr);
}

// command mode
void commandMode(FILE *fPtr) {
    char cmd[20];
    printf("Enter command (search/update/new/delete/history/exit): ");
    scanf("%19s", cmd);

    if (strcmp(cmd, "search") == 0) {
        searchByName(fPtr);
    } else if (strcmp(cmd, "update") == 0) {
        updateRecord(fPtr);
    } else if (strcmp(cmd, "new") == 0) {
        newRecord(fPtr);
    } else if (strcmp(cmd, "delete") == 0) {
        deleteRecord(fPtr);
    } else if (strcmp(cmd, "history") == 0) {
        unsigned int acct;
        printf("Enter account number: ");
        scanf("%u", &acct);
        transactionHistory(acct);
    } else if (strcmp(cmd, "exit") == 0) {
        printf("Exiting command mode.\n");
    } else {
        printf("Unknown command.\n");
    }
}

// menu
unsigned int enterChoice(void) {
    unsigned int menuChoice;
    printf("\nEnter your choice\n"
           "1 - store a formatted text file of accounts called \"accounts.txt\"\n"
           "2 - update an account\n"
           "3 - add a new account\n"
           "4 - delete an account\n"
           "5 - search an account by last name\n"
           "6 - view transaction history\n"
           "7 - command mode (type commands directly)\n"
           "9 - end program\n? ");
    scanf("%u", &menuChoice);
    return menuChoice;
}
