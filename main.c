/* main.c - Member 1: Areesha Amir menu, input helpers, add/display */
#include <stdlib.h>                               /* for exit() */
#include "bank_updated.h"                         /* our structure, constants and function prototypes */

/* The two history arrays are created here, once. Other files reach them through extern in the header.
   history[i] holds the last 5 transactions of accounts[i]. */
float history[MAX_ACCOUNTS][MAX_HISTORY];   /* definition of the 2D array */
int   historyCount[MAX_ACCOUNTS];           /* how many history slots each account has used */

/* Asks the question again and again until the user types a whole number from min to max.
   It returns that number. */
int getValidInt(const char *prompt, int min, int max) {
    char line[100], extra;                    /* line holds the typed text, extra catches junk after the number */
    int value;                                /* the number taken out of the text */
    while (1) {                               /* repeat until a valid number is returned */
        printf("%s", prompt);                 /* show the question */
        if (fgets(line, sizeof(line), stdin) == NULL) {   /* read the whole typed line as text */
            exit(0);                          /* input closed */
        }
        /* sscanf returns 1 only if a number was read and nothing else follows */
        if (sscanf(line, "%d %c", &value, &extra) == 1 && value >= min && value <= max) {
            return value;                     /* clean number in range: give it back */
        }
        printf("Invalid input. Enter a whole number from %d to %d.\n", min, max);   /* explain and ask again */
    }
}

/* Same as getValidInt but for decimal numbers, and it only checks a minimum value.
   It returns the number typed. */
float getValidFloat(const char *prompt, float min) {
    char line[100], extra;                    /* typed text and a catcher for junk after the number */
    float value;                              /* the decimal number taken out of the text */
    while (1) {                               /* repeat until a valid number is returned */
        printf("%s", prompt);                 /* show the question */
        if (fgets(line, sizeof(line), stdin) == NULL) {   /* read the typed line as text */
            exit(0);                          /* input closed */
        }
        if (sscanf(line, "%f %c", &value, &extra) == 1 && value >= min) {   /* clean number, not below min */
            return value;                     /* give it back */
        }
        printf("Invalid input. Enter a number of at least %.2f.\n", min);   /* explain and ask again */
    }
}

/* Reads a line of text such as a name or phone number and puts it in dest.
   It keeps asking until the text is not empty. */
void getValidString(const char *prompt, char *dest, int size) {
    while (1) {                               /* repeat until something is typed */
        printf("%s", prompt);                 /* show the question */
        if (fgets(dest, size, stdin) == NULL) {   /* read the line straight into dest */
            exit(0);                          /* input closed */
        }
        dest[strcspn(dest, "\n")] = '\0';     /* remove the Enter key from the end */
        if (strlen(dest) > 0) {               /* something was typed */
            return;                           /* done, the text is already in dest */
        }
        printf("This field cannot be empty.\n");   /* nothing typed, ask again */
    }
}

/* Adds one new account after checking every field.
   count is a pointer because this function increases the real count in main. */
void addAccount(struct Account accounts[], int *count) {
    struct Account a;                         /* temporary form that is filled in first */
    if (*count >= MAX_ACCOUNTS) {             /* is the bank already full? */
        printf("Bank is full. Cannot add more accounts.\n");
        return;                               /* stop, there is no room */
    }
    a.accNo = getValidInt("Account number (1-99999): ", 1, 99999);   /* read the account number */
    if (findAccountIndex(accounts, *count, a.accNo) != -1) {   /* anything but -1 means it already exists */
        printf("Error: account number already exists.\n");
        return;                               /* stop, numbers must be unique */
    }
    getValidString("Name: ", a.name, sizeof(a.name));      /* read the name into the form */
    getValidString("Phone: ", a.phone, sizeof(a.phone));   /* read the phone into the form */
    a.pin = getValidInt("4-digit PIN (1000-9999): ", 1000, 9999);   /* read the PIN */
    a.balance = getValidFloat("Opening balance (min 500): ", MIN_BALANCE);   /* read the opening balance */
    a.type = getValidInt("Type (1 = Savings, 2 = Current): ", 1, 2);   /* 1 = savings, 2 = current */
    if (a.type == 2) {                        /* a current account has an overdraft limit */
        a.info.overdraftLimit = getValidFloat("Overdraft limit: ", 0);   /* ask for it */
    } else {                                  /* a savings account */
        a.info.interestRate = 0;              /* no interest, so just store 0 */
    }
    accounts[*count] = a;                     /* copy the form into the next free place in the list */
    historyCount[*count] = 0;                 /* the new account starts with no transactions */
    (*count)++;                               /* one more account now (brackets: change the value inside count) */
    printf("Account created successfully.\n");   /* confirm to the user */
}

/* Prints every account as a neat table.
   count is a plain copy because this function only reads it. */
void displayAllAccounts(struct Account accounts[], int count) {
    int i;                                    /* loop counter */
    if (count == 0) {                         /* nothing to show */
        printf("No accounts to show.\n");
        return;                               /* stop */
    }
    printf("\n%-10s %-20s %-14s %-8s %10s\n", "AccNo", "Name", "Phone", "Type", "Balance");   /* column titles */
    printf("-------------------------------------------------------------------\n");   /* line under the titles */
    for (i = 0; i < count; i++) {             /* one row per account, last index is count - 1 */
        printf("%-10d %-20s %-14s %-8s %10.2f\n", accounts[i].accNo, accounts[i].name,   /* number and name */
               accounts[i].phone, accounts[i].type == 1 ? "Savings" : "Current",   /* phone and type as a word */
               accounts[i].balance);          /* balance with 2 decimals */
    }
}

/* Program starts here: loads saved accounts, shows the menu again and again,
   calls the right function for each choice, and saves everything on exit. */
int main(void) {
    struct Account accounts[MAX_ACCOUNTS];    /* the real list of accounts */
    int count = loadFromFile(accounts);       /* load saved accounts and get how many there are */
    int choice, idx, toNo, toIdx;             /* menu choice, logged-in position, receiver number and position */

    printf("%d account(s) loaded from file.\n", count);   /* tell the user how many were loaded */
    do {                                      /* show the menu at least once */
        printf("\n===== BANK MANAGEMENT SYSTEM =====\n");   /* menu heading */
        printf(" 1. Add account            2. Display all\n");   /* menu lines */
        printf(" 3. Search account         4. Update account\n");
        printf(" 5. Delete account         6. Deposit\n");
        printf(" 7. Withdraw               8. Transfer\n");
        printf(" 9. Balance inquiry       10. Mini statement\n");
        printf("11. Change PIN            12. Sort accounts\n");
        printf("13. Summary report         0. Save and exit\n");
        choice = getValidInt("Your choice: ", 0, 13);   /* read a choice from 0 to 13 */

        switch (choice) {                     /* run the function that matches the choice */
            case 1:  addAccount(accounts, &count); break;       /* &count so it can change count */
            case 2:  displayAllAccounts(accounts, count); break;   /* only reads count */
            case 3:  searchAccount(accounts, count); break;     /* find and show an account */
            case 4:  updateAccount(accounts, count); break;     /* change name and phone */
            case 5:  deleteAccount(accounts, &count); break;    /* &count so it can reduce count */
            case 12: sortAccounts(accounts, count); break;      /* show sorted accounts */
            case 13: summaryReport(accounts, count); break;     /* show totals */
            case 0:  break;                   /* leave the switch, the loop then ends */
            default:
                /* these options need the customer to log in first */
                idx = login(accounts, count);   /* position of the logged-in account, or -1 */
                if (idx == -1) break;         /* login failed, back to the menu */
                printf("Welcome, %s!\n", accounts[idx].name);   /* greet the customer */
                if (choice == 6)       deposit(&accounts[idx], idx);       /* address of that account and its position */
                else if (choice == 7)  withdraw(&accounts[idx], idx);      /* same for withdraw */
                else if (choice == 9)  balanceInquiry(&accounts[idx]);     /* only needs the account */
                else if (choice == 10) showMiniStatement(idx);             /* only needs the position */
                else if (choice == 11) changePIN(&accounts[idx]);          /* only needs the account */
                else if (choice == 8) {       /* transfer needs a receiver as well */
                    toNo = getValidInt("Receiver account number: ", 1, 99999);   /* receiver's account number */
                    toIdx = findAccountIndex(accounts, count, toNo);   /* receiver's position, or -1 */
                    if (toIdx == -1) printf("Receiver account not found.\n");   /* no such account */
                    else transfer(&accounts[idx], &accounts[toIdx], idx, toIdx);   /* sender and receiver */
                }
        }
    } while (choice != 0);                    /* keep going until the user chooses 0 */

    saveToFile(accounts, count);              /* write all accounts to the file before exiting */
    return 0;                                 /* end of the program */
}
