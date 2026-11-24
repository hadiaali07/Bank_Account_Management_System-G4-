// Member 2: Hadia Ali (CS-011)
/* records.c - Member 2: find, search, update, delete, change PIN */
#include "bank_updated.h"

/* Returns index of the account with this number, or -1 if not found */
int findAccountIndex(struct Account accounts[], int count, int accNo) {
    int i;
    for (i = 0; i < count; i++) {
        if (accounts[i].accNo == accNo) {
            return i;
        }
    }
    return -1;
}

/* Prints one account's details */
static void printAccount(struct Account *a) {
    printf("Account No : %d\nName       : %s\nPhone      : %s\n", a->accNo, a->name, a->phone);
    printf("Type       : %s\nBalance    : %.2f\n", a->type == 1 ? "Savings" : "Current", a->balance);
    if (a->type == 1) printf("Interest   : %.2f%%\n", a->info.interestRate);
    else              printf("Overdraft  : %.2f\n", a->info.overdraftLimit);
}

/* Search by account number or by exact name */
void searchAccount(struct Account accounts[], int count) {
    int choice, i, idx, found = 0;
    char name[50];
    choice = getValidInt("Search by 1) account number  2) name: ", 1, 2);
    if (choice == 1) {
        idx = findAccountIndex(accounts, count, getValidInt("Account number: ", 1, 99999));
        if (idx == -1) printf("Account not found.\n");
        else printAccount(&accounts[idx]);
    } else {
        getValidString("Name: ", name, sizeof(name));
        for (i = 0; i < count; i++) {
            if (strcmp(accounts[i].name, name) == 0) {
                printAccount(&accounts[i]);
                found = 1;
            }
        }
        if (!found) printf("Account not found.\n");
    }
}

/* Changes the name and phone of an existing account */
void updateAccount(struct Account accounts[], int count) {
    int idx = findAccountIndex(accounts, count, getValidInt("Account number to update: ", 1, 99999));
    if (idx == -1) {
        printf("Account not found.\n");
        return;
    }
    printf("Current details:\n");
    printAccount(&accounts[idx]);
    getValidString("New name: ", accounts[idx].name, sizeof(accounts[idx].name));
    getValidString("New phone: ", accounts[idx].phone, sizeof(accounts[idx].phone));
    printf("Account updated.\n");
}

/* Removes an account by shifting later ones left (history shifts too) */
void deleteAccount(struct Account accounts[], int *count) {
    int i, j, idx = findAccountIndex(accounts, *count, getValidInt("Account number to delete: ", 1, 99999));
    if (idx == -1) {
        printf("Account not found.\n");
        return;
    }
    for (i = idx; i < *count - 1; i++) {
        accounts[i] = accounts[i + 1];
        for (j = 0; j < MAX_HISTORY; j++) {
            history[i][j] = history[i + 1][j];
        }
        historyCount[i] = historyCount[i + 1];
    }
    (*count)--;
    historyCount[*count] = 0;
    printf("Account deleted.\n");
}

/* Changes the PIN after checking the old one */
void changePIN(struct Account *acc) {
    int oldPin = getValidInt("Old PIN: ", 0, 9999);
    if (oldPin != acc->pin) {
        printf("Wrong old PIN.\n");
        return;
    }
    acc->pin = getValidInt("New 4-digit PIN: ", 1000, 9999);
    printf("PIN changed.\n");
}
