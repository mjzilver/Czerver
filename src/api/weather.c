#include "weather.h"

#include "../datastructure/arena.h"
#include "../json/json.h"
#include "../json/json_utils.h"
#include "api_utils.h"

typedef struct Temperature {
    double low;
    double high;
} Temperature;

typedef struct WeatherDay {
    const char* date;
    char* cond;
    Temperature temp;
} WeatherDay;

typedef struct WeatherForecast {
    const char* location;
    size_t num_days;
    ArrayList* days;
} WeatherForecast;

char* weather_api_handler(const char* request_body) {
    (void)request_body;

    Arena* arena = get_fresh_request_arena();

    WeatherDay days_arr[] = {{"2025-09-26", "Cloudy", {12.5, 21.0}},
                             {"2025-09-27", "Sunny", {13.0, 22.5}},
                             {"2025-09-28", "Raining", {11.8, 20.0}}};

    ArrayList* days_list = arraylist_arena_new(arena, FIELD_COUNT(days_arr));

    FieldDescriptor temperature_fields[] = {{"low", FIELD_TYPE_NUMBER, offsetof(Temperature, low)},
                                            {"high", FIELD_TYPE_NUMBER, offsetof(Temperature, high)}};

    ObjectMeta object_meta = {.fields = temperature_fields, .field_count = FIELD_COUNT(temperature_fields)};

    FieldDescriptor day_fields[] = {{"date", FIELD_TYPE_STRING, offsetof(WeatherDay, date)},
                                    {"cond", FIELD_TYPE_STRING, offsetof(WeatherDay, cond)},
                                    {"temp", FIELD_TYPE_OBJECT, offsetof(WeatherDay, temp), .object = &object_meta}};

    for (size_t i = 0; i < 3; i++) {
        JsonValueNode* day_obj = struct_to_json_value_node(arena, &days_arr[i], day_fields, FIELD_COUNT(day_fields));
        arraylist_append(days_list, day_obj, 1);
    }

    WeatherForecast forecast = {.location = "Leiden", .days = days_list};

    FieldDescriptor forecast_fields[] = {
        {"location", FIELD_TYPE_STRING, offsetof(WeatherForecast, location)},
        {"days", FIELD_TYPE_ARRAY, offsetof(WeatherForecast, days), .array = days_list}};

    JsonValueNode* resp = struct_to_json_value_node(arena, &forecast, forecast_fields, FIELD_COUNT(forecast_fields));

    return json_stringify(resp);
}
