import threading
import json
from datetime import datetime
from typing import List, Dict

expenses: List[Dict] = []
DATA_FILE = "expenses.json"


def parse_date(date_str: str) -> datetime:
    try:
        return datetime.strptime(date_str, "%Y-%m-%d")
    except ValueError:
        return datetime.strptime(date_str, "%Y-%m-%d %H:%M:%S")


def save_expenses():
    with open(DATA_FILE, "w") as file:
        json.dump(expenses, file, default=str, indent=4)


def load_expenses():
    global expenses
    try:
        with open(DATA_FILE, "r") as file:
            expenses = json.load(file)
            for exp in expenses:
                exp["date"] = parse_date(exp["date"])
    except FileNotFoundError:
        expenses = []


def add_expense():
    try:
        date = parse_date(input("Enter date (YYYY-MM-DD): "))
        amount = float(input("Enter amount: "))
        category = input("Enter category: ").strip()
        description = input("Enter description: ").strip()

        expense = {
            "date": date,
            "amount": amount,
            "category": category,
            "description": description
        }

        expenses.append(expense)
        threading.Thread(target=save_expenses).start()
        print("Expense added.\n")

    except ValueError:
        print("Invalid input.\n")


def view_expenses():
    if not expenses:
        print("No expenses recorded.\n")
        return

    for exp in expenses:
        print(
            f"{exp['date'].date()} | "
            f"${exp['amount']:.2f} | "
            f"{exp['category']} | "
            f"{exp['description']}"
        )
    print()


def filter_by_category():
    category = input("Enter category: ").strip()
    filtered = [exp for exp in expenses if exp["category"].lower() == category.lower()]

    if not filtered:
        print("No results.\n")
        return

    for exp in filtered:
        print(f"{exp['date'].date()} | ${exp['amount']:.2f} | {exp['description']}")
    print()


def filter_by_date_range():
    try:
        start = parse_date(input("Start date (YYYY-MM-DD): "))
        end = parse_date(input("End date (YYYY-MM-DD): "))

        filtered = [exp for exp in expenses if start <= exp["date"] <= end]

        if not filtered:
            print("No results.\n")
            return

        for exp in filtered:
            print(
                f"{exp['date'].date()} | "
                f"${exp['amount']:.2f} | "
                f"{exp['category']} | "
                f"{exp['description']}"
            )
        print()

    except ValueError:
        print("Invalid date.\n")


def show_summary():
    category_totals: Dict[str, float] = {}
    total = 0.0

    for exp in expenses:
        category = exp["category"]
        amount = exp["amount"]
        category_totals[category] = category_totals.get(category, 0) + amount
        total += amount

    for category, amount in category_totals.items():
        print(f"{category}: ${amount:.2f}")

    print(f"Total: ${total:.2f}\n")


def main():
    load_expenses()

    while True:
        print("1. Add Expense")
        print("2. View Expenses")
        print("3. Filter by Category")
        print("4. Filter by Date Range")
        print("5. Show Summary")
        print("6. Exit")

        choice = input("Choose option: ").strip()

        if choice == "1":
            add_expense()
        elif choice == "2":
            view_expenses()
        elif choice == "3":
            filter_by_category()
        elif choice == "4":
            filter_by_date_range()
        elif choice == "5":
            show_summary()
        elif choice == "6":
            break
        else:
            print("Invalid choice.\n")


if __name__ == "__main__":
    main()

