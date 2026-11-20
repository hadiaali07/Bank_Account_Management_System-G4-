/* main.c - Member 1: menu, input helpers, add/display */
#include <stdlib.h>
#include "bank.h"

float history[MAX_ACCOUNTS][MAX_HISTORY];   /* definition of the 2D array */
int   historyCount[MAX_ACCOUNTS];

/* Reads a whole line and extracts an int; asks again on bad input */
int getValidInt(const char *prompt, int min, int max) {
    char line[100], extra;
    int value;
    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            exit(0);                              /* input closed */
        }
        /* sscanf returns 1 only if a number was read and nothing else follows */
        if (sscanf(line, "%d %c", &value, &extra) == 1 && value >= min && value <= max) {
            return value;
        }
        printf("Invalid input. Enter a whole number from %d to %d.\n", min, max);
    }
}

/* Same idea for decimal numbers (minimum value only) */
float getValidFloat(const char *prompt, float min) {
    char line[100], extra;
    float value;
    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            exit(0);
        }
        if (sscanf(line, "%f %c", &value, &extra) == 1 && value >= min) {
            return value;
        }
        printf("Invalid input. Enter a number of at least %.2f.\n", min);
    }
}

/* Reads a non-empty line of text (spaces allowed) */
void getValidString(const char *prompt, char *dest, int size) {
    while (1) {
        printf("%s", prompt);
        if (fgets(dest, size, stdin) == NULL) {
            exit(0);
        }
        dest[strcspn(dest, "\n")] = '\0';         /* remove the Enter key */
        if (strlen(dest) > 0) {
            return;
        }
        printf("This field cannot be empty.\n");
    }
}

/* Adds a new account after validating every field */
void addAccount(struct Account accounts[], int *count) {
    struct Account a;
    if (*count >= MAX_ACCOUNTS) {
        printf("Bank is full. Cannot add more accounts.\n");
        return;
    }
    a.accNo = getValidInt("Account number (1-99999): ", 1, 99999);
    if (findAccountIndex(accounts, *count, a.accNo) != -1) {
        printf("Error: account number already exists.\n");
        return;
    }
    getValidString("Name: ", a.name, sizeof(a.name));
    getValidString("Phone: ", a.phone, sizeof(a.phone));
    a.pin = getValidInt("4-digit PIN (1000-9999): ", 1000, 9999);
    a.balance = getValidFloat("Opening balance (min 500): ", MIN_BALANCE);
    a.type = getValidInt("Type (1 = Savings, 2 = Current): ", 1, 2);
        if (a.type == 2) {
        a.info.overdraftLimit = getValidFloat("Overdraft limit: ", 0);
    } else {
        a.info.interestRate = 0;
    }
    accounts[*count] = a;
    historyCount[*count] = 0;
    (*count)++;
    printf("Account created successfully.\n");
}

/* Prints all accounts as a table */
void displayAllAccounts(struct Account accounts[], int count) {
    int i;
    if (count == 0) {
        printf("No accounts to show.\n");
        return;
    }
    printf("\n%-10s %-20s %-14s %-8s %10s\n", "AccNo", "Name", "Phone", "Type", "Balance");
    printf("-------------------------------------------------------------------\n");
    for (i = 0; i < count; i++) {
        printf("%-10d %-20s %-14s %-8s %10.2f\n", accounts[i].accNo, accounts[i].name,
               accounts[i].phone, accounts[i].type == 1 ? "Savings" : "Current",
               accounts[i].balance);
    }
}

int main(void) {
    struct Account accounts[MAX_ACCOUNTS];
    int count = loadFromFile(accounts);
    int choice, idx, toNo, toIdx;

    printf("%d account(s) loaded from file.\n", count);
    do {
        printf("\n===== BANK MANAGEMENT SYSTEM =====\n");
        printf(" 1. Add account            2. Display all\n");
        printf(" 3. Search account         4. Update account\n");
        printf(" 5. Delete account         6. Deposit\n");
        printf(" 7. Withdraw               8. Transfer\n");
        printf(" 9. Balance inquiry       10. Mini statement\n");
        printf("11. Change PIN            12. Sort accounts\n");
        printf("13. Summary report         0. Save and exit\n");
        choice = getValidInt("Your choice: ", 0, 13);

        switch (choice) {
            case 1:  addAccount(accounts, &count); break;
            case 2:  displayAllAccounts(accounts, count); break;
            case 3:  searchAccount(accounts, count); break;
            case 4:  updateAccount(accounts, count); break;
            case 5:  deleteAccount(accounts, &count); break;
            case 12: sortAccounts(accounts, count); break;
            case 13: summaryReport(accounts, count); break;
            case 0:  break;
            default:
                /* these options need the customer to log in first */
                idx = login(accounts, count);
                if (idx == -1) break;
                printf("Welcome, %s!\n", accounts[idx].name);
                if (choice == 6)       deposit(&accounts[idx], idx);
                else if (choice == 7)  withdraw(&accounts[idx], idx);
                else if (choice == 9)  balanceInquiry(&accounts[idx]);
                else if (choice == 10) showMiniStatement(idx);
                else if (choice == 11) changePIN(&accounts[idx]);
                else if (choice == 8) {
                    toNo = getValidInt("Receiver account number: ", 1, 99999);
                    toIdx = findAccountIndex(accounts, count, toNo);
                    if (toIdx == -1) printf("Receiver account not found.\n");
                    else transfer(&accounts[idx], &accounts[toIdx], idx, toIdx);
                }
        }
    } while (choice != 0);

    saveToFile(accounts, count);
    return 0;
}
