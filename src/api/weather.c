#include "weather.h"

#include "../arena.h"
#include "../json.h"
#include "api_utils.h"

char* weather_api_handler(const char* request_body) {
    (void)request_body;

    Arena* arena = get_fresh_request_arena();

    json_object* resp = arena_alloc(arena, sizeof(json_object));
    resp->type = JSON_OBJECT;
    resp->value.object = dict_arena_new(arena, 4);

    json_object* loc = arena_alloc(arena, sizeof(json_object));
    loc->type = JSON_STRING;
    loc->value.string = arena_strdup(arena, "Leiden");
    dict_set(resp->value.object, "location", loc);

    json_object* current = arena_alloc(arena, sizeof(json_object));
    current->type = JSON_OBJECT;
    current->value.object = dict_arena_new(arena, 3);

    json_object* temp = arena_alloc(arena, sizeof(json_object));
    temp->type = JSON_NUMBER;
    temp->value.number = 18.2;
    dict_set(current->value.object, "temperature", temp);

    json_object* humidity = arena_alloc(arena, sizeof(json_object));
    humidity->type = JSON_NUMBER;
    humidity->value.number = 0.75;
    dict_set(current->value.object, "humidity", humidity);

    json_object* cond = arena_alloc(arena, sizeof(json_object));
    cond->type = JSON_STRING;
    cond->value.string = arena_strdup(arena, "Light Rain");
    dict_set(current->value.object, "condition", cond);

    dict_set(resp->value.object, "current", current);

    json_object* forecast = arena_alloc(arena, sizeof(json_object));
    forecast->type = JSON_ARRAY;
    forecast->value.array = arraylist_arena_new(arena, 10);

    const char* dates[] = {"2025-09-20", "2025-09-21", "2025-09-22"};
    double highs[] = {19, 21, 18};
    double lows[] = {12, 14, 11};
    const char* conditions[] = {"Partly Cloudy", "Sunny", "Rain"};

    for (int i = 0; i < 3; i++) {
        json_object* day_obj = arena_alloc(arena, sizeof(json_object));
        day_obj->type = JSON_OBJECT;
        day_obj->value.object = dict_arena_new(arena, 4);

        json_object* date = arena_alloc(arena, sizeof(json_object));
        date->type = JSON_STRING;
        date->value.string = arena_strdup(arena, dates[i]);
        dict_set(day_obj->value.object, "date", date);

        json_object* high = arena_alloc(arena, sizeof(json_object));
        high->type = JSON_NUMBER;
        high->value.number = highs[i];
        dict_set(day_obj->value.object, "high", high);

        json_object* low = arena_alloc(arena, sizeof(json_object));
        low->type = JSON_NUMBER;
        low->value.number = lows[i];
        dict_set(day_obj->value.object, "low", low);

        json_object* condition = arena_alloc(arena, sizeof(json_object));
        condition->type = JSON_STRING;
        condition->value.string = arena_strdup(arena, conditions[i]);
        dict_set(day_obj->value.object, "condition", condition);

        arraylist_append(forecast->value.array, day_obj, false);
    }

    dict_set(resp->value.object, "forecast", forecast);

    char* encoded = json_encode(resp);

    return encoded;
}
