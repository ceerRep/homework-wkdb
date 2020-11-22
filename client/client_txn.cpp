
#include "client_txn.h"
#include "mem_alloc.h"

void Inflight_entry::init() {
    num_inflight_txns = 0;
    sem_init(&mutex, 0, 1);
}

int32_t Inflight_entry::inc_inflight() {
    int32_t result;
    sem_wait(&mutex);
    if (num_inflight_txns < g_max_inflight) {
        result = ++num_inflight_txns;
    } else {
        result = -1;
    }
    sem_post(&mutex);
    return result;
}

int32_t Inflight_entry::dec_inflight() {
    int32_t result;
    sem_wait(&mutex);
    if(num_inflight_txns > 0) {
      result = --num_inflight_txns;
    }
    //assert(num_inflight_txns >= 0);
    sem_post(&mutex);
    return result;
}

int32_t Inflight_entry::get_inflight() {
    int32_t result;
    sem_wait(&mutex);
    result = num_inflight_txns;
    sem_post(&mutex);
    return result;
}

void Client_txn::init() {
    //inflight_txns = new Inflight_entry * [g_cnt_node]; 
    inflight_txns = new Inflight_entry * [g_servers_per_client]; 
    //for (uint32_t i = 0; i < g_cnt_node; ++i) {
    for (uint32_t i = 0; i < g_servers_per_client; ++i) {
        inflight_txns[i] = 
            (Inflight_entry *) alloc_memory.alloc(sizeof(Inflight_entry));
        inflight_txns[i]->init();
    }
}

int32_t Client_txn::inc_inflight(uint32_t node_id) {
    assert(node_id < g_servers_per_client);
    return inflight_txns[node_id]->inc_inflight();
}

int32_t Client_txn::dec_inflight(uint32_t node_id) {
    assert(node_id < g_servers_per_client);
    return inflight_txns[node_id]->dec_inflight();
}

int32_t Client_txn::get_inflight(uint32_t node_id) {
    assert(node_id < g_servers_per_client);
    return inflight_txns[node_id]->get_inflight();
}
