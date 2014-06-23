//--------------------------------------------------------------------------------------
// USHCN.cpp
// Code for parsing and processing daily USHCN data from
// http://cdiac.ornl.gov/ftp/ushcn_daily
// Written by Steve Goddard
// If you modify it and mess it up, don't blame it on me

#include <iostream>
#include "USHCN.h"

extern size_t most_recent_year;

void
DataRecord::setHighTemperature(unsigned int day_of_month, float value)
{
    getDailyHighTemperatureVector()[day_of_month] = value;
}

void
DataRecord::setLowTemperature(unsigned int day_of_month, float value)
{
    getDailyLowTemperatureVector()[day_of_month] = value;
}

void                    
DataRecord::parseTemperatureRecord(std::string record_string)
{
    /*
    Variable        Columns         Type
    COOP ID         1-6         Character
    YEAR            7-10        Integer
    MONTH           11-12       Integer
    ELEMENT         13-16       Character
    VALUE1          17-21       Integer
    MFLAG1          22      Character
    QFLAG1          23      Character
    SFLAG1          24      Character
    VALUE2          25-29       Integer
    MFLAG2          30      Character
    QFLAG2          31      Character
    SFLAG2          32      Character
    */

    size_t position = 0;

    if (record_string.length() < 261)
    {
        return;
    }

    unsigned int station_number = strtoul( record_string.substr(0, 6).c_str(), NULL, 10 );
    setStationNumber(station_number);
    unsigned int state_number = strtoul( record_string.substr(0, 2).c_str(), NULL, 10 );
    setStateNumber(state_number);
    //std::string state_name = getStateName();
    unsigned int year = strtoul( record_string.substr(6, 4).c_str(), NULL, 10 );

    if (year > most_recent_year)
    {
        most_recent_year = year;
    }

    setYear(year);
    unsigned int month = strtoul( record_string.substr(10, 2).c_str(), NULL, 10 );
    setMonth(month);
    std::string record_type = record_string.substr(12, 4);
    setRecordTypeString(record_type);

    position = 16;

    for (size_t i = 0; i < MAX_DAYS_IN_MONTH; i++)
    {
        std::string value_string =  record_string.substr(position, 5);
        long value = strtol( value_string.c_str(), NULL, 10 );

        if ( getRecordTypeString() == "TMIN" ) 
        { 
            getDailyLowTemperatureVector()[i] = (value == -9999) ? UNKNOWN_TEMPERATURE : float(value); 
        }
        else if ( getRecordTypeString() == "TMAX" ) 
        { 
            getDailyHighTemperatureVector()[i] = (value == -9999) ? UNKNOWN_TEMPERATURE : float(value); 
        }

        position += 8;
    }
}
