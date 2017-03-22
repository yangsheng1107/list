#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "list.h"

struct foo {
    int info;
    struct list_head list;
};

void add_node(int arg, struct list_head *head)
{
    struct foo *newNode = (struct foo *)malloc(sizeof(struct foo));
    assert(newNode != NULL);

    newNode->info = arg;
    INIT_LIST_HEAD(&newNode->list);
    list_add(&newNode->list, head);
}

void add_node_tail(int arg, struct list_head *head)
{
    struct foo *newNode = (struct foo *)malloc(sizeof(struct foo));
    assert(newNode != NULL);

    newNode->info = arg;
    INIT_LIST_HEAD(&newNode->list);
    list_add_tail(&newNode->list, head);
}


void display(struct list_head *head)
{
    struct list_head *iter;
    struct foo *Node;

    list_for_each(iter, head) {
        Node = list_entry(iter, struct foo, list);
        printf("%d ", Node->info);
    }
    printf("\n");
}

void delete_all(struct list_head *head)
{
    struct list_head *iter;
    struct foo *Node;

redo:
    list_for_each(iter, head) {
        Node = list_entry(iter, struct foo, list);
        list_del(&Node->list);
        free(Node);
        goto redo;
    }
}

int find_first_and_delete(int arg, struct list_head *head)
{
    struct list_head *iter;
    struct foo *Node;

    list_for_each(iter, head) {
        Node = list_entry(iter, struct foo, list);
        if(Node->info == arg) {
            list_del(&Node->list);
            free(Node);
            return 1;
        }
    }

    return 0;
}

void signalHandler(int signum, siginfo_t *siginfo, void *context)
{
    pid_t sender_pid  = siginfo->si_pid;

    printf("signum = %d from %d", signum, sender_pid);

    exit(signum);
}

int signalSetup()
{
    struct sigaction sa_usr;
    int signTable[] = { SIGINT, SIGQUIT, SIGSEGV, SIGTERM, -1 };  //2,3,11,15
    int i           = 0;

    // prepare sigaction
    memset(&sa_usr, 0x00, sizeof(sa_usr));
    sa_usr.sa_sigaction = signalHandler;
    sa_usr.sa_flags     = SA_SIGINFO; // get detail info

    while (signTable[i] != -1)
    {
        if (sigaction(signTable[i], &sa_usr, NULL) < 0)
        {
            char errMsg[32] = { 0x00 };
            snprintf(errMsg, sizeof(errMsg) - 1, "sigaction : %d", signTable[i]);
            perror(errMsg);
            return -1;
        }
        i++;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    struct list_head fooHead = LIST_HEAD_INIT(fooHead);
    // 或用 LIST_HEAD
    //LIST_HEAD(fooHead);
    if(signalSetup() < 0 ) {
        printf("error\r\n");
        return 0;
    }

    add_node(10, &fooHead);
    add_node(20, &fooHead);
    add_node(25, &fooHead);
    add_node(30, &fooHead);
    add_node_tail(5, &fooHead);

    display(&fooHead);
    find_first_and_delete(20, &fooHead);
    display(&fooHead);
    delete_all(&fooHead);
    display(&fooHead);
    return 0;
}