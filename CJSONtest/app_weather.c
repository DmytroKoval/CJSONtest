#include <errno.h>
#include <malloc.h>
#include <string.h>

#include "cJSON.h"
#include "app_weather.h"

static errno_t parse_weather_info_description(const cJSON* obj, weather_info_description_t* descr)
{
	if (obj == NULL)
	{
		return ENODATA;
	}

	const cJSON* descr_id = cJSON_GetObjectItemCaseSensitive(obj, "id");
	if (cJSON_IsNumber(descr_id))
	{
		descr->id = descr_id->valueint;
	}

	const cJSON* descr_main = cJSON_GetObjectItemCaseSensitive(obj, "main");
	if (cJSON_IsString(descr_main) && (descr_main->valuestring != NULL))
	{
		size_t len = strlen(descr_main->valuestring) + 1;
		descr->main = (char*)malloc(len);
		if (!descr->main)
		{
			return ENOMEM;
		}
		memset(descr->main, 0, len);
		strcpy_s(descr->main, len, descr_main->valuestring);
	}
	else
	{
		return ENODATA;
	}

	const cJSON* descr_description = cJSON_GetObjectItemCaseSensitive(obj, "description");
	if (cJSON_IsString(descr_description) && (descr_description->valuestring != NULL))
	{
		size_t len = strlen(descr_description->valuestring) + 1;
		descr->description = (char*)malloc(len);
		if (!descr->description)
		{
			return ENOMEM;
		}
		memset(descr->description, 0, len);
		strcpy_s(descr->description, len, descr_description->valuestring);
	}
	else
	{
		return ENODATA;
	}

	const cJSON* descr_icon_name = cJSON_GetObjectItemCaseSensitive(obj, "icon_name");
	if (cJSON_IsString(descr_icon_name) && (descr_icon_name->valuestring != NULL))
	{
		size_t len = strlen(descr_icon_name->valuestring) + 1;
		descr->icon_name = (char*)malloc(len);
		if (!descr->icon_name)
		{
			return ENOMEM;
		}
		memset(descr->icon_name, 0, len);
		strcpy_s(descr->icon_name, len, descr_icon_name->valuestring);
	}
	else
	{
		return ENODATA;
	}
	return 0;
}

static errno_t parse_weather_info_current(const cJSON* obj, weather_info_current_t* current)
{
	const cJSON* current_dt = cJSON_GetObjectItemCaseSensitive(obj, "dt");
	if (cJSON_IsNumber(current_dt))
	{
		time_t t = current_dt->valueint;
		localtime_s(&(current->dt), &(t));
	}
	const cJSON* current_sunrise = cJSON_GetObjectItemCaseSensitive(obj, "sunrise");
	if (cJSON_IsNumber(current_sunrise))
	{
		time_t t = current_sunrise->valueint;
		localtime_s(&(current->sunrise), &t);
	}
	const cJSON* current_sunset = cJSON_GetObjectItemCaseSensitive(obj, "sunset");
	if (cJSON_IsNumber(current_sunset))
	{
		time_t t = current_sunset->valueint;
		localtime_s(&(current->sunset), &t);
	}
	current->daylight = (time_t)current_sunset->valueint - current_sunrise->valueint;

	const cJSON* current_temp = cJSON_GetObjectItemCaseSensitive(obj, "temp");
	if (cJSON_IsNumber(current_temp))
	{
		current->temp = current_temp->valuedouble;
	}

	const cJSON* current_feels_like = cJSON_GetObjectItemCaseSensitive(obj, "feels_like");
	if (cJSON_IsNumber(current_feels_like))
	{
		current->feels_like = current_feels_like->valuedouble;
	}

	const cJSON* current_pressure = cJSON_GetObjectItemCaseSensitive(obj, "pressure");
	if (cJSON_IsNumber(current_pressure))
	{
		current->pressure = current_pressure->valueint;
	}

	const cJSON* current_humidity = cJSON_GetObjectItemCaseSensitive(obj, "humidity");
	if (cJSON_IsNumber(current_humidity))
	{
		current->humidity = current_humidity->valueint;
	}

	const cJSON* current_dew_point = cJSON_GetObjectItemCaseSensitive(obj, "dew_point");
	if (cJSON_IsNumber(current_dew_point))
	{
		current->dew_point = current_dew_point->valuedouble;
	}

	const cJSON* current_uvi = cJSON_GetObjectItemCaseSensitive(obj, "uvi");
	if (cJSON_IsNumber(current_uvi))
	{
		current->uvi = current_uvi->valuedouble;
	}

	const cJSON* current_clouds = cJSON_GetObjectItemCaseSensitive(obj, "clouds");
	if (cJSON_IsNumber(current_clouds))
	{
		current->clouds = current_clouds->valueint;
	}

	const cJSON* current_visibility = cJSON_GetObjectItemCaseSensitive(obj, "visibility");
	if (cJSON_IsNumber(current_visibility))
	{
		current->visibility = current_visibility->valueint;
	}

	const cJSON* current_wind_speed = cJSON_GetObjectItemCaseSensitive(obj, "wind_speed");
	if (cJSON_IsNumber(current_wind_speed))
	{
		current->wind_speed = current_wind_speed->valuedouble;
	}

	const cJSON* current_wind_deg = cJSON_GetObjectItemCaseSensitive(obj, "wind_deg");
	if (cJSON_IsNumber(current_wind_deg))
	{
		current->wind_deg = current_wind_deg->valueint;
	}

	const cJSON* current_wind_gust = cJSON_GetObjectItemCaseSensitive(obj, "wind_gust");
	if (cJSON_IsNumber(current_wind_gust))
	{
		current->wind_gust = current_wind_gust->valuedouble;
	}

	const cJSON* current_weather = NULL;
	const cJSON* cw = cJSON_GetObjectItemCaseSensitive(obj, "weather");
	if (cw != NULL && cJSON_IsArray(cw))
	{
		cJSON_ArrayForEach(current_weather, cw)
		{
			parse_weather_info_description(current_weather, &(current->weather));
			break;
		}
	}

	return 0;
}

static errno_t parse_hour_forecast(const cJSON* obj, weather_info_hour_t* hr)
{
	const cJSON* current_dt = cJSON_GetObjectItemCaseSensitive(obj, "dt");
	if (cJSON_IsNumber(current_dt))
	{
		time_t t = current_dt->valueint;
		localtime_s(&(hr->dt), &(t));
	}

	const cJSON* current_temp = cJSON_GetObjectItemCaseSensitive(obj, "temp");
	if (cJSON_IsNumber(current_temp))
	{
		hr->temp = current_temp->valuedouble;
	}

	const cJSON* current_feels_like = cJSON_GetObjectItemCaseSensitive(obj, "feels_like");
	if (cJSON_IsNumber(current_feels_like))
	{
		hr->feels_like = current_feels_like->valuedouble;
	}

	const cJSON* current_pressure = cJSON_GetObjectItemCaseSensitive(obj, "pressure");
	if (cJSON_IsNumber(current_pressure))
	{
		hr->pressure = current_pressure->valueint;
	}

	const cJSON* current_humidity = cJSON_GetObjectItemCaseSensitive(obj, "humidity");
	if (cJSON_IsNumber(current_humidity))
	{
		hr->humidity = current_humidity->valueint;
	}

	const cJSON* current_dew_point = cJSON_GetObjectItemCaseSensitive(obj, "dew_point");
	if (cJSON_IsNumber(current_dew_point))
	{
		hr->dew_point = current_dew_point->valuedouble;
	}

	const cJSON* current_uvi = cJSON_GetObjectItemCaseSensitive(obj, "uvi");
	if (cJSON_IsNumber(current_uvi))
	{
		hr->uvi = current_uvi->valuedouble;
	}

	const cJSON* current_clouds = cJSON_GetObjectItemCaseSensitive(obj, "clouds");
	if (cJSON_IsNumber(current_clouds))
	{
		hr->clouds = current_clouds->valueint;
	}

	const cJSON* current_visibility = cJSON_GetObjectItemCaseSensitive(obj, "visibility");
	if (cJSON_IsNumber(current_visibility))
	{
		hr->visibility = current_visibility->valueint;
	}

	const cJSON* current_wind_speed = cJSON_GetObjectItemCaseSensitive(obj, "wind_speed");
	if (cJSON_IsNumber(current_wind_speed))
	{
		hr->wind_speed = current_wind_speed->valuedouble;
	}

	const cJSON* current_wind_deg = cJSON_GetObjectItemCaseSensitive(obj, "wind_deg");
	if (cJSON_IsNumber(current_wind_deg))
	{
		hr->wind_deg = current_wind_deg->valueint;
	}

	const cJSON* current_wind_gust = cJSON_GetObjectItemCaseSensitive(obj, "wind_gust");
	if (cJSON_IsNumber(current_wind_gust))
	{
		hr->wind_gust = current_wind_gust->valuedouble;
	}

	const cJSON* weather = NULL;
	const cJSON* w = cJSON_GetObjectItemCaseSensitive(obj, "weather");
	if (w != NULL && cJSON_IsArray(w))
	{
		cJSON_ArrayForEach(weather, w)
		{
			parse_weather_info_description(weather, &(hr->weather));
			break;
		}
	}

	const cJSON* current_pop = cJSON_GetObjectItemCaseSensitive(obj, "pop");
	if (cJSON_IsNumber(current_pop))
	{
		hr->pop = (uint8_t)(current_pop->valuedouble * 100);
	}

	const cJSON* current_rain = cJSON_GetObjectItemCaseSensitive(obj, "rain");
	if (cJSON_IsObject(current_rain))
	{
		const cJSON* current_rain_1h = cJSON_GetObjectItemCaseSensitive(current_rain, "1h");
		if (cJSON_IsNumber(current_rain_1h))
		{
			hr->rain = current_rain_1h->valuedouble;
		}
	}

	const cJSON* current_snow = cJSON_GetObjectItemCaseSensitive(obj, "snow");
	if (cJSON_IsObject(current_snow))
	{
		const cJSON* current_snow_1h = cJSON_GetObjectItemCaseSensitive(current_snow, "1h");
		if (cJSON_IsNumber(current_snow_1h))
		{
			hr->snow = current_snow_1h->valuedouble;
		}
	}

	return 0;
}

static errno_t parse_day_forecast(const cJSON* obj, weather_info_day_t* day)
{
	const cJSON* day_dt = cJSON_GetObjectItemCaseSensitive(obj, "dt");
	if (cJSON_IsNumber(day_dt))
	{
		time_t t = day_dt->valueint;
		localtime_s(&(day->dt), &(t));
	}
	const cJSON* day_sunrise = cJSON_GetObjectItemCaseSensitive(obj, "sunrise");
	if (cJSON_IsNumber(day_sunrise))
	{
		time_t t = day_sunrise->valueint;
		localtime_s(&(day->sunrise), &t);
	}
	const cJSON* day_sunset = cJSON_GetObjectItemCaseSensitive(obj, "sunset");
	if (cJSON_IsNumber(day_sunset))
	{
		time_t t = day_sunset->valueint;
		localtime_s(&(day->sunset), &t);
	}

	day->daylight = (time_t)day_sunset->valueint - day_sunrise->valueint;

	const cJSON* day_moonrise = cJSON_GetObjectItemCaseSensitive(obj, "moonrise");
	if (cJSON_IsNumber(day_moonrise))
	{
		time_t t = day_moonrise->valueint;
		localtime_s(&(day->moonrise), &t);
	}
	const cJSON* day_moonset = cJSON_GetObjectItemCaseSensitive(obj, "moonset");
	if (cJSON_IsNumber(day_moonset))
	{
		time_t t = day_moonset->valueint;
		localtime_s(&(day->moonset), &t);
	}

	const cJSON* day_summary = cJSON_GetObjectItemCaseSensitive(obj, "summary");
	if (cJSON_IsString(day_summary) && (day_summary->valuestring != NULL))
	{
		size_t len = strlen(day_summary->valuestring) + 1;
		day->summary = (char*)malloc(len);
		if (!day->summary)
		{
			return ENOMEM;
		}
		memset(day->summary, 0, len);
		strcpy_s(day->summary, len, day_summary->valuestring);
	}


	const cJSON* day_temp = cJSON_GetObjectItemCaseSensitive(obj, "temp");
	if (cJSON_IsObject(day_temp))
	{
		const cJSON* day_temp_day = cJSON_GetObjectItemCaseSensitive(day_temp, "day");
		if (cJSON_IsNumber(day_temp_day))
		{
			day->temp.day = day_temp_day->valuedouble;
		}
		const cJSON* day_temp_night = cJSON_GetObjectItemCaseSensitive(day_temp, "night");
		if (cJSON_IsNumber(day_temp_night))
		{
			day->temp.night = day_temp_night->valuedouble;
		}
		const cJSON* day_temp_eve = cJSON_GetObjectItemCaseSensitive(day_temp, "eve");
		if (cJSON_IsNumber(day_temp_eve))
		{
			day->temp.eve = day_temp_eve->valuedouble;
		}
		const cJSON* day_temp_morn = cJSON_GetObjectItemCaseSensitive(day_temp, "morn");
		if (cJSON_IsNumber(day_temp_morn))
		{
			day->temp.morn = day_temp_morn->valuedouble;
		}
		const cJSON* day_temp_min = cJSON_GetObjectItemCaseSensitive(day_temp, "min");
		if (cJSON_IsNumber(day_temp_min))
		{
			day->temp.min = day_temp_min->valuedouble;
		}
		const cJSON* day_temp_max = cJSON_GetObjectItemCaseSensitive(day_temp, "max");
		if (cJSON_IsNumber(day_temp_max))
		{
			day->temp.max = day_temp_max->valuedouble;
		}
	}

	const cJSON* day_feels_like = cJSON_GetObjectItemCaseSensitive(obj, "feels_like");
	if (cJSON_IsObject(day_feels_like))
	{
		const cJSON* day_feels_like_day = cJSON_GetObjectItemCaseSensitive(day_feels_like, "day");
		if (cJSON_IsNumber(day_feels_like_day))
		{
			day->feels_like.day = day_feels_like_day->valuedouble;
		}
		const cJSON* day_feels_like_night = cJSON_GetObjectItemCaseSensitive(day_feels_like, "night");
		if (cJSON_IsNumber(day_feels_like_night))
		{
			day->feels_like.night = day_feels_like_night->valuedouble;
		}
		const cJSON* day_feels_like_eve = cJSON_GetObjectItemCaseSensitive(day_feels_like, "eve");
		if (cJSON_IsNumber(day_feels_like_eve))
		{
			day->feels_like.eve = day_feels_like_eve->valuedouble;
		}
		const cJSON* day_feels_like_morn = cJSON_GetObjectItemCaseSensitive(day_feels_like, "morn");
		if (cJSON_IsNumber(day_feels_like_morn))
		{
			day->feels_like.morn = day_feels_like_morn->valuedouble;
		}
	}

	const cJSON* day_pressure = cJSON_GetObjectItemCaseSensitive(obj, "pressure");
	if (cJSON_IsNumber(day_pressure))
	{
		day->pressure = day_pressure->valueint;
	}

	const cJSON* day_humidity = cJSON_GetObjectItemCaseSensitive(obj, "humidity");
	if (cJSON_IsNumber(day_humidity))
	{
		day->humidity = day_humidity->valueint;
	}

	const cJSON* day_dew_point = cJSON_GetObjectItemCaseSensitive(obj, "dew_point");
	if (cJSON_IsNumber(day_dew_point))
	{
		day->dew_point = day_dew_point->valuedouble;
	}

	const cJSON* day_wind_speed = cJSON_GetObjectItemCaseSensitive(obj, "wind_speed");
	if (cJSON_IsNumber(day_wind_speed))
	{
		day->wind_speed = day_wind_speed->valuedouble;
	}

	const cJSON* day_wind_deg = cJSON_GetObjectItemCaseSensitive(obj, "wind_deg");
	if (cJSON_IsNumber(day_wind_deg))
	{
		day->wind_deg = day_wind_deg->valueint;
	}

	const cJSON* day_wind_gust = cJSON_GetObjectItemCaseSensitive(obj, "wind_gust");
	if (cJSON_IsNumber(day_wind_gust))
	{
		day->wind_gust = day_wind_gust->valuedouble;
	}

	const cJSON* day_weather = NULL;
	const cJSON* dw = cJSON_GetObjectItemCaseSensitive(obj, "weather");
	if (dw != NULL && cJSON_IsArray(dw))
	{
		cJSON_ArrayForEach(day_weather, dw)
		{
			parse_weather_info_description(day_weather, &(day->weather));
			break;
		}
	}

	const cJSON* day_pop = cJSON_GetObjectItemCaseSensitive(obj, "pop");
	if (cJSON_IsNumber(day_pop))
	{
		day->pop = (uint8_t)(day_pop->valuedouble * 100);
	}

	const cJSON* day_rain = cJSON_GetObjectItemCaseSensitive(obj, "rain");
	if (cJSON_IsNumber(day_rain))
	{
		day->rain = day_rain->valuedouble;
	}

	const cJSON* day_snow = cJSON_GetObjectItemCaseSensitive(obj, "snow");
	if (cJSON_IsNumber(day_snow))
	{
		day->snow = day_snow->valuedouble;
	}

	const cJSON* day_uvi = cJSON_GetObjectItemCaseSensitive(obj, "uvi");
	if (cJSON_IsNumber(day_uvi))
	{
		day->uvi = day_uvi->valuedouble;
	}

	return 0;
}

static errno_t parse_alerts(const cJSON* obj, weather_info_alert_t* alert)
{
	const cJSON* alert_sender_name = cJSON_GetObjectItemCaseSensitive(obj, "sender_name");
	if (cJSON_IsString(alert_sender_name) && (alert_sender_name->valuestring != NULL))
	{
		size_t len = strlen(alert_sender_name->valuestring) + 1;
		alert->sender_name = (char*)malloc(len);
		if (!alert->sender_name)
		{
			return ENOMEM;
		}
		memset(alert->sender_name, 0, len);
		strcpy_s(alert->sender_name, len, alert_sender_name->valuestring);
	}

	const cJSON* alert_event = cJSON_GetObjectItemCaseSensitive(obj, "event");
	if (cJSON_IsString(alert_event) && (alert_event->valuestring != NULL))
	{
		size_t len = strlen(alert_event->valuestring) + 1;
		alert->event = (char*)malloc(len);
		if (!alert->event)
		{
			return ENOMEM;
		}
		memset(alert->event, 0, len);
		strcpy_s(alert->event, len, alert_event->valuestring);
	}

	const cJSON* alert_description = cJSON_GetObjectItemCaseSensitive(obj, "description");
	if (cJSON_IsString(alert_description) && (alert_description->valuestring != NULL))
	{
		size_t len = strlen(alert_description->valuestring) + 1;
		alert->description = (char*)malloc(len);
		if (!alert->description)
		{
			return ENOMEM;
		}
		memset(alert->description, 0, len);
		strcpy_s(alert->description, len, alert_description->valuestring);
	}
	
	const cJSON* alert_start = cJSON_GetObjectItemCaseSensitive(obj, "start");
	if (cJSON_IsNumber(alert_start))
	{
		time_t t = alert_start->valueint;
		localtime_s(&(alert->start_time), &(t));
	}

	const cJSON* alert_end = cJSON_GetObjectItemCaseSensitive(obj, "end");
	if (cJSON_IsNumber(alert_end))
	{
		time_t t = alert_end->valueint;
		localtime_s(&(alert->end_time), &(t));
	}

	const cJSON* alert_tags = cJSON_GetObjectItemCaseSensitive(obj, "tags");
	if (alert_tags != NULL && cJSON_IsArray(alert_tags))
	{
		alert->ntags = cJSON_GetArraySize(alert_tags);
		alert->tags = (char**)malloc(sizeof(char*) * alert->ntags);
		if (!alert->tags)
		{
			return ENOMEM;
		}
		const cJSON* tag = NULL;
		int n = 0;
		cJSON_ArrayForEach(tag, alert_tags)
		{
			if (cJSON_IsString(tag) && tag->valuestring != NULL)
			{
				size_t len = strlen(tag->valuestring) + 1;
				alert->tags[n] = (char*)malloc(sizeof(char*) * len);
				if (!alert->tags[n])
				{
					return ENOMEM;
				}
				memset(alert->tags[n], 0, len);
				strcpy_s(alert->tags[n], len, tag->valuestring);
				n++;
			}
		}
	}
	else
	{
		alert->ntags = 0;
		alert->tags = NULL;
	}
	return 0;
}

errno_t parse_weather(const cJSON* obj, weather_info_t* weather)
{
	const cJSON* timezone = cJSON_GetObjectItemCaseSensitive(obj, "timezone");
	if (cJSON_IsString(timezone) && (timezone->valuestring != NULL))
	{
		size_t len = strlen(timezone->valuestring) + 1;
		weather->timezone = (char*)malloc(len);
		if (!weather->timezone)
		{
			return ENOMEM;
		}
		memset(weather->timezone, 0, len);
		strcpy_s(weather->timezone, len, timezone->valuestring);
	}

	const cJSON* timezone_offset = cJSON_GetObjectItemCaseSensitive(obj, "timezone_offset");
	if (cJSON_IsNumber(timezone_offset))
	{
		weather->timezone_offset = timezone_offset->valueint;
	}

	const cJSON* coord_lat = cJSON_GetObjectItemCaseSensitive(obj, "lat");
	if (cJSON_IsNumber(coord_lat))
	{
		weather->lat = coord_lat->valuedouble;
	}

	const cJSON* coord_lon = cJSON_GetObjectItemCaseSensitive(obj, "lon");
	if (cJSON_IsNumber(coord_lon))
	{
		weather->lon = coord_lon->valuedouble;
	}

	const cJSON* current = cJSON_GetObjectItem(obj, "current");
	if (current != NULL && cJSON_IsObject(current))
	{
		parse_weather_info_current(current, &(weather->current));
	}

	const cJSON* hourly = cJSON_GetObjectItemCaseSensitive(obj, "hourly");
	if (hourly != NULL && cJSON_IsArray(hourly))
	{
		weather->nhours = cJSON_GetArraySize(hourly);
		weather->hourly = (weather_info_hour_t*)malloc(sizeof(weather_info_hour_t) * weather->nhours);
		if (!weather->hourly)
		{
			return ENOMEM;
		}
		const cJSON* hr = NULL;
		int n = 0;
		cJSON_ArrayForEach(hr, hourly)
		{
			parse_hour_forecast(hr, &(weather->hourly[n++]));
		}
	}
	else
	{
		weather->nhours = 0;
		weather->hourly = NULL;
	}

	const cJSON* daily = cJSON_GetObjectItemCaseSensitive(obj, "daily");
	if (daily != NULL && cJSON_IsArray(daily))
	{
		weather->ndays = cJSON_GetArraySize(daily);
		weather->daily = (weather_info_day_t*)malloc(sizeof(weather_info_day_t) * weather->ndays);
		if (!weather->daily)
		{
			return ENOMEM;
		}
		const cJSON* day = NULL;
		int n = 0;
		cJSON_ArrayForEach(day, daily)
		{
			parse_day_forecast(day, &(weather->daily[n++]));
		}
	}
	else
	{
		weather->ndays = 0;
		weather->daily = NULL;
	}
	const cJSON* alerts = cJSON_GetObjectItemCaseSensitive(obj, "alerts");
	weather->nalerts = 0;
	if (alerts != NULL && cJSON_IsArray(alerts))
	{
		weather->nalerts = cJSON_GetArraySize(alerts);
		weather->alerts = (weather_info_alert_t*)malloc(sizeof(weather_info_alert_t) * weather->nalerts);
		if (!weather->alerts)
		{
			return ENOMEM;
		}
		const cJSON* alert = NULL;
		int n = 0;
		cJSON_ArrayForEach(alert, alerts)
		{
			parse_alerts(alert, &weather->alerts[n++]);
		}
	}

	return 0;
}
