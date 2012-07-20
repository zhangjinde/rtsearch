#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#ifdef __cplusplus
extern "C"{
#endif
#include "json/json.h"

json_object *json_util_get(json_object *obj, const char *path);
int jobject_ptr_isvalid(json_object* ptr);

#ifdef __cplusplus
}
#endif

#endif

