/* bank_updated.h - shared constants, structures and function prototypes */
#ifndef BANK_H
#define BANK_H

#include <stdio.h>
#include <string.h>

#define MAX_ACCOUNTS 100      /* maximum number of accounts */
#define MAX_HISTORY  5        /* last 5 transactions per account */
#define MIN_BALANCE  500      /* minimum balance that must remain */
#define DATA_FILE    "accounts.dat"

/* Union: holds ONE value at a time, depending on account type */
union TypeInfo {
    float interestRate;       /* savings account (percent per year) */
    float overdraftLimit;     /* current account */
};

struct Account {
    int   accNo;
    char  name[50];
    char  phone[15];
    int   pin;
    float balance;
    int   type;               /* 1 = savings, 2 = current */
    union TypeInfo info;
};

/* 2D array: last 5 transactions of every account (+ deposit, - withdraw) */
extern float history[MAX_ACCOUNTS][MAX_HISTORY];
extern int   historyCount[MAX_ACCOUNTS];

/* ---- Input helpers (main.c) ---- */
int   getValidInt(const char *prompt, int min, int max);
float getValidFloat(const char *prompt, float min);
void  getValidString(const char *prompt, char *dest, int size);
void addAccount(struct Account accounts[], int *count);
void displayAllAccounts(struct Account accounts[], int count);

/* ---- Records ---- */
int  findAccountIndex(struct Account accounts[], int count, int accNo);
void searchAccount(struct Account accounts[], int count);
void updateAccount(struct Account accounts[], int count);
void deleteAccount(struct Account accounts[], int *count);
void changePIN(struct Account *acc);

/* ---- Transactions ---- */
void  deposit(struct Account *acc, int index);
void  withdraw(struct Account *acc, int index);
void  transfer(struct Account *from, struct Account *to, int fromIdx, int toIdx);
void  balanceInquiry(struct Account *acc);
void  recordTransaction(int index, float amount);
void  showMiniStatement(int index);

/* ---- Data and security ---- */
void saveToFile(struct Account accounts[], int count);
int  loadFromFile(struct Account accounts[]);
int  login(struct Account accounts[], int count);
void sortAccounts(struct Account accounts[], int count);
void summaryReport(struct Account accounts[], int count);

#endif
