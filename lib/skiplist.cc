#include <limits.h>
#include <random>
#include <map>
#include <utility>
using namespace std;

template<class T, class K>
struct SkiplistNode {
    SkiplistNode(T data, K key) {
        this->data = data;
        this->key = key;
        this->up = nullptr;
        this->down = nullptr;
        this->left = nullptr;
        this->right = nullptr;
    }

    K key;
    T data;
    SkiplistNode<T, K> *up;
    SkiplistNode<T, K> *down;
    SkiplistNode<T, K> *left;
    SkiplistNode<T, K> *right;
};

template<class T, class K>
class Skiplist {
    SkiplistNode<T, K> *head;
    SkiplistNode<T, K> *tail;
    int layer;
    int curLayerCount;
    random_device rd;
    default_random_engine e = default_random_engine{rd()};
    uniform_real_distribution<double> u = uniform_real_distribution<double>(0.0, 1.0);

private:
    SkiplistNode<T, K> *insertNewLayerNode(SkiplistNode<T, K> *target) {
        SkiplistNode<T, K> *newTarget = new SkiplistNode<T, K>(target->data, target->key);
        target->up = newTarget;
        newTarget->down = target;
        return newTarget;
    }

    void connectNewNodeToNewHead(SkiplistNode<T,K> *head, SkiplistNode<T,K> *node) {
        node->right = new SkiplistNode<T, K>(NULL, INT_MAX);
        head->right = node;
        node->left = head;
    }

    void removeNode(SkiplistNode<T, K> *target) {
        SkiplistNode<T, K> *tempUp = target->up, *tempDown = target->down;
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

    void removeLayerNode(SkiplistNode<T, K> *target) {
        target->right->left = target->left;
        target->left->right = target->right;

        delete target;
    }

public:
    template<class I, class J>
    static void insertNode(SkiplistNode<I, J> *point, SkiplistNode<I, J> *target) {
        SkiplistNode<I, J> *temp = point->right;
        point->right = target;
        target->right = temp;
        target->left = point;
        temp->left = target;
    }

    Skiplist(vector<pair<K, T>> datas): head(new SkiplistNode<T, K>(NULL, -INT_MAX)), tail(new SkiplistNode<T, K>(NULL, INT_MAX)) {
        new (this)Skiplist();
        for(auto begin = datas.begin(); begin != datas.end(); begin++) {
            insert(begin->first, begin->second);
        }
    }

    Skiplist(map<K,T> datas): head(new SkiplistNode<T, K>(NULL, -INT_MAX)), tail(new SkiplistNode<T, K>(NULL, INT_MAX)) {
        new (this)Skiplist();
        for(auto it = datas.begin(); it != datas.end(); it++) {
            insert(it->first, it->second);
        }
    }
    Skiplist(): head(new SkiplistNode<T, K>(NULL, -INT_MAX)), tail(new SkiplistNode<T, K>(NULL, INT_MAX)) {
        this->layer = 1;
        this->curLayerCount = 0;
        head->right = tail;
    }

    Skiplist<T,K> convert(map<K, T> datas){
        return Skiplist(datas);
    }

    Skiplist<T,K> convert(pair<K,T> datas[]) {
        return Skiplist(datas);
    }

    T get(K key) {
        SkiplistNode<T, K> *top = this->head;
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
            return NULL;
        }
        return top->data;
    }

    SkiplistNode<T, K> *get(K key, bool getNode) {
        SkiplistNode<T, K> *top = this->head;
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

    void remove(K key) {
        SkiplistNode<T, K> *target = get(key, true);
        if(!target) return;
        removeNode(target);
    }

    void set(K key, T data) {
       SkiplistNode<T, K> *target = get(key, true);
       if(target) target->data = data;
       else insert(key, data);
    }

    void insert(K key, T data) {
        SkiplistNode<T, K> *node = get(key, true);
        int curLayer = 1;
        if (node) {
            node->data = data;
            return;
        }

        SkiplistNode<T, K> *point = this->head, *end = this->tail, *target = nullptr;
        while (point != tail) {
            if (point->key < key && point->right->key > key) break;
            if (point->key < key) point = point->right;
        }
        if (point != tail) {
            target = new SkiplistNode<T, K>(data, key);
            Skiplist<T, K>::insertNode<T, K>(point, target);
        }

        while (u(e) > 0.5) {
            if (curLayer < layer) {
                SkiplistNode<T, K> *target = new SkiplistNode<T,K>(data, key);
                SkiplistNode<T, K> *upPoint;
                if(point == head) upPoint = point->up;
                else upPoint = point->left->up;
                Skiplist<T, K>::insertNode<T, K>(upPoint, target);
                curLayer++;
            } else {
                if (curLayer == 32) break;
                else {
                    layer += 1;
                    SkiplistNode<T, K> *top = head, *newTop = nullptr, *newTarget = nullptr;
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

