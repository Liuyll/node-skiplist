#include <random>
#include <map>
#include <iostream>
#include <sstream>
#include <functional>
#include <float.h>
using namespace std;


#ifndef SKIPLIST
#define SKIPLIST

const double MAGIC_MAX_NUMBER = DBL_MAX - 27;
const string MAGIC_KEY_MAX = "__skiplist_magic_key_max";
const string MAGIC_KEY_MIN = "__skiplist_magic_key_min";
const string MAGIC_KEY_NULL = "__skiplist_magic_key_null";

typedef bool(*Compare)(string, string);

struct SkiplistNodeType {
    string val;
    double numberVal;
    // > return 1; < return -1 ; = return 0
    Compare compare = nullptr;

    static SkiplistNodeType getNull() {
        return SkiplistNodeType(MAGIC_KEY_NULL);
    };

    static SkiplistNodeType getMax() {
        return SkiplistNodeType(MAGIC_KEY_MAX);
    }

    static SkiplistNodeType getMin() {
        return SkiplistNodeType(MAGIC_KEY_MIN);
    }

    SkiplistNodeType(string val) {
        this->val = val;
        if(val == MAGIC_KEY_MIN) {
            this->numberVal = DBL_MIN;
        } else if(val == MAGIC_KEY_MAX) {
            this->numberVal = MAGIC_MAX_NUMBER;
        } else if(val == MAGIC_KEY_NULL) {
            this->numberVal = MAGIC_MAX_NUMBER;
        } else {
            istringstream is(val);
            is >> numberVal;
            if(!is.eof()) this->numberVal = MAGIC_MAX_NUMBER;
        }
    }

    SkiplistNodeType(string val, Compare compare) {
        new (this)SkiplistNodeType(val);
        this->compare = compare;
    }

    bool isNull() {
        return this->val == MAGIC_KEY_NULL;
    }
    bool operator>(const SkiplistNodeType& another) const{
        if(this->val == MAGIC_KEY_MAX && another.val != MAGIC_KEY_MAX) return true;
        if(this->compare) return (this->compare(this->val, another.val)) == 1 ? true : false;
        if(this->numberVal != MAGIC_MAX_NUMBER && another.numberVal != MAGIC_MAX_NUMBER) return this->numberVal > another.numberVal;
        else if(this->numberVal == MAGIC_MAX_NUMBER && another.numberVal == MAGIC_MAX_NUMBER) {
            return this->val > another.val;
        }
        return false;
    }

    bool operator<(const SkiplistNodeType& another) const{
        if(this->val == MAGIC_KEY_MIN && another.val != MAGIC_KEY_MIN) return true;
        if(this->compare != nullptr) return (this->compare(this->val, another.val)) == -1 ? true : false;
        if(this->numberVal != MAGIC_MAX_NUMBER && another.numberVal != MAGIC_MAX_NUMBER) return this->numberVal < another.numberVal;
        else if(this->numberVal == MAGIC_MAX_NUMBER && another.numberVal == MAGIC_MAX_NUMBER) {
            return this->val < another.val;
        }
        return false;
    }

    bool operator==(const SkiplistNodeType& another) const{
        if(this->compare != nullptr) return !(this->compare(this->val, another.val));
        if(this->numberVal != MAGIC_MAX_NUMBER && another.numberVal != MAGIC_MAX_NUMBER) return this->numberVal == another.numberVal;
        else if(this->numberVal == MAGIC_MAX_NUMBER && another.numberVal == MAGIC_MAX_NUMBER) {
            return this->val == another.val;
        }
        return false;
    }

    bool operator!=(const SkiplistNodeType& another) const{
        if(this->compare != nullptr) return this->compare(this->val, another.val);
        if(this->numberVal != MAGIC_MAX_NUMBER && another.numberVal != MAGIC_MAX_NUMBER) return this->numberVal != another.numberVal;
        else if(this->numberVal == MAGIC_MAX_NUMBER && another.numberVal == MAGIC_MAX_NUMBER) {
            return this->val != another.val;
        }
        return true;
    }
};

typedef SkiplistNodeType SkiplistNodeKey ;
typedef SkiplistNodeType SkiplistNodeData ;

struct SkiplistNodeTypeCompare {
    bool operator()(const SkiplistNodeKey &A, const SkiplistNodeKey &B) const {
        return A < B;
    }
};

struct SkiplistNode {
    SkiplistNode(SkiplistNodeData data, SkiplistNodeKey key): key(key), data(data) {
        this->up = nullptr;
        this->down = nullptr;
        this->left = nullptr;
        this->right = nullptr;
    }

    SkiplistNodeKey key;
    SkiplistNodeData data;
    SkiplistNode *up;
    SkiplistNode *down;
    SkiplistNode *left;
    SkiplistNode *right;
};

class Skiplist {
    SkiplistNode *head;
    SkiplistNode *tail;
    int layer;
    int curLayerCount;
    random_device rd;
    default_random_engine e = default_random_engine{rd()};
    uniform_real_distribution<double> u = uniform_real_distribution<double>(0.0, 1.0);

private:
    SkiplistNode *insertNewLayerNode(SkiplistNode *target) {
        SkiplistNode *newTarget = new SkiplistNode(target->data, target->key);
        target->up = newTarget;
        newTarget->down = target;
        return newTarget;
    }

    void connectNewNodeToNewHead(SkiplistNode *head, SkiplistNode *node) {
        node->right = new SkiplistNode(SkiplistNodeData::getNull(), SkiplistNodeKey::getMax());
        head->right = node;
        node->left = head;
    }

    void removeNode(SkiplistNode *target) {
        SkiplistNode *tempUp = target->up, *tempDown = target->down;
        while (tempUp) {
            removeLayerNode(tempUp);
            tempUp = tempUp->up;
        }
        while (tempDown) {
            removeLayerNode(tempDown);
            tempDown = tempDown->down;
        }
        removeLayerNode(target);
    }

    void removeLayerNode(SkiplistNode *target) {
        target->right->left = target->left;
        target->left->right = target->right;
        delete target;
    }

public:
    void test() {
        cout<<"i am exist"<<endl;
    }
    static void insertNode(SkiplistNode *point, SkiplistNode *target) {
        SkiplistNode *temp = point->right;
        point->right = target;
        target->right = temp;
        target->left = point;
        temp->left = target;
    }

    Skiplist(const Skiplist& another) {
        this->head = another.head;
        this->tail = another.tail;
        this->layer = another.layer;
        this->curLayerCount = another.curLayerCount;
    }

    Skiplist(vector<pair<SkiplistNodeKey , SkiplistNodeData>> datas): head(new SkiplistNode(SkiplistNodeData::getNull(), SkiplistNodeKey::getMin())), tail(new SkiplistNode(SkiplistNodeData::getNull(), SkiplistNodeKey::getMax())) {
        new (this)Skiplist();
        for(auto begin = datas.begin(); begin != datas.end(); begin++) {
            insert(begin->first, begin->second);
        }
    }

    Skiplist(map<SkiplistNodeKey ,SkiplistNodeData, SkiplistNodeTypeCompare> datas): head(new SkiplistNode(SkiplistNodeData::getNull(), SkiplistNodeKey::getMin())), tail(new SkiplistNode(SkiplistNodeData::getNull(), SkiplistNodeKey::getMax())) {
        new (this)Skiplist();
        for(auto it = datas.begin(); it != datas.end(); it++) {
            insert(it->first, it->second);
        }
    }

    Skiplist(): head(new SkiplistNode(SkiplistNodeData::getNull(), SkiplistNodeKey::getMin())), tail(new SkiplistNode(SkiplistNodeData::getNull(), SkiplistNodeKey::getMax())) {
        this->layer = 1;
        this->curLayerCount = 0;
        head->right = tail;
    }

    Skiplist& operator=(Skiplist &another) {
        this->head = another.head;
        this->tail = another.tail;
        this->layer = another.layer;
        this->curLayerCount = another.curLayerCount;
    }

    static Skiplist convert(map<SkiplistNodeKey, SkiplistNodeData, SkiplistNodeTypeCompare> datas){
        return Skiplist(datas);
    }

    static Skiplist convert(vector<pair<SkiplistNodeKey, SkiplistNodeData>> datas) {
        return Skiplist(datas);
    }

    SkiplistNodeData get(SkiplistNodeKey key) {
        SkiplistNode *top = this->head;
        int curLayer = 1;
        while (curLayer < this->layer) {
            curLayer++;
            top = top->up;
        }
        while (top != nullptr && top->key != key) {
            if (top->right->key == key) top = top->right;
            else if (top->key < key && top->right->key > key) {
                top = top->down;
            } else if (top->key < key && top->right->key < key) {
                top = top->right;
            } else if (top->key > key && top->left->key > key) {
                top = top->left;
            }
        }
        if (top == nullptr) {
            return SkiplistNodeType::getNull();
        }
        return top->data;
    }

    SkiplistNode *get(SkiplistNodeKey key, bool getNode) {
        SkiplistNode *top = this->head;
        int curLayer = 1;
        while (curLayer < this->layer) {
            curLayer++;
            top = top->up;
        }

        while (top != nullptr && top->key != key) {
            if (top->right->key == key) top = top->right;
            else if (top->key < key && top->right->key > key) {
                top = top->down;
            } else if (top->key < key && top->right->key < key) {
                top = top->right;
            } else if (top->key > key && top->left->key > key) {
                top = top->left;
            }
        }
        if (top == nullptr) { return nullptr; }
        if (getNode) { return top; }
        return nullptr;
    }

    void remove(SkiplistNodeKey key) {
        SkiplistNode *target = get(key, true);
        if(!target) return;
        removeNode(target);
    }

    void set(SkiplistNodeKey key, SkiplistNodeData data) {
        SkiplistNode *target = get(key, true);
        if(target) target->data = data;
        else insert(key, data);
    }

    void insert(SkiplistNodeKey key, SkiplistNodeData data) {
        SkiplistNode *node = get(key, true);
        int curLayer = 1;
        if (node) {
            node->data = data;
            return;
        }

        SkiplistNode *point = this->head, *end = this->tail, *target = nullptr;
        while (point != tail) {
            if (point->key < key && point->right->key > key) break;
            if (point->key < key) point = point->right;
        }
        if (point != tail) {
            target = new SkiplistNode(data, key);
            Skiplist::insertNode(point, target);
        }

        while (u(e) > 0.5) {
            if (curLayer < layer) {
                SkiplistNode *target = new SkiplistNode(data, key);
                SkiplistNode *upPoint;
                if(point == head) upPoint = point->up;
                else upPoint = point->left->up;
                Skiplist::insertNode(upPoint, target);
                curLayer++;
            } else {
                if (curLayer == 32) break;
                else {
                    layer += 1;
                    SkiplistNode *top = head, *newTop = nullptr, *newTarget = nullptr;
                    while (top->up) {
                        top = top->up;
                    }

                    newTop = insertNewLayerNode(top);
                    newTarget = insertNewLayerNode(target);
                    connectNewNodeToNewHead(newTop, newTarget);
                    break;
                }
            }
        }
    }
};

#endif