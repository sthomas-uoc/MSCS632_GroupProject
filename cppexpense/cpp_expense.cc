#include <iostream>
#include <map>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

template <typename EnumType>
string GetEnumString(const map<EnumType, string> &enum_map, const EnumType &value)
{

  auto it = enum_map.find(value);
  if (it != enum_map.end())
  {
    return it->second;
  }

  return "";
}

template <typename EnumType>
EnumType StringToEnum(const map<string, EnumType> &enum_map, const string &str)
{
  auto it = enum_map.find(str);
  if (it != enum_map.end())
  {
    return it->second;
  }

  throw invalid_argument("Invalid enum value: " + str);
}

void ToUpperCase(string &str)
{
  transform(str.begin(), str.end(), str.begin(), [](unsigned char c)
            { return toupper(c); });
}

template <typename EnumType>
EnumType ReadEnum(const map<string, EnumType> &enum_map, const string &prompt)
{
  string input;
  while (true)
  {
    cout << prompt << "> ";
    getline(cin, input);

    ToUpperCase(input);

    try
    {
      auto enum_val = StringToEnum(enum_map, input);
      return enum_val;
    }
    catch (const invalid_argument &e)
    {
      cout << "Invalid value" << endl;
    }
  }
}

double ReadNumber(const string &prompt, bool positive)
{

  string input;

  while (true)
  {
    cout << prompt << "> ";
    getline(cin, input);

    try
    {
      double number = stod(input);
      if (positive && 0 > number)
      {
        throw invalid_argument("Enter a positive number");
      }
      return number;
    }
    catch (const invalid_argument &e)
    {
      cout << "Invalid number" << endl;
    }
    catch (const out_of_range &e)
    {
      cout << "Number is out of range" << endl;
    }
  }

  return 0.;
}

tm ReadDate(const string &prompt)
{

  tm date = {};
  string input;

  while (true)
  {
    cout << prompt << " (YYYY-MM-DD) > ";
    getline(cin, input);

    istringstream ss(input);
    ss >> get_time(&date, "%Y-%m-%d");

    if (!ss.fail())
    {
      // Check if the date can be represented as valid date
      if (mktime(&date) != -1)
      {
        return date;
      }
    }
    cout << "Invalid date" << endl;
  }

  return date;
}

string GetDateString(const tm &date)
{
  ostringstream date_ss;
  date_ss << put_time(&date, "%Y-%m-%d");

  return date_ss.str();
}

enum class ExpenseCategory
{
  GROCERIES = 1,
  TRAVEL,
  CLOTHING,
  ENTERTAINMENT,
  OTHER = 10
};

const map<string, ExpenseCategory> kStrExpenseCatEnums = {
    {"GROCERIES", ExpenseCategory::GROCERIES},
    {"TRAVEL", ExpenseCategory::TRAVEL},
    {"CLOTHING", ExpenseCategory::CLOTHING},
    {"ENTERTAINMENT", ExpenseCategory::ENTERTAINMENT},
    {"OTHER", ExpenseCategory::OTHER}};

const map<ExpenseCategory, string> kExpenseCatStrEnums = {
    {ExpenseCategory::GROCERIES, "Groceries"},
    {ExpenseCategory::TRAVEL, "Travel"},
    {ExpenseCategory::CLOTHING, "Clothing"},
    {ExpenseCategory::ENTERTAINMENT, "Entertainment"},
    {ExpenseCategory::OTHER, "Other"}};

enum class SearchType
{
  CATEGORY = 1,
  DATE_RANGE,
  DATE
};

const map<string, SearchType> kStrSearchTypEnums = {
    {"CATEGORY", SearchType::CATEGORY},
    {"DATE_RANGE", SearchType::DATE_RANGE},
    {"DATE", SearchType::DATE}};

class Expense
{
public:
  double amount;
  string description;
  ExpenseCategory category;
  tm expense_date;

  Expense(double amt, const string &desc, ExpenseCategory cat, tm date)
      : amount(amt), description(desc), category(cat), expense_date(date) {}
};

class ExpenseTracker
{

private:
  int expense_id = 1;
  map<int, Expense> expenses;
  // map<ExpenseCategory, vector<Expense>> categories;
  double total = 0.;

  void AddExpense()
  {

    double amount = ReadNumber(" amount", true);

    string description;
    cout << " description> ";
    getline(cin, description);

    Expense expense(amount,
                    description,
                    ReadEnum(kStrExpenseCatEnums, " category"),
                    ReadDate(" expense date"));

    expenses.insert({expense_id++, expense});
    total += amount;
  };

  void ListExpenses()
  {
    cout << "Entry\tDate\t\tAmount\t\tCategory\tDescription" << endl;
    for (const auto &entry : expenses)
    {
      const auto &expense = entry.second;
      cout << entry.first << "\t"
           << GetDateString(expense.expense_date) << "\t"
           << expense.amount << "\t\t"
           << GetEnumString(kExpenseCatStrEnums, expense.category) << "\t\t"
           << expense.description << endl;
    }
  };

  void ShowTotal()
  {
    map<ExpenseCategory, double> category_totals;
    for (const auto &entry : expenses)
    {
      const auto &expense = entry.second;
      category_totals[expense.category] += expense.amount;
    }

    cout << "Category\tTotal" << endl;

    for (const auto &entry : category_totals)
    {
      cout << GetEnumString(kExpenseCatStrEnums, entry.first) << "\t"
           << entry.second << endl;
    }
    cout << "Total: " << total << endl;
  };

  void SearchExpenses()
  {
    if (expenses.size() == 0)
    {
      cout << "No expenses" << endl;
      return;
    }

    auto search_type = ReadEnum(kStrSearchTypEnums, " search type");
    if (search_type == SearchType::CATEGORY)
    {
      auto category = ReadEnum(kStrExpenseCatEnums, " category");
      cout << "Entry\tDate\t\tAmount\t\tCategory\tDescription" << endl;
      for (const auto &entry : expenses)
      {
        const auto& expense = entry.second;
        if (expense.category == category)
        {
          cout << entry.first << "\t"
               << GetDateString(expense.expense_date) << "\t"
               << expense.amount << "\t\t"
               << GetEnumString(kExpenseCatStrEnums, expense.category) << "\t\t"
               << expense.description << endl;
        }
      }
    }
    else if (search_type == SearchType::DATE_RANGE)
    {
      auto tm_from_date = ReadDate(" from");
      auto tm_to_date = ReadDate(" to");
      auto from_date = mktime(&tm_from_date);
      auto to_date = mktime(&tm_to_date);

      if (from_date > to_date)
      {
        cout << "From date should be earlier than to date" << endl;
      }
      else
      {
        cout << "Entry\tDate\t\tAmount\t\tCategory\tDescription" << endl;
        for (const auto &entry : expenses)
        {
          const auto& expense = entry.second;

          auto temp_date = expense.expense_date;
          auto temp_time = mktime(&temp_date);
          if (temp_time >= from_date && temp_time <= to_date)
          {
            cout << entry.first << "\t"
                 << GetDateString(expense.expense_date) << "\t"
                 << expense.amount << "\t\t"
                 << GetEnumString(kExpenseCatStrEnums, expense.category) << "\t\t"
                 << expense.description << endl;
          }
        }
      }
    }
    else
    {
      auto tm_date = ReadDate(" date");
      auto date = mktime(&tm_date);

      cout << "Entry\tDate\t\tAmount\t\tCategory\tDescription" << endl;
      for (const auto &entry : expenses)
      {
        const auto& expense = entry.second;

        auto temp_date = expense.expense_date;
        if (mktime(&temp_date) == date)
        {
          cout << entry.first << "\t"
               << GetDateString(expense.expense_date) << "\t"
               << expense.amount << "\t\t"
               << GetEnumString(kExpenseCatStrEnums, expense.category) << "\t\t"
               << expense.description << endl;
        }
      }
    }
  };

public:
  void Repl()
  {
    string command;

    while (true)
    {
      cout << "$ ";
      getline(cin, command);

      if (command == "exit")
      {
        cout << "Exiting CppExpense" << endl;
        break;
      }
      else if (command == "add")
      {
        AddExpense();
      }
      else if (command == "list")
      {
        ListExpenses();
      }
      else if (command == "total")
      {
        ShowTotal();
      }
      else if (command == "search")
      {
        SearchExpenses();
      }
      else if (command == "help")
      {
        PrintHelp();
      }
      else
      {
        cout << "Unknown command " << command << ". Use help for a list of commands" << endl;
      }
    }
  }

  void PrintAbout()
  {
    string version_text = "Version 0.1.0";
    string about_text = "CppExpense: Expense Tracker";

    cout << about_text << endl
         << version_text << endl;
  }

  void PrintHelp()
  {
    string help_text = "CppExpense: Expense Tracker";

    cout << help_text << endl;
  }
};

int main(int argc, char *argv[])
{

  if (argc > 1)
  {
    cerr << "Usage: " << argv[0] << endl;
    return -1;
  }

  ExpenseTracker expense_tracker;
  expense_tracker.PrintAbout();

  expense_tracker.Repl();

  return 0;
}
