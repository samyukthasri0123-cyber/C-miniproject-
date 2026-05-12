#include <stdio.h>
#include <string.h>

struct clientData {
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

int main() {
    FILE *fPtr;
    struct clientData client = {0, "", "", 0.0};
    
    if ((fPtr = fopen("credit.dat", "wb")) == NULL) {
        puts("File could not be created.");
        return 1;
    }
    
    // Write 100 empty records to initialize the file
    for (int i = 0; i < 100; i++) {
        fwrite(&client, sizeof(struct clientData), 1, fPtr);
    }
    
    // Add a sample record at account 1
    client.acctNum = 1;
    strcpy(client.lastName, "Smith");
    strcpy(client.firstName, "John");
    client.balance = 1000.00;
    
    fseek(fPtr, 0 * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    
    // Add another sample record at account 2
    client.acctNum = 2;
    strcpy(client.lastName, "Johnson");
    strcpy(client.firstName, "Jane");
    client.balance = 2500.50;
    
    fseek(fPtr, 1 * sizeof(struct clientData), SEEK_SET);
    fwrite(&client, sizeof(struct clientData), 1, fPtr);
    
    fclose(fPtr);
    puts("credit.dat file created successfully.");
    return 0;
}