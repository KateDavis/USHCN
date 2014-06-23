//--------------------------------------------------------------------------------------
// USHCN.h
// Code for parsing and processing daily USHCN data from
// http://cdiac.ornl.gov/ftp/ushcn_daily
// Written by Steve Goddard
// If you modify it and mess it up, don't blame it on me

#ifndef USHCN_H_INCLUDED
#define USHCN_H_INCLUDED


#include <vector>
#include <string>
#include <map>

// Comment out the next two lines to compile on MS compilers
#include <stdlib.h>
#include <limits.h>

static const unsigned int   MAX_DAYS_IN_MONTH = 31;
static const unsigned int   NUMBER_OF_MONTHS_PER_YEAR = 12;
static const unsigned int   FIRST_YEAR = 1850;
static const unsigned int   NUMBER_OF_YEARS = 200;
static const unsigned int   MAX_YEARS = 3000;
static const unsigned int   MAX_STATE_NAME_LENGTH = 64;
static const unsigned int   NUMBER_OF_STATES = 48;
static const float          UNKNOWN_TEMPERATURE = -99.0f;
static const float          UNREASONABLE_HIGH_TEMPERATURE = 140.0f;
static const float          UNREASONABLE_LOW_TEMPERATURE = -100.0f;
static const unsigned int   NUMBER_OF_DAYS_PER_YEAR = 365;
static const int            NUMBER_OF_MONTHS_UNDER_TEST = 12;

static const char STATE_NAMES[][MAX_STATE_NAME_LENGTH] =
{
    "None",
    "Alabama",
    "Arizona",
    "Arkansas",
    "California",
    "Colorado",
    "Connecticut",
    "Delaware",
    "Florida",
    "Georgia",
    "Idaho",
    "Illinois",
    "Indiana",
    "Iowa",
    "Kansas",
    "Kentucky",
    "Louisiana",
    "Maine",
    "Maryland",
    "Massachusetts",
    "Michigan",
    "Minnesota",
    "Mississippi",
    "Missouri",
    "Montana",
    "Nebraska",
    "Nevada",
    "New Hampshire",
    "New Jersey",
    "New Mexico",
    "New York",
    "North Carolina",
    "North Dakota",
    "Ohio",
    "Oklahoma",
    "Oregon",
    "Pennsylvania",
    "Rhode Island",
    "South Carolina",
    "South Dakota",
    "Tennessee",
    "Texas",
    "Utah",
    "Vermont",
    "Virginia",
    "Washington",
    "West Virginia",
    "Wisconsin",
    "Wyoming"
};


class DataRecord 
{
public:
    enum RECORD_TYPE
    {
        RECORD_TYPE_TMAX,
        RECORD_TYPE_TMIN,
        RECORD_TYPE_SNOW,
        RECORD_TYPE_SNWD,
        RECORD_TYPE_PRCP,
        RECORD_TYPE_NONE
    };

                            DataRecord() : 
                                            m_daily_high_temperature_vector(MAX_DAYS_IN_MONTH),
                                            m_daily_low_temperature_vector(MAX_DAYS_IN_MONTH)
                            {
                                for (size_t i = 0; i < MAX_DAYS_IN_MONTH; i++)
                                {
                                    setHighTemperature(i, UNKNOWN_TEMPERATURE);
                                    setLowTemperature(i, UNKNOWN_TEMPERATURE);
                                }
                            }

    unsigned int            getStationNumber() { return m_station_number; }
    void                    setStationNumber(unsigned int value) { m_station_number = value; }
    std::string&            getRecordTypeString() { return m_record_type_string; }
    void                    setRecordTypeString(std::string type) { m_record_type_string = type; }
    unsigned int            getStateNumber() { return m_state_number; }
    void                    setStateNumber(unsigned int value) { m_state_number = value; }
    std::string             getStateName() { return std::string( STATE_NAMES[ getStateNumber() ] ); }
    unsigned int            getYear() { return m_year; }
    void                    setYear(unsigned int value) { m_year = value; }
    unsigned int            getMonth() { return m_month; }
    void                    setMonth(unsigned int value) { m_month = value; }
    std::vector<float>&     getDailyHighTemperatureVector() { return m_daily_high_temperature_vector; }
    float                   getHighTemperature(unsigned int day_of_month) { return getDailyHighTemperatureVector().at(day_of_month); }
    void                    setHighTemperature(unsigned int day_of_month, float value);
    std::vector<float>&     getDailyLowTemperatureVector() { return m_daily_high_temperature_vector; }
    float                   getLowTemperature(unsigned int day_of_month) { return getDailyLowTemperatureVector().at(day_of_month); }
    void                    setLowTemperature(unsigned int day_of_month, float value);


    void                    parseTemperatureRecord(std::string record_string);


protected:
    unsigned int            m_station_number;
    std::string             m_record_type_string;
    unsigned int            m_state_number;
    unsigned int            m_year;
    unsigned int            m_month;
    std::vector<float>      m_daily_high_temperature_vector;
    std::vector<float>      m_daily_low_temperature_vector;
};

class Day
{
public:
                            Day()
                            {
                                setMaxTemperature( UNKNOWN_TEMPERATURE );
                                setMinTemperature( UNKNOWN_TEMPERATURE );
                            }

    float                   getMaxTemperature() { return m_max_temperature; }
    void                    setMaxTemperature(float value) { m_max_temperature = value; }
    float                   getMinTemperature() { return m_min_temperature; }
    void                    setMinTemperature(float value) { m_min_temperature = value; }

protected:
    float                   m_max_temperature;
    float                   m_min_temperature;
};

class Month
{
public:
                            Month() : m_day_vector(MAX_DAYS_IN_MONTH)
                            {
                                setRecordMaxTemperature( float(INT_MIN) );
                                setRecordMinTemperature( float(INT_MAX) );
                                setRecordMaxDay(0);
                                setRecordMinDay(0);
                                setValid(false);
                            }

    bool                    getValid() { return m_valid; }
    void                    setValid(bool flag) { m_valid = flag; }
    std::vector<Day>&       getDayVector() { return m_day_vector; }
    float                   getRecordMaxTemperature() { return m_record_max_temperature; }
    void                    setRecordMaxTemperature(float value) { m_record_max_temperature = value; }
    float                   getRecordMinTemperature() { return m_record_min_temperature; }
    void                    setRecordMinTemperature(float value) { m_record_min_temperature = value; }
    unsigned int            getRecordMaxDay() { return m_record_max_day; }
    void                    setRecordMaxDay(unsigned int value) { m_record_max_day = value; }
    unsigned int            getRecordMinDay() { return m_record_min_day; }
    void                    setRecordMinDay(unsigned int value) { m_record_min_day = value; }
    float                   getTotalTemperature() { return m_total_temperature; }
    void                    setTotalTemperature(float value) { m_total_temperature = value; }
    void                    addToTotalTemperature(float value) { m_total_temperature += value; }
    unsigned int            getNumberOfTemperatures() { return m_number_of_temperatures; }
    void                    setNumberOfTemperatures(unsigned int value) { m_number_of_temperatures = value; }
    void                    incrementNumberOfTemperatures() { m_number_of_temperatures++; }

protected:
    bool                    m_valid;
    std::vector<Day>        m_day_vector;
    float                   m_record_max_temperature;
    float                   m_record_min_temperature;
    unsigned int            m_record_max_day;
    unsigned int            m_record_min_day;
    float                   m_total_temperature;
    unsigned int            m_number_of_temperatures;
};

class Year
{
public:
                            Year() : m_month_vector(NUMBER_OF_MONTHS_PER_YEAR)
                            {
                                setRecordMaxTemperature( float(INT_MIN) );
                                setRecordMinTemperature( float(INT_MAX) );
                                setRecordMaxMonth(0);
                                setRecordMinMonth(0);
                            }

    std::vector<Month>&     getMonthVector() { return m_month_vector; }
    unsigned int            getYear() { return m_year; }
    void                    setYear(unsigned int value) { m_year = value; }
    float                   getRecordMaxTemperature() { return m_record_max_temperature; }
    void                    setRecordMaxTemperature(float value) { m_record_max_temperature = value; }
    float                   getRecordMinTemperature() { return m_record_min_temperature; }
    void                    setRecordMinTemperature(float value) { m_record_min_temperature = value; }
    unsigned int            getRecordMaxMonth() { return m_record_max_month; }
    void                    setRecordMaxMonth(unsigned int value) { m_record_max_month = value; }
    unsigned int            getRecordMinMonth() { return m_record_min_month; }
    void                    setRecordMinMonth(unsigned int value) { m_record_min_month = value; }
    float                   getTotalTemperature() { return m_total_temperature; }
    void                    setTotalTemperature(float value) { m_total_temperature = value; }
    void                    addToTotalTemperature(float value) { m_total_temperature += value; }
    unsigned int            getNumberOfTemperatures() { return m_number_of_temperatures; }
    void                    setNumberOfTemperatures(unsigned int value) { m_number_of_temperatures = value; }
    void                    incrementNumberOfTemperatures() { m_number_of_temperatures++; }

protected:
    std::vector<Month>      m_month_vector;
    unsigned int            m_year;
    float                   m_record_max_temperature;
    float                   m_record_min_temperature;
    unsigned int            m_record_max_month;
    unsigned int            m_record_min_month;
    float                   m_total_temperature;
    unsigned int            m_number_of_temperatures;
};

class Station
{
public:
                            Station() 
                            {
                                setRecordMaxTemperature( float(INT_MIN) );
                                setRecordMinTemperature( float(INT_MAX) );
                                setRecordMaxYear(0);
                                setRecordMinYear(0);
                            }

                            std::vector<Year>&      getYearVector() { return m_year_vector; }
    unsigned int            getStationNumber() { return m_station_number; }
    void                    setStationNumber(unsigned int value) { m_station_number = value; }
    std::string&            getStationName() { return m_station_name; }
    void                    setStationName(std::string name) { m_station_name = name; }
    std::string&            getStateName() { return m_state_name; }
    void                    setStateName(std::string name) { m_state_name = name; }
    float                   getRecordMaxTemperature() { return m_record_max_temperature; }
    void                    setRecordMaxTemperature(float value) { m_record_max_temperature = value; }
    float                   getRecordMinTemperature() { return m_record_min_temperature; }
    void                    setRecordMinTemperature(float value) { m_record_min_temperature = value; }
    unsigned int            getRecordMaxYear() { return m_record_max_year; }
    void                    setRecordMaxYear(unsigned int value) { m_record_max_year = value; }
    unsigned int            getRecordMinYear() { return m_record_min_year; }
    void                    setRecordMinYear(unsigned int value) { m_record_min_year = value; }

protected:
    std::vector<Year>       m_year_vector;
    unsigned int            m_station_number;
    std::string             m_station_name;
    std::string             m_state_name;
    float                   m_record_max_temperature;
    float                   m_record_min_temperature;
    unsigned int            m_record_max_year;
    unsigned int            m_record_min_year;
};

class State
{
public:
                            State() 
                            {
                                setRecordMaxTemperature( float(INT_MIN) );
                                setRecordMinTemperature( float(INT_MAX) );
                                setRecordMaxYear(0);
                                setRecordMinYear(0);
                            }

    std::vector<Station>&   getStationVector() { return m_station_vector; }
    unsigned int            getStateNumber() { return m_state_number; }
    void                    setStateNumber(unsigned int value) { m_state_number = value; }
    std::string             getStateName() { return std::string( STATE_NAMES[ getStateNumber() ] ); }
    float                   getRecordMaxTemperature() { return m_record_max_temperature; }
    void                    setRecordMaxTemperature(float value) { m_record_max_temperature = value; }
    float                   getRecordMinTemperature() { return m_record_min_temperature; }
    void                    setRecordMinTemperature(float value) { m_record_min_temperature = value; }
    unsigned int            getRecordMaxYear() { return m_record_max_year; }
    void                    setRecordMaxYear(unsigned int value) { m_record_max_year = value; }
    unsigned int            getRecordMinYear() { return m_record_min_year; }
    void                    setRecordMinYear(unsigned int value) { m_record_min_year = value; }

protected:
    std::vector<Station>    m_station_vector;
    unsigned int            m_state_number;
    float                   m_record_max_temperature;
    float                   m_record_min_temperature;
    unsigned int            m_record_max_year;
    unsigned int            m_record_min_year;
};

class Country
{
public:
                            Country() : m_state_vector(NUMBER_OF_STATES)
                            {
                                setRecordMaxTemperature( float(INT_MIN) );
                                setRecordMinTemperature( float(INT_MAX) );
                                setRecordMaxYear(0);
                                setRecordMinYear(0);
                            }

    std::map<float,size_t>& getVariableMonthMeanAverageMap() { return m_variable_month_mean_average_map; }
    std::map<float,size_t>& getVariableMonthMeanMaximumMap() { return m_variable_month_mean_maximum_map; }
    std::map<float,size_t>& getVariableMonthMeanMinimumMap() { return m_variable_month_mean_minimum_map; }
    std::vector<State>&     getStateVector() { return m_state_vector; }
    float                   getRecordMaxTemperature() { return m_record_max_temperature; }
    void                    setRecordMaxTemperature(float value) { m_record_max_temperature = value; }
    float                   getRecordMinTemperature() { return m_record_min_temperature; }
    void                    setRecordMinTemperature(float value) { m_record_min_temperature = value; }
    unsigned int            getRecordMaxYear() { return m_record_max_year; }
    void                    setRecordMaxYear(unsigned int value) { m_record_max_year = value; }
    unsigned int            getRecordMinYear() { return m_record_min_year; }
    void                    setRecordMinYear(unsigned int value) { m_record_min_year = value; }

protected:
    std::map<float,size_t>  m_variable_month_mean_average_map;
    std::map<float,size_t>  m_variable_month_mean_maximum_map;
    std::map<float,size_t>  m_variable_month_mean_minimum_map;
    std::vector<State>      m_state_vector;
    float                   m_record_max_temperature;
    float                   m_record_min_temperature;
    unsigned int            m_record_max_year;
    unsigned int            m_record_min_year;
};

#endif // USHCN_H_INCLUDED



