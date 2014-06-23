//--------------------------------------------------------------------------------------
// Main.cpp
// Code for parsing and processing daily USHCN data from
// http://cdiac.ornl.gov/ftp/ushcn_daily
// Written by Steve Goddard
// If you modify it and mess it up, don't blame it on me

// The algorthim for locating records has no time bias.
// All years which share in the min or max record get counted.
// You can run the algorithm forwards, backwards or randomly
// and you will always get the same result.

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

#include "USHCN.h"

std::map<size_t, bool> months_under_test_map;
size_t most_recent_year = 0;

void parseUSHCN_2(std::string record_string, std::ifstream& ushcn_data_file, std::string input_file_name_string, size_t month_under_test, size_t months_under_test, int number_of_months_for_sequential_statistics)
{
    Country US;
    std::vector<State>& state_vector = US.getStateVector();

    float total_monthly_temperature_sum[MAX_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
    float average_monthly_temperature[MAX_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
    float yearly_temperature[MAX_YEARS];
    unsigned int number_of_monthly_temperature_records[MAX_YEARS][NUMBER_OF_MONTHS_PER_YEAR];

    float total_fabricated_monthly_temperature_sum[MAX_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
    float average_fabricated_monthly_temperature[MAX_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
    float fabricated_yearly_temperature[MAX_YEARS];
    unsigned int number_of_fabricated_monthly_temperature_records[MAX_YEARS][NUMBER_OF_MONTHS_PER_YEAR];

    float total_non_fabricated_monthly_temperature_sum[MAX_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
    float average_non_fabricated_monthly_temperature[MAX_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
    float non_fabricated_yearly_temperature[MAX_YEARS];
    unsigned int number_of_non_fabricated_monthly_temperature_records[MAX_YEARS][NUMBER_OF_MONTHS_PER_YEAR];

    size_t first_month = month_under_test;
    size_t last_month = month_under_test + months_under_test - 1;

    for (size_t i = 0; i < MAX_YEARS; i++)
    {
        yearly_temperature[i] = 0.0f;

        for (size_t j = 0; j < NUMBER_OF_MONTHS_PER_YEAR; j++)
        {
            total_monthly_temperature_sum[i][j] = 0.0f;
            number_of_monthly_temperature_records[i][j] = 0;
            average_monthly_temperature[i][j] = UNKNOWN_TEMPERATURE;
            total_fabricated_monthly_temperature_sum[i][j] = 0.0f;
            number_of_fabricated_monthly_temperature_records[i][j] = 0;
            average_fabricated_monthly_temperature[i][j] = UNKNOWN_TEMPERATURE;
            total_non_fabricated_monthly_temperature_sum[i][j] = 0.0f;
            number_of_non_fabricated_monthly_temperature_records[i][j] = 0;
            average_non_fabricated_monthly_temperature[i][j] = UNKNOWN_TEMPERATURE;
        }
    }

    unsigned int type_flag;
    std::string current_state_name = "";

    if ( record_string.substr(0, 3) == "USH" )
    {
        do
        {
            unsigned int station_number = strtoul( record_string.substr(5, 6).c_str(), NULL, 10 );
            unsigned int state_number = strtoul( record_string.substr(5, 2).c_str(), NULL, 10 );
            std::string state_name = STATE_NAMES[state_number];

            if (state_name != current_state_name)
            {
                std::cout << state_name << std::endl;
                //std::cerr << state_name << std::endl;
                current_state_name = state_name;
            }

            unsigned int year = strtoul( record_string.substr(12, 4).c_str(), NULL, 10 );

            if (year > most_recent_year)
            {
                most_recent_year = year;
            }

            size_t position = 16;

            for (size_t month = 0; month < NUMBER_OF_MONTHS_PER_YEAR; month++)
            {
                if ( ( month_under_test && !months_under_test_map[month + 1] ) || ( record_string.substr(position, 6) == " -9999" ) )
                {
                    position += 9;
                    continue;
                }

                float temperature = (float)( strtol( record_string.substr(position, 6).c_str(), NULL, 10 ) ) / 100.0f;
                temperature = (temperature * 1.8f) + 32;
                position += 6;

                std::string dmflag = record_string.substr(position, 1);

                total_monthly_temperature_sum[year][month] += temperature;
                number_of_monthly_temperature_records[year][month]++;

                if (dmflag == "E")
                {
                	//std::cerr << "found fabricated temp" << std::endl;
                	total_fabricated_monthly_temperature_sum[year][month] += temperature;
                	number_of_fabricated_monthly_temperature_records[year][month]++;
                }
                else
                {
                	//std::cerr << "found fabricated temp" << std::endl;
                	total_non_fabricated_monthly_temperature_sum[year][month] += temperature;
                	number_of_non_fabricated_monthly_temperature_records[year][month]++;
                }

                position += 3;
            }

            getline(ushcn_data_file, record_string);
        } while ( ushcn_data_file.good() );
    }
    else
    {
        do
        {
            unsigned int station_number = strtoul( record_string.substr(0, 6).c_str(), NULL, 10 );
            unsigned int state_number = strtoul( record_string.substr(0, 2).c_str(), NULL, 10 );
            std::string state_name = STATE_NAMES[state_number];

            if (state_name != current_state_name)
            {
                std::cout << state_name << std::endl;
                //std::cerr << state_name << std::endl;
                current_state_name = state_name;
            }

            type_flag = strtoul( record_string.substr(6, 1).c_str(), NULL, 10 );
            unsigned int year = strtoul( record_string.substr(7, 4).c_str(), NULL, 10 );

            if (year > most_recent_year)
            {
                most_recent_year = year;
            }

            size_t position = 12;


            for (size_t month = 0; month < NUMBER_OF_MONTHS_PER_YEAR; month++)
            {
                if ( ( month_under_test && !months_under_test_map[month + 1] ) || ( record_string.substr(position, 5) == "-9999" ) )
                {
                    position += 7;
                    continue;
                }

                float temperature = (float)( strtol( record_string.substr(position, 5).c_str(), NULL, 10 ) ) / 10.0f;

                total_monthly_temperature_sum[year][month] += temperature;
                number_of_monthly_temperature_records[year][month]++;

                position += 7;
            }

            getline(ushcn_data_file, record_string);
        } while ( ushcn_data_file.good() );
    }


    std::cout << input_file_name_string << std::endl;

    if (month_under_test)
    {
        for (size_t month = 1; month <= NUMBER_OF_MONTHS_PER_YEAR; month++)
        {
            if (months_under_test_map[month] ) 
            {
                switch (month)
                {
                    case 1 : std::cout << "January" << " "; break;
                    case 2 : std::cout << "February" << " "; break;
                    case 3 : std::cout << "March" << " "; break;
                    case 4 : std::cout << "April" << " "; break;
                    case 5 : std::cout << "May" << " "; break;
                    case 6 : std::cout << "June" << " "; break;
                    case 7 : std::cout << "July" << " "; break;
                    case 8 : std::cout << "August" << " "; break;
                    case 9 : std::cout << "September" << " "; break;
                    case 10 : std::cout << "October" << " "; break;
                    case 11 : std::cout << "November" << " "; break;
                    case 12 : std::cout << "December" << " "; break;
                    default : break;
                }
            }
        }
    }
    else
    {
        std::cout << "Annual mean temperature";
    }

    std::cout << "Year,Temperature,#Months,Count,Year,Fabricated temperature,#Months,Fabricated count,Year,NonFabricated temperature,#Months,NonFabricated count" << std::endl;

    std::cout << std::endl;

    for (unsigned int year = 0; year < MAX_YEARS; year++)
    {
        float monthly_sum = 0.0f;
        unsigned int monthly_count = 0;
        size_t yearly_count = 0;
        float fabricated_monthly_sum = 0.0f;
        unsigned int fabricated_monthly_count = 0;
        size_t fabricated_yearly_count = 0;
        float non_fabricated_monthly_sum = 0.0f;
        unsigned int non_fabricated_monthly_count = 0;
        size_t non_fabricated_yearly_count = 0;

        for (unsigned int month = 0; month < NUMBER_OF_MONTHS_PER_YEAR; month++)
        {
            if ( number_of_monthly_temperature_records[year][month] )
            {
                monthly_sum += total_monthly_temperature_sum[year][month];
                monthly_count += number_of_monthly_temperature_records[year][month];
                yearly_count += monthly_count;
                average_monthly_temperature[year][month] = total_monthly_temperature_sum[year][month] / float( number_of_monthly_temperature_records[year][month] );
            }

            if ( number_of_fabricated_monthly_temperature_records[year][month] )
            {
            	fabricated_monthly_sum += total_fabricated_monthly_temperature_sum[year][month];
            	fabricated_monthly_count += number_of_fabricated_monthly_temperature_records[year][month];
            	fabricated_yearly_count += fabricated_monthly_count;
                average_fabricated_monthly_temperature[year][month] = total_fabricated_monthly_temperature_sum[year][month] / float( number_of_fabricated_monthly_temperature_records[year][month] );
            }

            if ( number_of_non_fabricated_monthly_temperature_records[year][month] )
            {
            	non_fabricated_monthly_sum += total_non_fabricated_monthly_temperature_sum[year][month];
            	non_fabricated_monthly_count += number_of_non_fabricated_monthly_temperature_records[year][month];
            	non_fabricated_yearly_count += non_fabricated_monthly_count;
                average_non_fabricated_monthly_temperature[year][month] = total_non_fabricated_monthly_temperature_sum[year][month] / float( number_of_non_fabricated_monthly_temperature_records[year][month] );
            }
        }

		if (monthly_count)
		{
			float average_temperature = monthly_sum / float(monthly_count);
			yearly_temperature[year] = average_temperature;

			float sum = 0.0f;
			int number_of_months_with_valid_data = 0;;

			for (int month = 0; month < 12; month++)
			{
				if ( number_of_monthly_temperature_records[year][month] )
				{
					sum += average_monthly_temperature[year][month];
					number_of_months_with_valid_data++;
				}
			}

			average_temperature = sum / (float)number_of_months_with_valid_data;

			std::cout << year << "," << average_temperature;
			std::cout << "," << number_of_months_with_valid_data;
			std::cout << "," << yearly_count;
		}

		if (fabricated_monthly_count)
		{
			float average_fabricated_temperature = fabricated_monthly_sum / float(fabricated_monthly_count);
			fabricated_yearly_temperature[year] = average_fabricated_temperature;

			float sum = 0.0f;
			int number_of_months_with_valid_fabricated_data = 0;

			for (int month = 0; month < 12; month++)
			{
				if ( number_of_fabricated_monthly_temperature_records[year][month] )
				{
					sum += average_fabricated_monthly_temperature[year][month];
					number_of_months_with_valid_fabricated_data++;
				}
			}

			average_fabricated_temperature = sum / (float)number_of_months_with_valid_fabricated_data;

			std::cout << "," << year << "," << average_fabricated_temperature;
			std::cout << "," << number_of_months_with_valid_fabricated_data;
			std::cout << "," << fabricated_yearly_count;
		}

		if (non_fabricated_monthly_count)
		{
			float average_non_fabricated_temperature = non_fabricated_monthly_sum / float(non_fabricated_monthly_count);
			non_fabricated_yearly_temperature[year] = average_non_fabricated_temperature;

			float sum = 0.0f;
			int number_of_months_with_valid_non_fabricated_data = 0;

			for (int month = 0; month < 12; month++)
			{
				if ( number_of_non_fabricated_monthly_temperature_records[year][month] )
				{
					sum += average_non_fabricated_monthly_temperature[year][month];
					number_of_months_with_valid_non_fabricated_data++;
				}
			}

			average_non_fabricated_temperature = sum / (float)number_of_months_with_valid_non_fabricated_data;

			std::cout << "," << year << "," << average_non_fabricated_temperature;
			std::cout << "," << number_of_months_with_valid_non_fabricated_data;
			std::cout << "," << non_fabricated_yearly_count;
		}

		if (monthly_count || fabricated_monthly_count || non_fabricated_monthly_count)
		{
			std::cout << std::endl;
		}
    }

    for (unsigned int year = 0; year < MAX_YEARS; year++)
    {
        for (unsigned int month = 0; month < NUMBER_OF_MONTHS_PER_YEAR; month++)
        {
            if ( number_of_monthly_temperature_records[year][month] )
            {
                float* month_ptr = &( average_monthly_temperature[year][month] );
                float sum = 0.0f;

                for (int i = -(number_of_months_for_sequential_statistics - 1); i <= 0; i++)
                {
                    sum += *(month_ptr + i);
                }

                float average = sum / float(number_of_months_for_sequential_statistics);
                size_t month_number = (year * NUMBER_OF_MONTHS_PER_YEAR) + month;
                US.getVariableMonthMeanAverageMap()[average] = month_number;
            }
        }
    }

    std::cout << "Hottest Maximum " << number_of_months_for_sequential_statistics << " month periods" << std::endl;
    std::cout << "Rank, " << "Month, " << "Year, " << "Temperature " << std::endl;
    std::map<float,size_t>::reverse_iterator variable_month_iterator = US.getVariableMonthMeanAverageMap().rbegin();
    size_t count = 1;

    for ( ; variable_month_iterator != US.getVariableMonthMeanAverageMap().rend(); variable_month_iterator++ )
    {
        unsigned int year = variable_month_iterator->second / NUMBER_OF_MONTHS_PER_YEAR;
        unsigned int month = variable_month_iterator->second % NUMBER_OF_MONTHS_PER_YEAR;
        float temperature = variable_month_iterator->first;

        std::cout << count++ << "," << month + 1 << "," << year << "," << temperature << std::endl;
    }
}

void parseUSHCN_2_5(std::string record_string, std::ifstream& ushcn_data_file, std::string input_file_name_string, size_t month_under_test, size_t months_under_test, int number_of_months_for_sequential_statistics)
{
    Country US;
    std::vector<State>& state_vector = US.getStateVector();
    float total_monthly_temperature_sum[MAX_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
    float average_monthly_temperature[MAX_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
    float yearly_temperature[MAX_YEARS];
    unsigned int number_of_monthly_temperature_records[MAX_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
    size_t first_month = month_under_test;
    size_t last_month = month_under_test + months_under_test - 1;

    for (size_t i = 0; i < MAX_YEARS; i++)
    {
        yearly_temperature[i] = 0.0f;

        for (size_t j = 0; j < NUMBER_OF_MONTHS_PER_YEAR; j++)
        {
            total_monthly_temperature_sum[i][j] = 0.0f;
            number_of_monthly_temperature_records[i][j] = 0;
            average_monthly_temperature[i][j] = UNKNOWN_TEMPERATURE;
        }
    }

    std::string current_state_name = "";

    do
    {
        unsigned int station_number = strtoul( record_string.substr(5, 6).c_str(), NULL, 10 );
        unsigned int state_number = strtoul( record_string.substr(5, 2).c_str(), NULL, 10 );
        std::string state_name = STATE_NAMES[state_number];

        if (state_name != current_state_name)
        {
            std::cout << state_name << std::endl;
            //std::cerr << state_name << std::endl;
            current_state_name = state_name;
        }

        unsigned int year = strtoul( record_string.substr(12, 4).c_str(), NULL, 10 );

        if (year > most_recent_year)
        {
            most_recent_year = year;
        }

        size_t position = 17;

        for (size_t month = 0; month < NUMBER_OF_MONTHS_PER_YEAR; month++)
        {
            if ( ( month_under_test && !months_under_test_map[month + 1] ) || ( record_string.substr(position, 5) == "-9999" ) )
            {
                position += 9;
                continue;
            }

            float temperature = (float)( strtol( record_string.substr(position, 5).c_str(), NULL, 10 ) ) / 100.0f;
            temperature = (temperature * 1.8f) + 32;

            total_monthly_temperature_sum[year][month] += temperature;
            number_of_monthly_temperature_records[year][month]++;

            position += 9;
        }

        getline(ushcn_data_file, record_string);
    } while ( ushcn_data_file.good() );

    std::cout << input_file_name_string << std::endl;

    if (month_under_test)
    {
        for (size_t month = 1; month <= NUMBER_OF_MONTHS_PER_YEAR; month++)
        {
            if (months_under_test_map[month] ) 
            {
                switch (month)
                {
                    case 1 : std::cout << "January" << " "; break;
                    case 2 : std::cout << "February" << " "; break;
                    case 3 : std::cout << "March" << " "; break;
                    case 4 : std::cout << "April" << " "; break;
                    case 5 : std::cout << "May" << " "; break;
                    case 6 : std::cout << "June" << " "; break;
                    case 7 : std::cout << "July" << " "; break;
                    case 8 : std::cout << "August" << " "; break;
                    case 9 : std::cout << "September" << " "; break;
                    case 10 : std::cout << "October" << " "; break;
                    case 11 : std::cout << "November" << " "; break;
                    case 12 : std::cout << "December" << " "; break;
                    default : break;
                }
            }
        }
    }
    else
    {
        std::cout << "Annual mean temperature" << std::endl;
    }

    for (unsigned int year = 0; year < MAX_YEARS; year++)
    {
        float monthly_sum = 0.0f;
        unsigned int monthly_count = 0;
        float monthly_average[NUMBER_OF_MONTHS_PER_YEAR];
        float sum_of_monthly_temperatures = 0.0f;
        size_t number_of_months = 0;

        for (unsigned int month = 0; month < NUMBER_OF_MONTHS_PER_YEAR; month++)
        {
            if ( number_of_monthly_temperature_records[year][month] )
            {
                monthly_sum += total_monthly_temperature_sum[year][month];
                monthly_count += number_of_monthly_temperature_records[year][month];
                average_monthly_temperature[year][month] = total_monthly_temperature_sum[year][month] / float( number_of_monthly_temperature_records[year][month] );
                monthly_average[month] = total_monthly_temperature_sum[year][month] / number_of_monthly_temperature_records[year][month];
                sum_of_monthly_temperatures += monthly_average[month];
                number_of_months++;
            }
        }

        if (number_of_months)
        {
            //float average_temperature = monthly_sum / float(monthly_count);
            //yearly_temperature[year] = average_temperature;
            float average_temperature = sum_of_monthly_temperatures / float(number_of_months);
            yearly_temperature[year] = average_temperature;

            std::cout << year << ", " << average_temperature << std::endl; 
        }
    }

    for (unsigned int year = 0; year < MAX_YEARS; year++)
    {
        for (unsigned int month = 0; month < NUMBER_OF_MONTHS_PER_YEAR; month++)
        {
            if ( number_of_monthly_temperature_records[year][month] )
            {
                float* month_ptr = &( average_monthly_temperature[year][month] );
                float sum = 0.0f;

                for (int i = -(number_of_months_for_sequential_statistics - 1); i <= 0; i++)
                {
                    sum += *(month_ptr + i);
                }

                float average = sum / float(number_of_months_for_sequential_statistics);
                size_t month_number = (year * NUMBER_OF_MONTHS_PER_YEAR) + month;
                US.getVariableMonthMeanAverageMap()[average] = month_number;
            }
        }
    }

    std::cout << "Hottest Maximum " << number_of_months_for_sequential_statistics << " month periods" << std::endl;
    std::cout << "Rank, " << "Month, " << "Year, " << "Temperature " << std::endl;
    std::map<float,size_t>::reverse_iterator variable_month_iterator = US.getVariableMonthMeanAverageMap().rbegin();
    size_t count = 1;

    for ( ; variable_month_iterator != US.getVariableMonthMeanAverageMap().rend(); variable_month_iterator++ )
    {
        unsigned int year = variable_month_iterator->second / NUMBER_OF_MONTHS_PER_YEAR;
        unsigned int month = variable_month_iterator->second % NUMBER_OF_MONTHS_PER_YEAR;
        float temperature = variable_month_iterator->first;

        std::cout << count++ << ", " << month + 1 << ", " << year << ", " << temperature << std::endl;
    }
}

int main (int argc, char** argv) 
{
    if (argc < 2)
    {
        std::cerr << "Usage : ushcn.exe USHCN_DATA_FILE_NAME [month=0-12] [year=YYYY] [date=MODY] [dump=MODYYEAR]" << std::endl;
        return (1);
    }

    std::string input_file_name_string = argv[1];

    size_t station_under_test = 0;
    size_t year_under_test = 0;
    size_t month_under_test = 0;
    size_t months_under_test = 1;
    size_t month_to_dump = 0;
    size_t day_to_dump = 0;
    size_t year_to_dump = 0;
    int number_of_months_for_sequential_statistics = 12;
    size_t start_year_for_comparing_records = 1930;

    for (int i = 2; i < argc; i++)
    {
        std::string argument_string = std::string( argv[i] );

        if ( argument_string.find("year=") != std::string::npos )
        {
            std::string year_string = argument_string.substr(5, 4);
            year_under_test = (size_t)strtol(year_string.c_str(), NULL, 10);
            std::cout << year_string << std::endl;
            std::cerr << year_string << std::endl;
        }
        else if ( argument_string.find("station=") != std::string::npos )
        {
            std::string station_string = argument_string.substr(8, 6);
            station_under_test = (size_t)strtol(station_string.c_str(), NULL, 10);
            std::cout << "Station " << station_under_test << std::endl;
            std::cerr << "Station " << station_under_test << std::endl;
        }
        else if ( argument_string.find("start=") != std::string::npos )
        {
            std::string year_string = argument_string.substr(6, 4);
            start_year_for_comparing_records = (size_t)strtol(year_string.c_str(), NULL, 10);
            std::cout << "Start year " << start_year_for_comparing_records << std::endl;
            std::cerr << "Start year " << start_year_for_comparing_records << std::endl;
        }
        else if ( argument_string.find("month=") != std::string::npos )
        {
            std::string month_string = argument_string.substr(6, 2);
            month_under_test = (size_t)strtol(month_string.c_str(), NULL, 10);
            months_under_test_map[month_under_test] = true;
            std::cout << "Month " << month_string << std::endl;
            std::cerr << "Month " << month_string << std::endl;
        }
        else if ( argument_string.find("months=") != std::string::npos )
        {
            std::string months_string = argument_string.substr(7, 2);
            months_under_test = (size_t)strtol(months_string.c_str(), NULL, 10);
            std::cout << "Number of months " << months_under_test << std::endl;
            std::cerr << "Number of months " << months_under_test << std::endl;

            for (size_t i = 0; i < months_under_test; i++)
            {
                size_t valid_month = ( month_under_test + i ) % 12;
                valid_month = (valid_month == 0) ? 12 : valid_month;
                months_under_test_map[valid_month] = true;
                std::cerr << "Month under test " << valid_month << std::endl;
            }
        }
        else if ( argument_string.find("period=") != std::string::npos )
        {
            size_t length = argument_string.size();
            std::string period_string = argument_string.substr(7, length - 7);
            number_of_months_for_sequential_statistics = (int)strtol(period_string.c_str(), NULL, 10);
            std::cout << period_string << std::endl;
            std::cerr << period_string << std::endl;
        }
        else if ( argument_string.find("date=") != std::string::npos )
        {
            std::string dump_date_string = argument_string.substr(5, 4);
            month_to_dump = (size_t)strtol(dump_date_string.substr(0, 2).c_str(), NULL, 10);
            day_to_dump = (size_t)strtol(dump_date_string.substr(2, 2).c_str(), NULL, 10);

            if ( argument_string.size() == 13 )
            {
                year_to_dump = (size_t)strtol(argument_string.substr(9, 4).c_str(), NULL, 10);
            }
        }
    }

    // Read in the station information
    std::map<unsigned int, std::string> station_name_map;
    std::string record_string;
    // http://cdiac.ornl.gov/ftp/ushcn_daily/ushcn-stations.txt
    std::ifstream ushcn_station_file("ushcn-stations.txt");

    if ( ushcn_station_file.is_open() )
    {
        while ( ushcn_station_file.good() )
        {
            getline(ushcn_station_file, record_string);

            if (record_string.length() < 90)
            {
                continue;
            }

            unsigned int station_number = strtoul( record_string.substr(0, 6).c_str(), NULL, 10 );
            std::string station_name = record_string.substr(36, 15);
            station_name_map[station_number] = station_name;
        }

        ushcn_station_file.close();
    }
    else 
    {
        std::cout << "Unable to open ushcn-stations.txt" << std::endl; 
    }

    // read in the station data
    // http://cdiac.ornl.gov/ftp/ushcn_daily/
    std::ifstream ushcn_data_file( input_file_name_string.c_str() );

    Country US;
    unsigned int current_station_number = 0;
    unsigned int current_state_number = 0;
    unsigned int current_year_number = 0;
    bool check_ushcn_2 = true;
    bool check_ushcn_2_5 = true;

    // Read in the temperature database
    if ( ushcn_data_file.is_open() )
    {
        while ( ushcn_data_file.good() )
        {
            getline(ushcn_data_file, record_string);

            //if ( check_ushcn_2_5 && record_string.substr(0, 3) == "USH" )
            //{
            //    parseUSHCN_2_5(record_string, ushcn_data_file, input_file_name_string, month_under_test, months_under_test, number_of_months_for_sequential_statistics);
            //    return(1);
            //}
            //else
            //{
            //    check_ushcn_2_5 = false;
            //}
            
            if (   ( check_ushcn_2_5 && record_string.substr(0, 3) == "USH" )
                || ( check_ushcn_2 && record_string.substr(6, 2) != "18" && record_string.substr(6, 2) != "19" && record_string.substr(6, 2) != "20" ) 
               )
            {
                parseUSHCN_2(record_string, ushcn_data_file, input_file_name_string, month_under_test, months_under_test, number_of_months_for_sequential_statistics);
                return(1);
            }
            else
            {
                check_ushcn_2 = false;
            }


            DataRecord record;
            record.parseTemperatureRecord(record_string);

            // Uncomment this if you want to see the station info printed as the file is parsed
#if 0
            if ( record.getYear() == most_recent_year )
            {
                std::cout << record.getStateName() << " " << record.getStationNumber() << " ";
                std::cout << station_name_map[ record.getStationNumber() ];
                std::cout << " " << record.getRecordTypeString();
                std::cout << " " << record.getMonth();
                std::cout << " " << record.getYear();
                std::cout << std::endl;
            }
#endif
            // Build the database

            // Look for a new state
            if ( record.getStateNumber() != current_state_number )
            {
                current_state_number = record.getStateNumber();
                std::cerr << record.getStateName() << std::endl;
                std::cout << record.getStateName() << std::endl;
                US.getStateVector().at(current_state_number - 1).setStateNumber(current_state_number);
            }

            // Look for a new station
            if ( record.getStationNumber() != current_station_number )
            {
                current_station_number = record.getStationNumber();
                Station new_station;
                new_station.setStationNumber(current_station_number);
                new_station.setStateName( record.getStateName() );
                new_station.setStationName( station_name_map[ record.getStationNumber() ] );
                US.getStateVector().at(current_state_number - 1).getStationVector().push_back(new_station);
            }

            // Look for a new year
            if ( record.getYear() != current_year_number )
            {
                current_year_number = record.getYear();

                if (current_year_number > most_recent_year)
                {
                    most_recent_year = current_year_number;
                }

                Year new_year;
                new_year.setYear(current_year_number);
                US.getStateVector().at(current_state_number - 1).getStationVector().back().getYearVector().push_back(new_year);
            }

            State& current_state = US.getStateVector().at(current_state_number - 1);
            Station& current_station = current_state.getStationVector().back();
            Year& current_year = current_station.getYearVector().back();
            Month& current_month = current_year.getMonthVector().at( record.getMonth() - 1 );

            // read in the TMAX and TMIN records for each day of the month
            if ( record.getRecordTypeString() == "TMAX" || record.getRecordTypeString() == "TMIN" )
            {
                for (size_t day_number = 0; day_number < MAX_DAYS_IN_MONTH; day_number++)
                {
                    float high_temperature = record.getHighTemperature(day_number);
                    float low_temperature = record.getLowTemperature(day_number);
                    Day& day = current_month.getDayVector().at(day_number);

                    if ( record.getRecordTypeString() == "TMAX" && 
                        high_temperature != UNKNOWN_TEMPERATURE && 
                        high_temperature < UNREASONABLE_HIGH_TEMPERATURE )
                    {
                        day.setMaxTemperature(high_temperature);

                        if ( high_temperature > US.getRecordMaxTemperature() )
                        {
                            US.setRecordMaxTemperature( high_temperature );
                            US.setRecordMaxYear( current_year_number );
                        }

                        if ( high_temperature > current_state.getRecordMaxTemperature() )
                        {
                            current_state.setRecordMaxTemperature( high_temperature );
                            current_state.setRecordMaxYear( current_year_number );
                        }

                        if ( high_temperature > current_station.getRecordMaxTemperature() )
                        {
                            current_station.setRecordMaxTemperature( high_temperature );
                            current_station.setRecordMaxYear( current_year_number );
                        }

                        if ( high_temperature > current_year.getRecordMaxTemperature() )
                        {
                            current_year.setRecordMaxTemperature( high_temperature );
                            current_year.setRecordMaxMonth( record.getMonth() );
                        }

                        if ( high_temperature > current_month.getRecordMaxTemperature() )
                        {
                            current_month.setRecordMaxTemperature( high_temperature );
                            current_month.setRecordMaxDay( day_number + 1 );
                        }
                    }

                    if ( record.getRecordTypeString() == "TMIN" &&
                        low_temperature != UNKNOWN_TEMPERATURE  && 
                        low_temperature > UNREASONABLE_LOW_TEMPERATURE )
                    {
                        day.setMinTemperature(low_temperature);

                        if ( low_temperature < US.getRecordMinTemperature() )
                        {
                            US.setRecordMinTemperature( low_temperature );
                            US.setRecordMinYear( current_year_number );
                        }

                        if ( low_temperature < current_state.getRecordMinTemperature() )
                        {
                            current_state.setRecordMinTemperature( low_temperature );
                            current_state.setRecordMinYear( current_year_number );
                        }

                        if ( low_temperature < current_station.getRecordMinTemperature() )
                        {
                            current_station.setRecordMinTemperature( low_temperature );
                            current_station.setRecordMinYear( current_year_number );
                        }

                        if ( low_temperature < current_year.getRecordMinTemperature() )
                        {
                            current_year.setRecordMinTemperature( low_temperature );
                            current_year.setRecordMinMonth( record.getMonth() );
                        }

                        if ( low_temperature < current_month.getRecordMinTemperature() )
                        {
                            current_month.setRecordMinTemperature( low_temperature );
                            current_month.setRecordMinDay( day_number );
                        }
                    }
                }
            }
        }

        ushcn_data_file.close();

        std::vector<State>& state_vector = US.getStateVector();
        size_t state_vector_size = state_vector.size();

        // Maps to tracks records and sums

        std::map<unsigned int, unsigned int> record_max_per_year_map;
        std::map<unsigned int, unsigned int> record_min_per_year_map;
        std::map<unsigned int, unsigned int> record_incremental_max_per_year_map;
        std::map<unsigned int, unsigned int> record_incremental_min_per_year_map;
        std::map<unsigned int, float> total_temperature_per_year_map;
        std::map<unsigned int, unsigned int> number_of_readings_per_year_map;
        std::map<unsigned int, float> total_max_temperature_per_year_map;
        std::map<unsigned int, unsigned int> number_of_max_readings_per_year_map;
        std::map<unsigned int, float> total_min_temperature_per_year_map;
        std::map<unsigned int, unsigned int> number_of_min_readings_per_year_map;
        float total_temperature_per_month[NUMBER_OF_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
        unsigned int number_of_readings_per_month[NUMBER_OF_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
        float total_max_temperature_per_month[NUMBER_OF_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
        unsigned int number_of_max_readings_per_month[NUMBER_OF_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
        float total_min_temperature_per_month[NUMBER_OF_YEARS][NUMBER_OF_MONTHS_PER_YEAR];
        unsigned int number_of_min_readings_per_month[NUMBER_OF_YEARS][NUMBER_OF_MONTHS_PER_YEAR];

        // Zero out the maps
        for (unsigned int year = FIRST_YEAR; year <= most_recent_year; year++)
        {
            record_max_per_year_map[year] = 0;
            record_min_per_year_map[year] = 0;
            record_incremental_max_per_year_map[year] = 0;
            record_incremental_min_per_year_map[year] = 0;
            total_temperature_per_year_map[year] = 0.0f;
            total_max_temperature_per_year_map[year] = 0.0f;
            total_min_temperature_per_year_map[year] = 0.0f;
            number_of_readings_per_year_map[year] = 0;
            number_of_max_readings_per_year_map[year] = 0;
            number_of_min_readings_per_year_map[year] = 0;

            for (size_t month = 0; month < NUMBER_OF_MONTHS_PER_YEAR; month++)
            {
                total_temperature_per_month[year - FIRST_YEAR][month] = 0.0f;
                number_of_readings_per_month[year - FIRST_YEAR][month] = 0;
                total_max_temperature_per_month[year - FIRST_YEAR][month] = 0.0f;
                number_of_max_readings_per_month[year - FIRST_YEAR][month] = 0;
                total_min_temperature_per_month[year - FIRST_YEAR][month] = 0.0f;
                number_of_min_readings_per_month[year - FIRST_YEAR][month] = 0;
            }
        }

        // Walk through all temperature records
        for (size_t state_number = 0; state_number < state_vector_size; state_number++)
        {
            std::vector<Station>& station_vector = state_vector.at(state_number).getStationVector();
            size_t station_vector_size = station_vector.size();

            for (size_t station_number = 0; station_number < station_vector_size; station_number++)
            {
                std::vector<Year>& year_vector = station_vector.at(station_number).getYearVector();
                size_t year_vector_size = year_vector.size();

                if ( start_year_for_comparing_records && !month_under_test && !station_under_test && !month_to_dump &&
                     (   !year_vector_size 
                       || year_vector.at(0).getYear() > start_year_for_comparing_records 
                       || year_vector.at(year_vector_size - 1).getYear() < most_recent_year 
                     )
                   )
                {
                    continue;
                }

                std::map<unsigned int, unsigned int> station_record_max_per_year_map;
                std::map<unsigned int, unsigned int> station_record_min_per_year_map;

                size_t station = station_vector.at(station_number).getStationNumber();

                if ( station_under_test && (station_under_test != station) )
                {
                    continue;
                }


                float record_max_temperatures[NUMBER_OF_MONTHS_PER_YEAR][MAX_DAYS_IN_MONTH];
                float record_min_temperatures[NUMBER_OF_MONTHS_PER_YEAR][MAX_DAYS_IN_MONTH];
                std::vector<unsigned int> record_max_temperature_year_vector[NUMBER_OF_MONTHS_PER_YEAR][MAX_DAYS_IN_MONTH];
                std::vector<unsigned int> record_min_temperature_year_vector[NUMBER_OF_MONTHS_PER_YEAR][MAX_DAYS_IN_MONTH];

                for (size_t i = 0; i < NUMBER_OF_MONTHS_PER_YEAR; i++)
                {
                    for (size_t j = 0; j < MAX_DAYS_IN_MONTH; j++)
                    {
                        record_max_temperatures[i][j] = float(INT_MIN);
                        record_min_temperatures[i][j] = float(INT_MAX);
                    }
                }

                for (size_t year_number = 0; year_number < year_vector_size; year_number++)
                {
                    std::vector<Month>& month_vector = year_vector.at(year_number).getMonthVector();
                    size_t month_vector_size = month_vector.size();
                    unsigned int year = year_vector.at(year_number).getYear();

                    if ( year_under_test && (year_under_test != year) )
                    {
                        continue;
                    }

                    for (size_t month_number = 0; month_number < month_vector_size; month_number++)
                    {
                        std::vector<Day>& day_vector = month_vector.at(month_number).getDayVector();
                        size_t day_vector_size = day_vector.size();

                        if ( month_under_test && !months_under_test_map[month_number + 1] )
                        {
                            continue;
                        }

                        for (size_t day_number = 0; day_number < day_vector_size; day_number++)
                        {
                            Day& day = day_vector.at(day_number);
                            float max_temperature = day.getMaxTemperature();
                            float min_temperature = day.getMinTemperature();

                            if ( (year_to_dump == year) && ( month_to_dump == (month_number + 1) ) && ( day_to_dump == (day_number + 1) )  &&
                                 (max_temperature != UNKNOWN_TEMPERATURE) && ( min_temperature != UNKNOWN_TEMPERATURE) )
                            {
                                std::cout << std::setw(15) << station_vector.at(station_number).getStateName() << ",  ";
                                std::cout << station_vector.at(station_number).getStationName() << ", " << month_to_dump << "/" << day_to_dump;
                                std::cout << "/" << year;
                                std::cout << ", " << std::setw(3) << max_temperature << ", " << std::setw(3) << min_temperature << std::endl;
                            }

                            // Don't use broken readings
                            if (max_temperature != UNKNOWN_TEMPERATURE  && max_temperature < UNREASONABLE_HIGH_TEMPERATURE)
                            {
                                total_temperature_per_year_map[year] = total_temperature_per_year_map[year] + max_temperature;
                                number_of_readings_per_year_map[year] = number_of_readings_per_year_map[year] + 1;
                                total_temperature_per_month[year - FIRST_YEAR][month_number] += max_temperature;
                                number_of_readings_per_month[year - FIRST_YEAR][month_number]++;
                                total_max_temperature_per_month[year - FIRST_YEAR][month_number] += max_temperature;
                                number_of_max_readings_per_month[year - FIRST_YEAR][month_number]++;
                                total_max_temperature_per_year_map[year] = total_max_temperature_per_year_map[year] + max_temperature;
                                number_of_max_readings_per_year_map[year] = number_of_max_readings_per_year_map[year] + 1;

                                if ( max_temperature == record_max_temperatures[month_number][day_number] )
                                {
                                    record_max_temperature_year_vector[month_number][day_number].push_back(year);
                                }

                                if ( max_temperature > record_max_temperatures[month_number][day_number] )
                                {
                                    record_max_temperatures[month_number][day_number] = max_temperature;
                                    record_incremental_max_per_year_map[year] = record_incremental_max_per_year_map[year] + 1;

                                    std::vector<unsigned int>& record_max_vector = record_max_temperature_year_vector[month_number][day_number];
                                    record_max_vector.erase( record_max_vector.begin(), record_max_vector.end() );
                                    record_max_vector.push_back(year);
                                }
                            }

                            if (min_temperature != UNKNOWN_TEMPERATURE  && min_temperature > UNREASONABLE_LOW_TEMPERATURE)
                            {
                                total_temperature_per_year_map[year] = total_temperature_per_year_map[year] + min_temperature;
                                number_of_readings_per_year_map[year] = number_of_readings_per_year_map[year] + 1;
                                total_temperature_per_month[year - FIRST_YEAR][month_number] += min_temperature;
                                number_of_readings_per_month[year - FIRST_YEAR][month_number]++;
                                total_min_temperature_per_month[year - FIRST_YEAR][month_number] += min_temperature;
                                number_of_min_readings_per_month[year - FIRST_YEAR][month_number]++;
                                total_min_temperature_per_year_map[year] = total_min_temperature_per_year_map[year] + min_temperature;
                                number_of_min_readings_per_year_map[year] = number_of_min_readings_per_year_map[year] + 1;

                                if ( min_temperature == record_min_temperatures[month_number][day_number] )
                                {
                                    record_min_temperature_year_vector[month_number][day_number].push_back(year);
                                }

                                if ( min_temperature < record_min_temperatures[month_number][day_number] )
                                {
                                    record_min_temperatures[month_number][day_number] = min_temperature;
                                    record_incremental_min_per_year_map[year] = record_incremental_min_per_year_map[year] + 1;

                                    std::vector<unsigned int>& record_min_vector = record_min_temperature_year_vector[month_number][day_number];
                                    record_min_vector.erase( record_min_vector.begin(), record_min_vector.end() );
                                    record_min_vector.push_back(year);
                                }
                            }
                        }
                    }
                }

                if ( year_vector.at(0).getYear() <= start_year_for_comparing_records && year_vector.at(year_vector_size - 1).getYear() >= most_recent_year )
                {
                    for (size_t i = 0; i < NUMBER_OF_MONTHS_PER_YEAR; i++)
                    {   
                        for (size_t j = 0; j < MAX_DAYS_IN_MONTH; j++)
                        {
                            if ( ( month_to_dump == (i + 1) ) && ( day_to_dump == (j + 1) ) && (year_to_dump == 0) )
                            {
                                std::cout << std::setw(15) << station_vector.at(station_number).getStateName() << ",  ";
                                std::cout << station_vector.at(station_number).getStationName() << ", " << month_to_dump << "/" << day_to_dump;
                                std::cout << ", " << std::setw(3) << record_max_temperatures[i][j] << ", ";

                                size_t size = record_max_temperature_year_vector[i][j].size();
                                size_t k = 0;

                                for ( ; k < size - 1; k++)
                                {
                                    std::cout << record_max_temperature_year_vector[i][j].at(k) << ", ";
                                }

                                std::cout << record_max_temperature_year_vector[i][j].at(k) << std::endl;

#if 0
                                std::cerr << "    " << record_min_temperatures[i][j] << " : ";

                                size = record_min_temperature_year_vector[i][j].size();

                                for (size_t k = 0; k < size; k++)
                                {
                                    std::cerr << record_min_temperature_year_vector[i][j].at(k) << ", ";
                                }

#endif
                            }

                            size_t size = record_max_temperature_year_vector[i][j].size();

                            for (size_t k = 0; k < size; k++)
                            {
                                unsigned int record_max_year = record_max_temperature_year_vector[i][j].at(k);
                                record_max_per_year_map[record_max_year] = record_max_per_year_map[record_max_year] + 1;
                                station_record_max_per_year_map[record_max_year] = station_record_max_per_year_map[record_max_year] + 1;
                            }

                            size = record_min_temperature_year_vector[i][j].size();

                            for (size_t k = 0; k < size; k++)
                            {
                                unsigned int record_min_year = record_min_temperature_year_vector[i][j].at(k);
                                record_min_per_year_map[record_min_year] = record_min_per_year_map[record_min_year] + 1;
                                station_record_min_per_year_map[record_min_year] = station_record_min_per_year_map[record_min_year] + 1;
                            }
                        }
                    }
                }
            }
        }

        size_t record_map_size = record_max_per_year_map.size();


        // Dump out the results
        std::cout << "Start year for record comparison " << start_year_for_comparing_records << std::endl;
        std::cout << "Record Maximums," << std::endl;
        std::map<unsigned int, unsigned int>::iterator itmax = record_max_per_year_map.begin();

        while ( itmax != record_max_per_year_map.end() )
        {
            std::cout << itmax->first << ", " << itmax->second << "," << std::endl;
            ++itmax;
        }

        std::cout << "Start year for record comparison " << start_year_for_comparing_records << std::endl;
        std::cout << "Record Minimums," << std::endl;
        std::map<unsigned int, unsigned int>::iterator itmin = record_min_per_year_map.begin();

        while ( itmin != record_min_per_year_map.end() )
        {
            std::cout << itmin->first << ", " << itmin->second << "," << std::endl;
            ++itmin;
        }

        std::cout << "Record Incremental Maximums," << std::endl;
        itmax = record_incremental_max_per_year_map.begin();

        while ( itmax != record_incremental_max_per_year_map.end() )
        {
            std::cout << itmax->first << ", " << itmax->second << "," << std::endl;
            ++itmax;
        }

        std::cout << "Record Incremental Minimums," << std::endl;
        itmin = record_incremental_min_per_year_map.begin();

        while ( itmin != record_incremental_min_per_year_map.end() )
        {
            std::cout << itmin->first << ", " << itmin->second << "," << std::endl;
            ++itmin;
        }

        std::cout << "Ratio Tmax/Tmin," << std::endl;
        itmax = record_max_per_year_map.begin();
        itmin = record_min_per_year_map.begin();

        while ( itmin != record_min_per_year_map.end() && itmax != record_max_per_year_map.end() )
        {
            float ratio = float(itmax->second) / float(itmin->second);
            std::cout << itmin->first << ", " << ratio << "," << std::endl;
            ++itmax;
            ++itmin;
        }

        std::cout << "Average temperature," << std::endl;
        std::map<unsigned int, float>::iterator temperature_it = total_temperature_per_year_map.begin();
        std::map<unsigned int, unsigned int>::iterator count_it = number_of_readings_per_year_map.begin();
        std::vector<float> maximum_month_running_total_vector;
        std::vector<float> minimum_month_running_total_vector;
        std::vector<float> average_month_running_total_vector;
        float total_temperature = 0.0f;
        int consecutive_count = 0;
        size_t previous_month_number = 0;

        while ( temperature_it != total_temperature_per_year_map.end() && count_it != number_of_readings_per_year_map.end() )
        {
            float average = float(temperature_it->second) / float(count_it->second);
            std::cout << temperature_it->first << ", " << average << ", " << count_it->second << ",,   ";

            unsigned int year = temperature_it->first;

            for (size_t month = 0; month < NUMBER_OF_MONTHS_PER_YEAR; month++)
            {
                float monthly_average = UNKNOWN_TEMPERATURE;
                size_t month_number = (year * NUMBER_OF_MONTHS_PER_YEAR) + month;


                if ( number_of_readings_per_month[year - FIRST_YEAR][month] )
                {
                    if ( (month_number - previous_month_number) == 1 )
                    {
                        consecutive_count ++;
                    }
                    else
                    {
                        consecutive_count = 0;
                    }

                    monthly_average = total_temperature_per_month[year - FIRST_YEAR][month] / number_of_readings_per_month[year - FIRST_YEAR][month];
                    total_temperature += monthly_average;
                    average_month_running_total_vector.push_back(total_temperature);

                    if (consecutive_count >= number_of_months_for_sequential_statistics)
                    {
                        size_t size = average_month_running_total_vector.size();
                        float total_variable_month_temperature = average_month_running_total_vector.at(size - 1) - average_month_running_total_vector.at(size - 1 - number_of_months_for_sequential_statistics);
                        float average_temperature = total_variable_month_temperature / float(number_of_months_for_sequential_statistics);
                        US.getVariableMonthMeanAverageMap()[average_temperature] = month_number;
                    }

                    previous_month_number = month_number;
                }

                std::cout << " " << monthly_average << ", ";
            }

            std::cout << std::endl;

            temperature_it++;
            count_it++;
        }

        std::cout << "Hottest Average" << number_of_months_for_sequential_statistics << " month periods " << std::endl;
        std::cout << "Rank, " << "Month, " << "Year, " << "Temperature " << std::endl;
        std::map<float,size_t>::reverse_iterator variable_month_iterator = US.getVariableMonthMeanAverageMap().rbegin();
        size_t count = 1;

        for ( ; variable_month_iterator != US.getVariableMonthMeanAverageMap().rend(); variable_month_iterator++ )
        {
            unsigned int year = variable_month_iterator->second / NUMBER_OF_MONTHS_PER_YEAR;
            unsigned int month = variable_month_iterator->second % NUMBER_OF_MONTHS_PER_YEAR;
            float temperature = variable_month_iterator->first;

            std::cout << count++ << ", " << month + 1 << ", " << year << ", " << temperature << std::endl;
        }


        std::cout << "Average maximum temperature," << std::endl;
        temperature_it = total_max_temperature_per_year_map.begin();
        count_it = number_of_max_readings_per_year_map.begin();

        while ( temperature_it != total_max_temperature_per_year_map.end() && count_it != number_of_max_readings_per_year_map.end() )
        {
            float average = float(temperature_it->second) / float(count_it->second);
            std::cout << temperature_it->first << ", " << average << ", " << count_it->second << ",,   ";

            unsigned int year = temperature_it->first;

            for (size_t month = 0; month < NUMBER_OF_MONTHS_PER_YEAR; month++)
            {
                float monthly_average = UNKNOWN_TEMPERATURE;
                size_t month_number = (year * NUMBER_OF_MONTHS_PER_YEAR) + month;

                if ( number_of_max_readings_per_month[year - FIRST_YEAR][month] )
                {
                    if ( (month_number - previous_month_number) == 1 )
                    {
                        consecutive_count ++;
                    }
                    else
                    {
                        consecutive_count = 0;
                    }

                    monthly_average = total_max_temperature_per_month[year - FIRST_YEAR][month] / number_of_max_readings_per_month[year - FIRST_YEAR][month];
                    total_temperature += monthly_average;

                    maximum_month_running_total_vector.push_back(total_temperature);

                    if (consecutive_count >= number_of_months_for_sequential_statistics)
                    {
                        size_t size = maximum_month_running_total_vector.size();
                        float total_variable_month_temperature = maximum_month_running_total_vector.at(size - 1) - maximum_month_running_total_vector.at(size - 1 - number_of_months_for_sequential_statistics);
                        float average_temperature = total_variable_month_temperature / float(number_of_months_for_sequential_statistics);
                        US.getVariableMonthMeanMaximumMap()[average_temperature] = month_number;
                    }

                    previous_month_number = month_number;
                }

                std::cout << " " << monthly_average << ", ";
            }

            std::cout << std::endl;

            temperature_it++;
            count_it++;
        }

        std::cout << "Hottest Maximum" << number_of_months_for_sequential_statistics << " month periods " << std::endl;
        std::cout << "Rank, " << "Month, " << "Year, " << "Temperature " << std::endl;
        variable_month_iterator = US.getVariableMonthMeanMaximumMap().rbegin();
        count = 1;

        for ( ; variable_month_iterator != US.getVariableMonthMeanMaximumMap().rend(); variable_month_iterator++ )
        {
            unsigned int year = variable_month_iterator->second / NUMBER_OF_MONTHS_PER_YEAR;
            unsigned int month = variable_month_iterator->second % NUMBER_OF_MONTHS_PER_YEAR;
            float temperature = variable_month_iterator->first;

            std::cout << count++ << ", " << month + 1 << ", " << year << ", " << temperature << std::endl;
        }

        std::cout << "Average minimum temperature," << std::endl;
        temperature_it = total_min_temperature_per_year_map.begin();
        count_it = number_of_min_readings_per_year_map.begin();

        while ( temperature_it != total_min_temperature_per_year_map.end() && count_it != number_of_min_readings_per_year_map.end() )
        {
            float average = float(temperature_it->second) / float(count_it->second);
            std::cout << temperature_it->first << ", " << average << ", " << count_it->second << ",,   ";

            unsigned int year = temperature_it->first;

            for (size_t month = 0; month < NUMBER_OF_MONTHS_PER_YEAR; month++)
            {
                float monthly_average = UNKNOWN_TEMPERATURE;
                size_t month_number = (year * NUMBER_OF_MONTHS_PER_YEAR) + month;

                if ( number_of_min_readings_per_month[year - FIRST_YEAR][month] )
                {
                    if ( (month_number - previous_month_number) == 1 )
                    {
                        consecutive_count ++;
                    }
                    else
                    {
                        consecutive_count = 0;
                    }

                    monthly_average = total_min_temperature_per_month[year - FIRST_YEAR][month] / number_of_min_readings_per_month[year - FIRST_YEAR][month];
                    total_temperature += monthly_average;

                    minimum_month_running_total_vector.push_back(total_temperature);

                    if (consecutive_count >= number_of_months_for_sequential_statistics)
                    {
                        size_t size = minimum_month_running_total_vector.size();
                        float total_variable_month_temperature = minimum_month_running_total_vector.at(size - 1) - minimum_month_running_total_vector.at(size - 1 - number_of_months_for_sequential_statistics);
                        float average_temperature = total_variable_month_temperature / float(number_of_months_for_sequential_statistics);
                        US.getVariableMonthMeanMinimumMap()[average_temperature] = month_number;
                    }

                    previous_month_number = month_number;
                }

                std::cout << " " << monthly_average << ", ";
            }

            std::cout << std::endl;

            temperature_it++;
            count_it++;
        }

        std::cout << "Hottest Minimum" << number_of_months_for_sequential_statistics << " month periods " << std::endl;
        std::cout << "Rank, " << "Month, " << "Year, " << "Temperature " << std::endl;
        variable_month_iterator = US.getVariableMonthMeanMinimumMap().rbegin();
        count = 1;

        for ( ; variable_month_iterator != US.getVariableMonthMeanMinimumMap().rend(); variable_month_iterator++ )
        {
            unsigned int year = variable_month_iterator->second / NUMBER_OF_MONTHS_PER_YEAR;
            unsigned int month = variable_month_iterator->second % NUMBER_OF_MONTHS_PER_YEAR;
            float temperature = variable_month_iterator->first;

            std::cout << count++ << ", " << month + 1 << ", " << year << ", " << temperature << std::endl;
        }


    }
    else 
    {
        std::cout << "Unable to open us.txt" << std::endl; 
    }

    return 0;
}



