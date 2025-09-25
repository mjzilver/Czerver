#include "weather.h"

#include "../arena.h"
#include "../json.h"
#include "../json_utils.h"
#include "api_utils.h"

typedef struct {
    const char* location;
    double temperature;
    double humidity;
    const char* condition;
} Weather;

char* weather_api_handler(const char* request_body) {
    (void)request_body;

    Arena* arena = get_fresh_request_arena();

    Weather w = {.location = "Leiden", .temperature = 18.2, .humidity = 0.75, .condition = "Light Rain"};

    FieldDescriptor weather_fields[] = {
        {"location", FIELD_STRING, offsetof(Weather, location)},
        {"temperature", FIELD_NUMBER, offsetof(Weather, temperature)},
        {"humidity", FIELD_NUMBER, offsetof(Weather, humidity)},
        {"condition", FIELD_STRING, offsetof(Weather, condition)},
    };
    size_t weather_field_count = 4;

    json_object* resp = struct_to_json(arena, &w, weather_fields, weather_field_count);

    return json_encode(resp);
}
