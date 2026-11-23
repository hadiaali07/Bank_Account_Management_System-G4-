/* storage.c - Member 4: Iqra (CS-004)
save/load file, login, sort and summary report */
#include "bank.h"

/* Saves the number of accounts and all the accounts to the data file.
   It is called when the user chooses "Save and exit". */
void saveToFile(struct Account accounts[], int count) {
    FILE *fp = fopen(DATA_FILE, "wb");        /* open the file for writing in binary mode */
    if (fp == NULL) {                         /* NULL means the file could not be opened */
        printf("Error: could not open file for saving.\n");   /* tell the user */
        return;                               /* stop, so the program does not crash */
    }
    fwrite(&count, sizeof(int), 1, fp);       /* write the number of accounts first */
    fwrite(accounts, sizeof(struct Account), count, fp);   /* write all the accounts */
    fclose(fp);                               /* close the file so the data is saved */
    printf("Data saved (%d accounts).\n", count);   /* confirm to the user */
}

/* Reads the saved accounts from the data file into the accounts array.
   It returns how many accounts were loaded and is called when the program starts. */
int loadFromFile(struct Account accounts[]) {
    int count = 0;                            /* number of accounts found in the file */
    FILE *fp = fopen(DATA_FILE, "rb");        /* open the file for reading in binary mode */
    if (fp == NULL) {                         /* on the first run the file does not exist */
        return 0;                             /* so start with 0 accounts */
    }
    /* read the count first (same order as saving) and check that it is valid */
    if (fread(&count, sizeof(int), 1, fp) != 1 || count < 0 || count > MAX_ACCOUNTS) {
        fclose(fp);                           /* close the file */
        return 0;                             /* the file is empty or damaged */
    }
    /* read all the accounts; if fewer were read than expected, the file is damaged */
    if ((int)fread(accounts, sizeof(struct Account), count, fp) != count) {
        count = 0;                            /* ignore the damaged data */
    }
    fclose(fp);                               /* close the file */
    return count;                             /* give the number of accounts to main */
}

/* Asks for the account number and PIN, with 3 attempts.
   It returns the position of the account on success, or -1 if login fails. */
int login(struct Account accounts[], int count) {
    int attempt, accNo, pin, idx;             /* variables for the attempts and the input */
    for (attempt = 1; attempt <= 3; attempt++) {   /* allow 3 attempts */
        accNo = getValidInt("Login - account number: ", 1, 99999);   /* read account number */
        pin   = getValidInt("Login - PIN: ", 0, 9999);               /* read PIN */
        idx   = findAccountIndex(accounts, count, accNo);   /* find the account (-1 if not found) */
        if (idx != -1 && accounts[idx].pin == pin) {   /* account exists and PIN matches */
            return idx;                       /* login successful, return its position */
        }
        printf("Wrong account number or PIN. Attempt %d of 3.\n", attempt);   /* show the attempt */
    }
    printf("Too many wrong attempts. Login blocked.\n");   /* all 3 attempts failed */
    return -1;                                /* -1 tells main that login failed */
}

/* Swaps two accounts. Pointers are used so the real accounts are changed. */
static void swapAccounts(struct Account *a, struct Account *b) {
    struct Account temp = *a;                 /* keep a copy of the first account */
    *a = *b;                                  /* put the second account in the first place */
    *b = temp;                                /* put the saved copy in the second place */
}

/* Shows the accounts sorted by balance or by name using bubble sort.
   Only a copy is sorted, so the real list and the transaction history stay unchanged. */
void sortAccounts(struct Account accounts[], int count) {
    struct Account copy[MAX_ACCOUNTS];        /* temporary array that will be sorted */
    int i, j, choice, outOfOrder;             /* loop counters, sort choice, order check */
    if (count == 0) {                         /* nothing to sort if there are no accounts */
        printf("No accounts to sort.\n");
        return;
    }
    choice = getValidInt("Sort by 1) balance (high to low)  2) name (A-Z): ", 1, 2);   /* ask how to sort */
    for (i = 0; i < count; i++) {             /* copy every account into the temporary array */
        copy[i] = accounts[i];
    }
    for (i = 0; i < count - 1; i++) {         /* each pass puts one account in its final place */
        for (j = 0; j < count - 1 - i; j++) { /* compare neighbouring accounts */
            if (choice == 1) {
                outOfOrder = copy[j].balance < copy[j + 1].balance;   /* lower balance first is wrong */
            } else {
                outOfOrder = strcmp(copy[j].name, copy[j + 1].name) > 0;   /* later name first is wrong */
            }
            if (outOfOrder) {                 /* if the pair is in the wrong order */
                swapAccounts(&copy[j], &copy[j + 1]);   /* swap the two accounts */
            }
        }
    }
    displayAllAccounts(copy, count);          /* show the sorted copy as a table */
}

/* Shows the number of accounts, the total money and the highest balance. */
void summaryReport(struct Account accounts[], int count) {
    int i;                                    /* loop counter */
    int richest = 0;                          /* position of the highest balance so far */
    float total = 0;                          /* running total of all balances */
    if (count == 0) {                         /* nothing to report if there are no accounts */
        printf("No accounts to summarize.\n");
        return;
    }
    for (i = 0; i < count; i++) {             /* go through every account */
        total += accounts[i].balance;         /* add its balance to the total */
        if (accounts[i].balance > accounts[richest].balance) {   /* is it the highest so far? */
            richest = i;                      /* remember this account */
        }
    }
    printf("Total accounts : %d\n", count);   /* show the number of accounts */
    printf("Total money    : %.2f\n", total); /* show the total money */
    printf("Highest balance: %.2f (%s)\n", accounts[richest].balance, accounts[richest].name);   /* show the richest account */
}
