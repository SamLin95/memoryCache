/**********************************************
****** 		DO NOT MODIFY THIS FILE		*******
***********************************************/

#ifndef CACHESIM_H
#define CACHESIM_H

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* The statistics struct */
struct cache_stats_t {
    uint64_t accesses;
    uint64_t reads;
    uint64_t read_misses;
    uint64_t writes;
    uint64_t write_misses;
    uint64_t misses;
	uint64_t write_backs;
   
	uint64_t access_time;
    uint64_t miss_penalty;
    double   miss_rate;
    double   avg_access_time;
};


typedef struct {
    /* data */
    int valid;
    int dirty;
    uint64_t tag;
    uint64_t time_stamp; 
    void *data;
} cache_block;


typedef struct {
    /* data */
    cache_block *blocks;
    uint64_t lru;
} cache_line;

typedef struct {
    /* data */
    cache_line *lines;
    uint64_t c;
    uint64_t b;
    uint64_t s;
    uint64_t num_lines;
    uint64_t block_size;
    uint64_t num_blocks_set;
    uint64_t timer;
} cache;

void cache_init(uint64_t C, uint64_t S, uint64_t B);
void cache_access (char rw, uint64_t address, struct cache_stats_t *stats);
void cache_cleanup (struct cache_stats_t *stats);
uint64_t get_tag(uint64_t address);
uint64_t get_line_number(uint64_t address);
uint64_t get_offset(uint64_t get_offset);
void handle_miss(cache_line *current_line, uint64_t tag, char rw, struct cache_stats_t *stats);
cache_block* get_victim(cache_line *current_line, uint64_t tag, char rw, struct cache_stats_t *stats);

static const uint64_t DEFAULT_C = 15;   /* 32KB Cache */
static const uint64_t DEFAULT_B = 5;    /* 32-byte blocks */
static const uint64_t DEFAULT_S = 3;    /* 8 blocks per set */

static const char READ = 'r';
static const char WRITE = 'w';

cache *my_cache;


#endif
