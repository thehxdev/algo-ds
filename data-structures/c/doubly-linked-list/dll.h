#ifndef DLL_H
#define DLL_H

#include <stdint.h>
#include <stdlib.h>
#include <memory.h>


struct __node {
    void *data;
    size_t size;
    struct __node *next;
    struct __node *prev;
};
typedef struct __node Node;


struct __dll {
    Node *head;
    Node *tail;
};
typedef struct __dll DLL_t;


/**
 * Create a new node
 */
Node *node_new(void *val, size_t size) {
    if (val == NULL || size == 0)
        return NULL;

    Node *nn = (Node*) malloc(sizeof(Node));
    if (nn == NULL)
        return NULL;

    nn->data = (void*) malloc(size);
    if (nn->data == NULL) {
        free(nn);
        return NULL;
    }

    memcpy(nn->data, val, size);
    nn->size = size;
    nn->next = NULL;
    nn->prev = NULL;

    return nn;
}


/**
 * Free a node's space from memory
 */
void node_free(Node *np) {
    if (np) {
        if (np->data) {
            free(np->data);
            np->data = NULL;
        }
        free(np);
        np = NULL;
    }
}


/**
 * Create a new DLL
 */
DLL_t *dll_new() {
    DLL_t *ndll = (DLL_t*) malloc(sizeof(DLL_t));
    if (ndll == NULL)
        return NULL;

    ndll->head = NULL;
    ndll->tail = NULL;

    return ndll;
}


/**
 * Add a new node to end of the DLL as TAIL
 */
int dll_append(DLL_t *dllp, void *val, size_t size) {
    int stat = 0;
    Node *nn = node_new(val, size);
    if (nn == NULL) {
        stat = 1;
        goto ret;
    }

    if (dllp->head == NULL) {
        dllp->head = nn;
        dllp->tail = nn;
        goto ret;
    }

    nn->prev = dllp->tail;
    dllp->tail->next = nn;
    dllp->tail = nn;

ret:
    return stat;
}


/**
 * Add a new node to beginning of the DLL as HEAD
 */
int dll_prepend(DLL_t *dllp, void *val, size_t size) {
    int stat = 0;
    Node *nn = node_new(val, size);
    if (nn == NULL) {
        stat = 1;
        goto ret;
    }

    if (dllp->head == NULL) {
        dllp->head = nn;
        dllp->tail = nn;
        goto ret;
    }

    dllp->head->prev = nn;
    nn->next   = dllp->head;
    dllp->head = nn;

ret:
    return stat;
}


/**
 * Find a node in DLL by it's index
 */
Node *dll_findNodeByIdx(DLL_t *dllp, size_t idx) {
    if (dllp == NULL)
        return NULL;

    Node *tmp = dllp->head;
    if (idx == 0)
        goto ret;

    for (uint32_t i = 1; i <= idx; i++) {
        tmp = tmp->next;
        if (tmp == NULL)
            return NULL;
    }

ret:
    return tmp;
}


/**
 * Find a node in DLL by it's value
 */
Node *dll_findNodeByVal(DLL_t *dllp, void *val, size_t size) {
    if (dllp == NULL)
        return NULL;

    Node *tmp = dllp->head;
    while (tmp != NULL) {
        if (tmp->size != size) {
            tmp = tmp->next;
            continue;
        }

        if (memcmp(tmp->data, val, size) == 0)
            return tmp;

        tmp = tmp->next;
    }

    return NULL;
}


/**
 * Insert a new node to DLL with a specific index
 */
int dll_insertAtIdx(DLL_t *dllp, size_t idx, void *val, size_t size) {
    int stat = 0;
    if (dllp == NULL || val == NULL || size == 0) {
        stat = 1;
        goto ret;
    }

    if (idx == 0) {
        stat = dll_prepend(dllp, val, size);
        goto ret;
    }

    Node *tmp = dll_findNodeByIdx(dllp, idx);
    if (tmp == NULL) {
        stat = 1;
        goto ret;
    }

    if (tmp->next == NULL) {
        stat = dll_append(dllp, val, size);
        goto ret;
    }

    Node *nn = node_new(val, size);
    if (nn == NULL) {
        stat = 1;
        goto ret;
    }

    nn->next = tmp;
    nn->prev = tmp->prev;
    tmp->prev->next = nn;
    tmp->prev = nn;

    if (nn->next == NULL)
        dllp->tail = nn;

ret:
    return stat;
}


/**
 * Insert a new node after a specific Node in the DLL
 */
int dll_insertAfterNode(DLL_t *dllp, Node *np, void *val, size_t size) {
    int stat = 0;
    if (dllp == NULL || np == NULL || val == NULL || size == 0) {
        stat = 1;
        goto ret;
    }
    if (dllp->head == NULL) {
        stat = 1;
        goto ret;
    }

    Node *nn = node_new(val, size);
    if (nn == NULL) {
        stat = 1;
        goto ret;
    }

    nn->next = np->next;
    nn->prev = np;
    np->next->prev = nn;
    np->next = nn;

    if (nn->next == NULL)
        dllp->tail = nn;

ret:
    return stat;
}


/**
 * delete a value from DLL by it's value
 */
int dll_deleteByVal(DLL_t *dllp, void *val, size_t size) {
    int stat = 0;
    if (dllp == NULL || val == NULL || size == 0) {
        stat = 1;
        goto ret;
    }
    if (dllp->head == NULL) {
        stat = 1;
        goto ret;
    }

    Node *tmp = dll_findNodeByVal(dllp, val, size);

    if (tmp->prev == NULL && tmp->next == NULL) {
        dllp->head = NULL;
        dllp->tail = NULL;
    }
    else if (tmp->prev == NULL && tmp->next != NULL) {
        dllp->head = tmp->next;
        tmp->next->prev = NULL;
    }
    else if (tmp->prev != NULL && tmp->next == NULL) {
        dllp->tail = tmp->prev;
        tmp->prev->next = NULL;
    }
    else {
        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
    }

    node_free(tmp);
ret:
    return stat;
}


/**
 * Delete a node from DLL by it's index
 */
int dll_deleteByIdx(DLL_t *dllp, size_t idx) {
    int stat = 0;
    if (dllp == NULL) {
        stat = 1;
        goto ret;
    }
    if (dllp->head == NULL) {
        stat = 1;
        goto ret;
    }
    Node *tmp = dll_findNodeByIdx(dllp, idx);
    if (tmp == NULL) {
        stat = 1;
        goto ret;
    }

    if (tmp->prev == NULL) {
        if (tmp->next != NULL) {
            tmp->next->prev = NULL;
            dllp->head = tmp->next;
            goto free_ret;
        }
        dllp->head = NULL;
        dllp->tail = NULL;
        goto free_ret;
    }

    tmp->prev->next = tmp->next;
    if (tmp->next) {
        tmp->next->prev = tmp->prev;
    }

free_ret:
    node_free(tmp);
ret:
    return stat;
}


/**
 * Delete a node from DLL by it's memory address
 */
int dll_deleteByAddr(DLL_t *dllp, Node *np) {
    int stat = 0;
    if (dllp == NULL || np == NULL) {
        stat = 1;
        goto ret;
    }
    if (dllp->head == NULL || dllp->tail == NULL) {
        stat = 1;
        goto ret;
    }

    /*
     * check if np is the only node in DLL
     * (it's also head and tail)
     */
    if (np->prev == NULL && np->next == NULL) {
        dllp->head = NULL;
        dllp->tail = NULL;
    }
    else if (np->prev == NULL && np->next != NULL) {
        np->next->prev = NULL;
        dllp->head = np->next;
    }
    else if (np->prev != NULL && np->next == NULL) {
        np->prev->next = NULL;
        dllp->tail = np->next;
    }
    else {
        np->prev->next = np->next;
        np->next->prev = np->prev;
    }
    node_free(np);

ret:
    return stat;
}


/**
 * Free a doubly-linked-list from memory and 
 * it's nodes.
 */
void dll_free(DLL_t *dllp) {
    if (dllp != NULL) {
        Node *tmp = dllp->head;
        if (tmp == NULL)
            goto only_list;

        while (1) {
            if (tmp->prev != NULL)
                free(tmp->prev);

            if (tmp->next == NULL) {
                node_free(tmp);
                break;
            }

            free(tmp->data);
            tmp = tmp->next;
        }
only_list:
        free(dllp);
        dllp = NULL;
    }
}


#endif // DLL_H

