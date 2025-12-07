#include "../date.hpp"

#include <DateUtil.hpp> // from spi_util

Date::Date()
    :
    m_date(),
    m_year(),
    m_month(),
    m_day()
{}

Date::Date(int date)
    :
    m_date(date),
    m_year(),
    m_month(),
    m_day()
{
    set_ymd();
}

Date::Date(int year, int month, int day)
    :
    m_date(),
    m_year(year),
    m_month(month),
    m_day(day)
{
    m_date = spi_util::YMDToDate(m_year, m_month, m_day);
}

Date::Date(const std::string& str)
    :
    m_date(),
    m_year(),
    m_month(),
    m_day()
{
    m_date = spi_util::StringToDate(str);
    set_ymd();
}

Date::operator int() const
{
    return m_date;
}

int Date::year() const
{
    return 0;
}

int Date::month() const
{
    return 0;
}

int Date::day() const
{
    return 0;
}

void Date::set_ymd()
{
    spi_util::DateToYMD(m_date, &m_year, &m_month, &m_day);
}
