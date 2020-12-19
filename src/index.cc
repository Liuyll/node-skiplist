#include <node_api.h>
#include <assert.h>
#include <utility>
#include <iostream>
#include "../lib/skiplist.cc"
#include "helper.cc"
#include "skiplist_wrap.cc"

// using std::pair, std::cout, std::endl, std::pair;
using namespace std;

JsSkiplist* getJsSkiplist(
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
            vector<pair<SkiplistNodeKey,SkiplistNodeData>> datas;   

            for(uint32_t i = 0;i < length; i++) {
                napi_value ret = get_array_property_val(env, argv[0], i);
                bool isArray = type_is_array(env, ret);
                assert(isArray == true);
                string key = get_string_from_any_type(env, get_array_property_val(env, ret, 0));
                string val = get_string_from_any_type(env, get_array_property_val(env, ret, 1));
                pair<SkiplistNodeKey, SkiplistNodeData> data = make_pair(SkiplistNodeKey(key),SkiplistNodeData(val));
                datas.push_back(data);
            }
            return new JsSkiplist(Skiplist(datas));
        }
    }
}

napi_value createJsSkiplistWrap(napi_env env, JsSkiplist *jsSkiplist) {
    assert(jsSkiplist != nullptr);
    const string jsSkiplistClassName = "Skiplist";
    napi_ref ret;
    napi_status status;
    napi_value jsObject;

    status = napi_create_object(env, &jsObject);
    assert(status == napi_ok);

    status = napi_wrap(
        env,
        jsObject,
        jsSkiplist,
        NULL,
        NULL,
        NULL
    );

    if(status != napi_ok) return NULL;
    napi_value getInvoke;
    status = napi_create_function(env, NULL, 0, JsSkiplist_Get, NULL, &getInvoke);
    assert(status == napi_ok);
    status = napi_set_named_property(env, jsObject, "get", getInvoke);
    assert(status == napi_ok);
    return jsObject;
}

napi_value JsBindindBridge(napi_env env, napi_callback_info info) {
    JsSkiplist *jsSkiplist = getJsSkiplist(env, info);
    SkiplistNodeData data = jsSkiplist->get(SkiplistNodeKey("qwe123"));
    if(data.isNull()) {
        cout<<"no"<<endl;
    }
    return createJsSkiplistWrap(env, jsSkiplist);
}

napi_value init(
    napi_env env,
    napi_value exports
) {
    napi_status status;
    napi_value makeJsSkiplist;

    status = napi_create_function(env, "Skiplist", NAPI_AUTO_LENGTH, JsBindindBridge, nullptr, &makeJsSkiplist);
    assert(status == napi_ok);

    napi_value ret;
    napi_create_int32(env, 5, &ret);
    status = napi_set_named_property(env, exports, "Skiplist", makeJsSkiplist);
    assert(status == napi_ok);

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);