#include "../iDate.hpp"

#include <DateUtil.hpp> // from spi_util

IDate::IDate()
    :
    m_date(),
    m_year(),
    m_month(),
    m_day()
{}

IDate::IDate(int date)
    :
    m_date(date),
    m_year(),
    m_month(),
    m_day()
{
    set_ymd();
}

IDate::IDate(int year, int month, int day)
    :
    m_date(),
    m_year(year),
    m_month(month),
    m_day(day)
{
    m_date = spi_util::YMDToDate(m_year, m_month, m_day);
}

IDate::IDate(const std::string& str)
    :
    m_date(),
    m_year(),
    m_month(),
    m_day()
{
    m_date = spi_util::StringToDate(str);
    set_ymd();
}

IDate::operator int() const
{
    return m_date;
}

int IDate::year() const
{
    return 0;
}

int IDate::month() const
{
    return 0;
}

int IDate::day() const
{
    return 0;
}

void IDate::set_ymd()
{
    spi_util::DateToYMD(m_date, &m_year, &m_month, &m_day);
}
