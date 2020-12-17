#include <node_api.h>
#include <string>
#include <iostream>

using std::cout, std::endl, std::string;
napi_status get_array_property_val(napi_env env, napi_value array, uint32_t index, napi_value *ret) {
    napi_value key;
    napi_status status;
    status = napi_create_int32(env, index, &key);
    if(status != napi_ok) return status;
    return napi_get_property(env, array, key, ret);
}

napi_value get_array_property_val(napi_env env, napi_value array, uint32_t index) {
    napi_value key;
    napi_value ret;
    napi_status status;
    status = napi_create_int32(env, index, &key);
    if(status != napi_ok) return NULL;
    status = napi_get_property(env, array, key, &ret);
    if(status != napi_ok) return NULL;
    return ret;
}

int get_int32(napi_env env, napi_value val) {
    napi_status status;
    int32_t ret;
    status = napi_get_value_int32(env, val, &ret);
    if(status != napi_ok) return NULL;
    return ret;
}

string get_string(napi_env env,napi_value val) {
    const int buf_length = 1000;
    napi_status status;
    char buf[buf_length];
    size_t length;
    string ret = "";
    status = napi_get_value_string_utf8(env, val, buf, buf_length, &length);
    if(status != napi_ok) return NULL;

    for(auto i = 0;i < length;i++) {
        if(buf[i] == 0) break;
        ret += buf[i];
    }
    return ret;
}

bool type_is_array(napi_env env, napi_value val) {
    bool ret;
    napi_status status = napi_is_array(env, val, &ret);
    assert(status == napi_ok);
    return ret;
}