#ifndef TEST_INNER_INNER_DATE_HPP
#define TEST_INNER_INNER_DATE_HPP

#include <string>
#include <vector>

class IDate
{
public:
    IDate();
    IDate(int date);
    IDate(int year, int month, int day);
    IDate(const std::string& str);

    operator int() const;
    int year() const;
    int month() const;
    int day() const;

private:
    int m_date;
    int m_year;
    int m_month;
    int m_day;

    void set_ymd();
};

#endif
