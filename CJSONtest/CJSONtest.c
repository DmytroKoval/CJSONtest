// CJSONtest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include "cJSON.h"
#include "app_weather.h"

int main()
{
	const char* filename = "weather.json";
	
	int fh;
	if (_sopen_s(&fh, filename, _O_RDONLY, _SH_DENYNO, 0) != 0)
	{
		printf("\"%s\" not found\n", filename);
		return;
	}

	size_t length = _lseek(fh, 0, SEEK_END);
	_lseek(fh, 0, SEEK_SET);

	char* buffer = (char*)malloc(length + 1);
	if (!buffer) 
	{
		printf("can not allocate read buffer\n");
		return;
	}
	int bytes_read = _read(fh, buffer, (uint32_t)length);
	if (bytes_read != length)
	{
		printf("can not read file\n");
		return;
	}
	_close(fh);

	/*
		time_t now = time(NULL);
		struct tm ts;
		wchar_t buf[80];

		localtime_s(&ts, &now);
		//strtime_s(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
		_wstrtime_s(buf, 80);

		wprintf(L"%s\n", buf);
	*/

	cJSON* json = cJSON_Parse(buffer);
	const char* error_ptr = cJSON_GetErrorPtr();
	if (error_ptr != NULL)
	{
		fprintf(stderr, "Error before: %s\n", error_ptr);
		cJSON_Delete(json);
		return;
	}

	weather_info_t weather;

	parse_weather(json, &weather);
	
	print_weather(&weather);
	
	cJSON_Delete(json);
}

void print_weather(weather_info_t const* weather)
{
	char current_time[80];
	char sunrise_time[10];
	char sunset_time[10];
	
	struct tm daylight_tm;
	gmtime_s(&daylight_tm, &weather->current.daylight);
	char daylight_time[10];

	const char* fmt = "%Y-%m-%d %H:%M:%S";
	const char* fmt_t = "%H:%M:%S";
	
	strftime(current_time, sizeof(current_time), fmt, &weather->current.dt);
	strftime(sunrise_time, sizeof(sunrise_time), fmt_t, &weather->current.sunrise);
	strftime(sunset_time, sizeof(sunset_time), fmt_t, &weather->current.sunset);
	strftime(daylight_time, sizeof(daylight_time), fmt_t, &daylight_tm);
	printf("\n\nLattitude: %.06f, Longtitude: %.06f\n"
		"Time: %s (%s)\n"
		"Sunrise: %s\tSunset:  %s\n"
		"Day duration: %s\n"
		"Current weather:\n"
		"   Temp, C:   %12.01f   feels like, C:\t%5.1f\n"
		"   Pressure, hPa: %6d     Hum, %%:%15d\n"
		"   Wind: %03d deg, %.01f m/s (%.01f m/s)\n"
		"   %s, %s\n"
		,
		weather->lat, weather->lon,
		current_time, weather->timezone,
		sunrise_time, sunset_time,
		daylight_time,
		weather->current.temp, weather->current.feels_like,
		weather->current.pressure, weather->current.humidity,
		weather->current.wind_deg, weather->current.wind_speed, weather->current.wind_gust,
		weather->current.weather.main, weather->current.weather.description
	);
	
	if (weather->nhours > 0 && weather->hourly != NULL)
	{
		printf("Hourly forecast:\n================\n");
		for (int i = 0; i < weather->nhours; i++)
		{
			weather_info_hour_t h = weather->hourly[i];
			strftime(current_time, sizeof(current_time), fmt, &h.dt);
			printf("%20s: %5.01f (%5.01f)\n", current_time, h.temp, h.feels_like);
		}
		puts("\n");
	}
	
	if (weather->ndays > 0 && weather->daily != NULL)
	{
		printf("Daily forecast:\n================\n");
		for (int i = 0; i < weather->ndays; i++)
		{
			weather_info_day_t day = weather->daily[i];
			printf("%5.01f (%5.01f) - %s\n", day.temp.day, day.feels_like.day, day.summary);
		}
		puts("\n");
	}
	char alert_start[30];
	char alert_end[30];
	if (weather->nalerts > 0 && weather->alerts != NULL)
	{
		printf("Alerts:\n================\n");
		for (int i = 0; i < weather->nalerts; i++)
		{
			weather_info_alert_t alert = weather->alerts[i];
			if (alert.ntags > 0 && alert.tags != NULL)
			{
				for (int j = 0; j < alert.ntags; j++)
				{
					printf("[%s]", alert.tags[j]);
					if (j < alert.ntags - 1)
					{
						puts(" ");
					}
				}
				printf("\n");
			}
			strftime(alert_start, sizeof(alert_start), fmt, &alert.start_time);
			strftime(alert_end, sizeof(alert_end), fmt, &alert.end_time);
			printf("%s reports %s from %s to %s.\n%s\n================\n", alert.sender_name, alert.event, alert_start, alert_end, alert.description);
		}
	}
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
