/* transactions.c - Member 3: deposit, withdraw, transfer, history */
#include "bank_updated.h"

/* Stores an amount in the account's last-5 history (shifts left when full) */
void recordTransaction(int index, float amount) {
    int i;
    if (historyCount[index] == MAX_HISTORY) {
        for (i = 0; i < MAX_HISTORY - 1; i++) {
            history[index][i] = history[index][i + 1];
        }
        history[index][MAX_HISTORY - 1] = amount;
    } else {
        history[index][historyCount[index]] = amount;
        historyCount[index]++;
    }
}

/* Lowest balance allowed (current accounts may use their overdraft) */
static float lowestAllowed(struct Account *acc) {
    return acc->type == 2 ? MIN_BALANCE - acc->info.overdraftLimit : MIN_BALANCE;
}

/* Adds money to the account */
void deposit(struct Account *acc, int index) {
    float amount = getValidFloat("Deposit amount: ", 0.01f);
    acc->balance += amount;
    recordTransaction(index, amount);
    printf("Deposited %.2f. New balance: %.2f\n", amount, acc->balance);
}

/* Takes money out, keeping the minimum balance */
void withdraw(struct Account *acc, int index) {
    float amount = getValidFloat("Withdraw amount: ", 0.01f);
    if (acc->balance - amount < lowestAllowed(acc)) {
        printf("Insufficient balance (minimum %.2f must remain).\n", lowestAllowed(acc));
        return;
    }
    acc->balance -= amount;
    recordTransaction(index, -amount);
    printf("Withdrawn %.2f. New balance: %.2f\n", amount, acc->balance);
}

/* Sends money from one account to another */
void transfer(struct Account *from, struct Account *to, int fromIdx, int toIdx) {
    float amount;
    if (fromIdx == toIdx) {
        printf("Cannot transfer to the same account.\n");
        return;
    }
    amount = getValidFloat("Transfer amount: ", 0.01f);
    if (from->balance - amount < lowestAllowed(from)) {
        printf("Insufficient balance (minimum %.2f must remain).\n", lowestAllowed(from));
        return;
    }
    from->balance -= amount;
    to->balance   += amount;
    recordTransaction(fromIdx, -amount);
    recordTransaction(toIdx, amount);
    printf("Transferred %.2f to %s. Your balance: %.2f\n", amount, to->name, from->balance);
}

/* Shows the current balance */
void balanceInquiry(struct Account *acc) {
    printf("Current balance of %s: %.2f\n", acc->name, acc->balance);
}

/* Prints the stored transactions (oldest first) */
void showMiniStatement(int index) {
    int i;
    if (historyCount[index] == 0) {
        printf("No transactions in this session yet.\n");
        return;
    }
    printf("--- Last %d transaction(s) ---\n", historyCount[index]);
    for (i = 0; i < historyCount[index]; i++) {
        printf("%2d. %s%.2f\n", i + 1, history[index][i] >= 0 ? "+" : "", history[index][i]);
    }
}
