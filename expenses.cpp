#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

const int MAX_CATEGORIES = 5;
const int MAX_PERIODS = 100;
const string categories[MAX_CATEGORIES] = {"Shopping", "Bills", "Traveling", "Food", "Others"};

// ---------------- Base Class ----------------
class ExpenseBase {
public:
    virtual void inputExpenses(float allowed) = 0;
    virtual float calculateOverallTotal() = 0;
    virtual void showCategoryTotals() = 0;
    virtual float totalSpentThisMonth() = 0;
    virtual float forecastMonthly() = 0;
    virtual float forecastYearly() = 0;
    virtual ~ExpenseBase() {}
};

// ---------------- Derived Class ----------------
class Expense : public ExpenseBase {
    float expenses[MAX_CATEGORIES][MAX_PERIODS]{};
    int numPeriods, mode;
    bool overBudget = false;

public:
    Expense(int p, int m) : numPeriods(p), mode(m) {}

    void inputExpenses(float allowed) override {
        if (allowed <= 0) {
            cout << "\nNo expense allowed (saving >= income).\n";
            return;
        }

        float runningTotal = 0.0f;

        for (int i = 0; i < MAX_CATEGORIES; ++i) {
            cout << "\nCategory: " << categories[i] << endl;

            for (int j = 0; j < numPeriods; ++j) {

                if (overBudget) {
                    expenses[i][j] = 0;
                    continue;
                }

                cout << "  Period " << (j + 1) << " expense (Rs.): ";
                float x;
                cin >> x;

                if (cin.fail() || x < 0) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    cout << "  Invalid input. Setting to 0.\n";
                    x = 0;
                }

                if (runningTotal + x > allowed) {
                    cout << "\n*** Budget exceeded! No more entries allowed. ***\n";
                    overBudget = true;
                    expenses[i][j] = 0;
                } else {
                    expenses[i][j] = x;
                    runningTotal += x;
                }
            }
        }
    }

    float calculateOverallTotal() override {
        float total = 0;
        for (int i = 0; i < MAX_CATEGORIES; ++i)
            for (int j = 0; j < numPeriods; ++j)
                total += expenses[i][j];
        return total;
    }

    void showCategoryTotals() override {
        cout << "\nTotal Expenses by Category:\n";
        for (int i = 0; i < MAX_CATEGORIES; ++i) {
            float sum = 0;
            for (int j = 0; j < numPeriods; ++j)
                sum += expenses[i][j];

            cout << " - " << categories[i] << ": Rs. " << sum << endl;
        }
    }

    float totalSpentThisMonth() override {
        float total = calculateOverallTotal();

        if (mode == 1) return total;           // daily input (within month)
        else if (mode == 2) return total;      // weekly input
        else return total / (numPeriods * 12); // yearly → monthly avg
    }

    float forecastMonthly() override {
        float total = calculateOverallTotal();
        float avgPerPeriod = total / numPeriods;

        if (mode == 1) return avgPerPeriod * 30;
        else if (mode == 2) return avgPerPeriod * 4.3f;
        else return total / (numPeriods * 12);
    }

    float forecastYearly() override {
        float total = calculateOverallTotal();
        float avgPerPeriod = total / numPeriods;

        if (mode == 1) return avgPerPeriod * 365;
        else if (mode == 2) return avgPerPeriod * 52;
        else return total;
    }
};

// ---------------- Advice ----------------
void showAdvice(float totalIncome, float totalSaving, float estimated, bool yearly) {
    float available = totalIncome - totalSaving;

    cout << "\nMoney Saving Advice:\n-------------------------------------\n";

    if (estimated <= available) {
        cout << "Your expenses are within your savings goal.\n";

        float perMonth = yearly ? (estimated / 12.0f) : estimated;

        if (perMonth < 5000)
            cout << "Low spending. Consider investing.\n";
        else if (perMonth <= 15000)
            cout << "Moderate spending. Monitor subscriptions.\n";
        else
            cout << "High spending. Try strict budgeting.\n";
    } else {
        cout << "Expenses exceed your budget.\n";
        cout << "Reduce expenses by Rs. " << (estimated - available)
             << (yearly ? " per year.\n" : " per month.\n");
    }
}

// ---------------- Main ----------------
int main() {
    int mode, periods;
    float income, saving;
    char runAgain;

    do {
        cout << fixed << setprecision(2);

        cout << "\n========= EXPENSE FORECASTER =========\n";

        cout << "1. Daily\n2. Weekly\n3. Yearly\nEnter choice: ";
        cin >> mode;

        if (mode < 1 || mode > 3) {
            cout << "Invalid choice.\n";
            return 1;
        }

        cout << "Enter number of periods: ";
        cin >> periods;

        if (periods <= 0 || periods > MAX_PERIODS) {
            cout << "Invalid number.\n";
            return 1;
        }

        cout << "Enter income (Rs.): ";
        cin >> income;

        cout << "Enter savings (Rs.): ";
        cin >> saving;

        float totalIncome = income;
        float totalSaving = saving;

        if (mode == 3) {
            totalIncome = income * periods;
            totalSaving = saving * periods;
        }

        float allowedExpense = totalIncome - totalSaving;

        Expense expObj(periods, mode);
        ExpenseBase* exp = &expObj;

        exp->inputExpenses(allowedExpense);

        float totalExpenses = exp->calculateOverallTotal();
        float monthlyEstimate = exp->forecastMonthly();
        float yearlyEstimate = exp->forecastYearly();

        cout << "\n============= SUMMARY =============\n";
        exp->showCategoryTotals();

        cout << "Total Spent: Rs. " << totalExpenses << endl;

        if (mode == 3) {
            cout << "Total Income: Rs. " << totalIncome << endl;
            cout << "Total Saving: Rs. " << totalSaving << endl;
        } else {
            cout << "Monthly Estimate: Rs. " << monthlyEstimate << endl;
            cout << "Yearly Estimate : Rs. " << yearlyEstimate << endl;
        }

        showAdvice(totalIncome, totalSaving,
                   (mode == 3 ? yearlyEstimate : monthlyEstimate),
                   (mode == 3));

        cout << "\nRun again? (Y/N): ";
        cin >> runAgain;

    } while (runAgain == 'Y' || runAgain == 'y');

    cout << "\nThank you!\n";
    return 0;
}
