#pragma once

#include <time.h>
#include <stdint.h>

#include "cJSON.h"

#ifdef __cplusplus
extern "C" {
#endif
	/**
	 * @brief Temperature typedef.
	 */
	typedef double temperature_t;
	typedef double uv_index_t;
	/** struct weather_info_description_t
	 * @brief Description of current weather conditions.
	 * @var weather_info_description_t::id
	 * condition id
	 * @var weather_info_description_t::main
	 * condition name
	 * @var weather_info_description_t::description
	 * condition description
	 * @var weather_info_description_t::icon_name
	 * condition icon name
	 */
	typedef struct {
		int id;
		char* main;
		char* description;
		char* icon_name;
	} weather_info_description_t;

	/**
	* @sample_data:
		"dt": 1734115079,
		"sunrise": 1734067534,
		"sunset": 1734097581,
		"temp": -4.28,
		"feels_like": -11.28,
		"pressure": 1029,
		"humidity": 62,
		"dew_point": -9.79,
		"uvi": 0,
		"clouds": 68,
		"visibility": 10000,
		"wind_speed": 7.26,
		"wind_deg": 322,
		"wind_gust": 11.75,
	*/
	typedef struct {
		struct tm dt;
		struct tm sunrise;
		struct tm sunset;
		time_t daylight;
		temperature_t temp;
		temperature_t feels_like;
		uint16_t pressure;
		uint8_t humidity;
		temperature_t dew_point;
		uv_index_t uvi;
		uint8_t clouds;
		int16_t visibility;
		temperature_t wind_speed;
		uint16_t wind_deg;
		temperature_t wind_gust;
		weather_info_description_t weather;
	} weather_info_current_t;

	typedef struct {
		struct tm dt;
		temperature_t temp;
		temperature_t feels_like;
		uint16_t pressure;
		uint8_t humidity;
		temperature_t dew_point;
		uv_index_t uvi;
		uint8_t clouds;
		uint16_t visibility;
		temperature_t wind_speed;
		uint8_t wind_deg;
		temperature_t wind_gust;
		weather_info_description_t weather;
		uint8_t pop;
		temperature_t rain;
		temperature_t snow;
	} weather_info_hour_t;

	typedef struct {
		temperature_t day;
		temperature_t night;
		temperature_t eve;
		temperature_t morn;
		temperature_t min;
		temperature_t max;
	} weather_temperature_info_t;

	typedef struct {
		struct tm dt;
		struct tm sunrise;
		struct tm sunset;
		time_t daylight;
		struct tm moonrise;
		struct tm moonset;
		temperature_t moon_phase;
		char* summary;
		weather_temperature_info_t temp;
		weather_temperature_info_t feels_like;
		uint16_t pressure;
		uint8_t humidity;
		temperature_t dew_point;
		temperature_t wind_speed;
		uint8_t wind_deg;
		temperature_t wind_gust;
		weather_info_description_t weather;
		uint8_t clouds;
		uint16_t pop;
		temperature_t rain;
		temperature_t snow;
		uv_index_t uvi;
	} weather_info_day_t;

	/*
"alerts": [
		{
			"sender_name": "Ukrainian Hydrometeorological Center",
			"event": "Wind warning",
			"start": 1734328800,
			"end": 1734386400,
			"description": "Wind gusts 15-20 m/s.",
			"tags": [
				"Other dangers"
			]
		}
	]
	*/

	typedef struct {
		char* sender_name;
		char* event;
		struct tm start_time;
		struct tm end_time;
		char* description;
		int ntags;
		char** tags;
	} weather_info_alert_t;

	typedef struct {
		temperature_t lat; // Lattitude
		temperature_t lon; // Longtitude
		char* timezone; // Timezone name
		uint16_t timezone_offset; // Timezone offset
		weather_info_current_t current; // Current weather conditions
		uint8_t nhours;
		weather_info_hour_t* hourly; // 48 hour forecast
		uint8_t ndays;
		weather_info_day_t* daily; // 8 day forecast
		uint8_t nalerts;
		weather_info_alert_t* alerts;
	} weather_info_t;

	errno_t parse_weather(const cJSON* obj, weather_info_t* weather);
	void print_weather(weather_info_t const* weather);

#ifdef __cplusplus
}
#endif
