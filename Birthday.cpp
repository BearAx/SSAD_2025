#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Date
{
protected:
    int day, month, year;

    bool isLeapYear(int y) const
    {
        return (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
    }

    int daysInMonth(int m, int y) const
    {
        if (m == 2)
        {
            return isLeapYear(y) ? 29 : 28;
        }
        if (m == 4 || m == 6 || m == 9 || m == 11)
        {
            return 30;
        }
        return 31;
    }

public:
    Date(int d, int m, int y) : day(d), month(m), year(y) {}

    int getYear() const { return year; }
    int getMonth() const { return month; }
    int getDay() const { return day; }

    bool isValid() const
    {
        if (year <= 0 || month < 1 || month > 12 || day < 1) {
            return false;
        }
        return day <= daysInMonth(month, year);
    }

    // Function to calculate the number of days between two dates
    int daysFromStart() const
    {
        int totalDays = 0;

        // Add days for the years before the current year
        for (int i = 1; i < year; ++i)
        {
            totalDays += isLeapYear(i) ? 366 : 365;
        }

        // Add days for the months in the current year before the current month
        for (int i = 1; i < month; ++i)
        {
            totalDays += daysInMonth(i, year);
        }

        // Add the days in the current month
        totalDays += day;

        return totalDays;
    }

    // Function to calculate the difference in days between two dates
    int operator-(const Date& other) const
    {
        return abs(daysFromStart() - other.daysFromStart());
    }
};

// Function for sorting dates
bool cmp(const Date& a, const Date& b)
{
    if (a.getYear() != b.getYear())
    {
        return a.getYear() < b.getYear();
    }
    if (a.getMonth() != b.getMonth())
    {
        return a.getMonth() < b.getMonth();
    }
    return a.getDay() < b.getDay();
}

int main() {
    int d1, m1, y1, d2, m2, y2, d3, m3, y3;
    cin >> d1 >> m1 >> y1 >> d2 >> m2 >> y2 >> d3 >> m3 >> y3;

    Date a(d1, m1, y1);
    Date b(d2, m2, y2);
    Date c(d3, m3, y3);

    vector<Date> v = { a, b, c };

    if (!a.isValid() || !b.isValid() || !c.isValid()) {
        cout << "Invalid Input" << endl;
        return 0;
    }

    sort(v.begin(), v.end(), cmp);

    cout << v[2] - v[0] << endl;
    return 0;
}
