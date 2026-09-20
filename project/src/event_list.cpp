#include "event_list.h"
namespace nano_edr {
void ListPushBack(nano_edr::EventList* list, const nano_edr::Event* event){
    nano_edr::EventNode* new_node = new nano_edr::EventNode{*event,nullptr};
    if (list->capacity!=0 && list->size>=list->capacity){
        nano_edr::EventNode* old_node=list->head;
        list->head=list->head->next;
        delete old_node;
        list->size--;
        if (list->head==nullptr){list->tail=nullptr;}

    }
    
    if (list->head==nullptr){
        list->head=new_node;
        list->tail=new_node;
        list->size++;}
    else{list->tail->next=new_node;
    list->tail=new_node;
    list->size++;}
}
void ListPopFront(EventList* list){
    if (list->head==nullptr){return;}
    nano_edr::EventNode* delete_node=list->head;
    list->head=list->head->next;
    delete delete_node;
    list->size--;
    if (list->size==0){list->tail=nullptr;list->size=0;}
}
void ListClear(EventList* list){
    nano_edr::EventNode* current=list->head;
    while (current!=nullptr){
        nano_edr::EventNode* next=current->next;
        delete current;
        current=next;
    }
    list->head=nullptr;
    list->tail=nullptr;
    list->size=0;

}
}