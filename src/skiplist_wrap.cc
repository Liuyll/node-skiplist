#include <node_api.h>
#include "../lib/skiplist.cc"

class JsSkiplist {
    private:
        Skiplist skiplist;
    public:
        JsSkiplist(Skiplist skiplist):skiplist(skiplist) {}
        SkiplistNodeData get(SkiplistNodeKey key) {
            this->skiplist.test();
            cout<<"key:"<<key.val<<endl;
            SkiplistNodeData ret = this->skiplist.get(key);
            cout<<"qwer"<<endl;
            if(ret.isNull()) return SkiplistNodeData::getNull();
            return ret;
        }
};

napi_value JsSkiplist_Get(napi_env env, napi_callback_info context) {
    napi_status status;
    JsSkiplist *jsSkiplist;
    get_native_object_from_context(env, context, jsSkiplist);
    size_t argc = 2;
    napi_value *argv = get_args_from_context(env, context, &argc);

    string key = get_string_from_any_type(env, argv[1]);
    SkiplistNodeData ret = jsSkiplist->get(SkiplistNodeKey(key));
    cout<<"val:"<<(ret.val)<<endl;
    return NULL;
}

