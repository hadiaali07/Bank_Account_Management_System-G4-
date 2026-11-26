/*Transactions.c -Member 3:AYESHA SALAHUDDIN CS-010*/
/* transactions.c - Member 3: Transaction Management - deposite,withdraw,transfer,histroy*/

#include "bank_updated.h" // Includes shared header file

/* Stores the last 5 transactions */
void recordTransaction(int index, float amount) { // Records a transaction
    int i; // Loop variable

    if (historyCount[index] == MAX_HISTORY) { // Checks if history is full
        for (i = 0; i < MAX_HISTORY - 1; i++) { // Shifts old transactions
            history[index][i] = history[index][i + 1]; // Moves history left
        }
        history[index][MAX_HISTORY - 1] = amount; // Stores latest transaction
    }
    else {
        history[index][historyCount[index]] = amount; // Stores new transaction
        historyCount[index]++; // Increases history count
    }
}

/* Calculates minimum allowed balance */
static float lowestAllowed(struct Account *acc) { // Finds minimum balance
    return acc->type == 2 // Checks account type
        ? MIN_BALANCE - acc->info.overdraftLimit // Current account limit
        : MIN_BALANCE; // Savings account limit
}

/* Deposits money into an account */
void deposit(struct Account *acc, int index) { // Deposit function
    float amount = getValidFloat("Deposit amount: ", 0.01f); // Takes amount

    acc->balance += amount; // Adds money to balance
    recordTransaction(index, amount); // Saves transaction

    printf("Deposit successful! New balance: %.2f\n", acc->balance); // Displays balance
}

/* Withdraws money from an account */
void withdraw(struct Account *acc, int index) { // Withdrawal function
    float amount = getValidFloat("Withdraw amount: ", 0.01f); // Takes amount

    if (acc->balance - amount < lowestAllowed(acc)) { // Checks minimum balance
        printf("Insufficient balance!\n"); // Displays error
        return; // Stops function
    }

    acc->balance -= amount; // Deducts money
    recordTransaction(index, -amount); // Saves negative transaction

    printf("Withdrawal successful! New balance: %.2f\n", acc->balance); // Displays balance
}

/* Transfers money between accounts */
void transfer(struct Account *from, struct Account *to, int fromIdx, int toIdx) { // Transfer function
    float amount; // Stores transfer amount

    if (fromIdx == toIdx) { // Checks same account
        printf("Cannot transfer to the same account!\n"); // Displays error
        return; // Stops function
    }

    amount = getValidFloat("Transfer amount: ", 0.01f); // Takes amount

    if (from->balance - amount < lowestAllowed(from)) { // Checks sender balance
        printf("Insufficient balance!\n"); // Displays error
        return; // Stops function
    }

    from->balance -= amount; // Deducts from sender
    to->balance += amount; // Adds to receiver

    recordTransaction(fromIdx, -amount); // Records sender transaction
    recordTransaction(toIdx, amount); // Records receiver transaction

    printf("Transfer successful!\n"); // Displays success
}

/* Displays account balance */
void balanceInquiry(struct Account *acc) { // Balance inquiry function
    printf("\nAccount Holder: %s\n", acc->name); // Displays account holder
    printf("Current Balance: %.2f\n", acc->balance); // Displays balance
}

/* Displays transaction history */
void showMiniStatement(int index) { // Mini statement function
    int i; // Loop variable

    if (historyCount[index] == 0) { // Checks empty history
        printf("No transactions found.\n"); // Displays message
        return; // Stops function
    }

    printf("\n--- Mini Statement ---\n"); // Prints heading

    for (i = 0; i < historyCount[index]; i++) { // Loops through history
        printf("%2d. %s%.2f\n", // Prints transaction
               i + 1, // Transaction number
               history[index][i] >= 0 ? "+" : "", // Adds plus sign
               history[index][i]); // Displays amount
    }
}
