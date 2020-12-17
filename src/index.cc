#include <node_api.h>
#include <assert.h>
#include <utility>
#include <iostream>
#include "../lib/skiplist.cc"
#include "helper.cc"

using std::pair, std::cout, std::endl, std::pair;
napi_value JsSkiplist(
    napi_env env,
    napi_callback_info context
) {
    napi_status status;

    size_t argc = 1;
    napi_value argv[1];
    status = napi_get_cb_info(env, context, &argc, argv, NULL, NULL);   
    assert(status == napi_ok);

    if(argc > 1) {
        napi_throw_type_error(env, NULL, "Skiplist can;t accept second params!");
        return NULL;
    }

    napi_valuetype argtype;
    status = napi_typeof(env, argv[0], &argtype);
    assert(status == napi_ok);

    if(argtype != napi_null && argtype != napi_object) {
        napi_throw_type_error(env, NULL, "Skiplist only accept pairs or map or object or null param!");
        return NULL;
    }

    if(status != napi_null) {
        bool ret;
        status = napi_is_array(env, argv[0], &ret);
        assert(status == napi_ok);
        if(ret) {
            uint32_t length;
            status = napi_get_array_length(env, argv[0], &length);
            assert(status == napi_ok);
            assert(length > 0);
            vector<pair<string,int>> datas;   

            for(uint32_t i = 0;i < length; i++) {
                napi_value ret = get_array_property_val(env, argv[0], i);
                bool isArray = type_is_array(env, ret);
                assert(isArray == true);
                string key = get_string(env, get_array_property_val(env, ret, 0));
                int val = get_int32(env, get_array_property_val(env, ret, 1));
                pair<string,int> data(key,val);
                datas.push_back(data);
            }
            Skiplist<int, string> skiplist(datas);
        }
    }

} 

napi_value init(
    napi_env env,
    napi_value exports
) {
    napi_status status;
    napi_value makeJsSkiplist;

    status = napi_create_function(env, "Skiplist", NAPI_AUTO_LENGTH, JsSkiplist, nullptr, &makeJsSkiplist);
    assert(status == napi_ok);

    napi_value ret;
    napi_create_int32(env, 5, &ret);
    status = napi_set_named_property(env, exports, "Skiplist", makeJsSkiplist);
    assert(status == napi_ok);

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);