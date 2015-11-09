/* CS 2200 - Project 5 - Fall 2015
 * Name -
 * GTID -
 */

#include "cachesim.h"
/**
 * Sub-routine for initializing your cache with the parameters.
 * You may initialize any global variables here.
 *
 * @param C The total size of your cache is 2^C bytes
 * @param S The set associativity is 2^S
 * @param B The size of your block is 2^B bytes
 */

void cache_init(uint64_t C, uint64_t S, uint64_t B) {
	int i, j, k;
	cache_line *current_line;
	cache_block *current_block;
	uint64_t num_lines = 1 << (C - B - S);
	uint64_t num_blocks_set = 1 << S;
	uint64_t block_size = 1 << B;
	
	my_cache = (cache*)malloc(sizeof(cache));
	my_cache -> c = C;
	my_cache -> b = B;
	my_cache -> s = S;
	my_cache -> num_lines = num_lines;
	my_cache -> block_size = block_size;
	my_cache -> num_blocks_set = num_blocks_set;
	my_cache -> lines = (cache_line*)calloc(num_lines, sizeof(cache_line));

	for (i = 0; i < num_lines;i++) {
		current_line = &(my_cache->lines[i]);
		current_line -> blocks = (cache_block*)calloc(num_blocks_set, sizeof(cache_block));
	}
}

/**
 * Subroutine that simulates one cache event at a time.
 * @param rw The type of access, READ or WRITE
 * @param address The address that is being accessed
 * @param stats The struct that you are supposed to store the stats in
 */
void cache_access (char rw, uint64_t address, struct cache_stats_t *stats) {
	int hit;
	uint64_t tag, line_number, offset, i;
	cache_line *current_line;
	cache_block *current_block; 
	cache_block *hit_block;
	stats -> accesses++;
	my_cache->timer++;
	tag = get_tag(address);
	line_number = get_line_number(address);
	offset = get_offset(address);

	current_line = &(my_cache->lines[line_number]);
	hit = 0;
	for (i = 0; i < my_cache -> num_blocks_set;i++) {
		current_block = &(current_line->blocks[i]);
		if (current_block->valid && current_block->tag == tag) {
			hit = 1;
			hit_block = current_block;
			hit_block->time_stamp = my_cache ->timer;
		}

	}
	if (rw == READ){
		stats->reads++;
	} else if (rw == WRITE) {
		stats->writes++;
	} 

	if (hit && rw == WRITE){
		hit_block -> dirty = 1;
	} else if (!hit) {
		handle_miss(current_line, tag, rw, stats);//get a victim from current line and replace it with the memory from the address.
	}
}

void handle_miss(cache_line *current_line, uint64_t tag, char rw, struct cache_stats_t *stats) {
	cache_block *ava_block = NULL;
	int i;
	stats -> misses++;
	for (i = 0; i < my_cache -> num_blocks_set;i++) {
		if (!(current_line -> blocks[i].valid)) {
			ava_block = &(current_line -> blocks[i]);
		}
	}
	if (ava_block == NULL) {
		ava_block = get_victim(current_line, tag, rw, stats);
	}
	if (rw == READ) {
		stats -> read_misses++;
		ava_block -> tag = tag;
		ava_block -> valid = 1;
		ava_block -> time_stamp = my_cache -> timer;
	} else if (rw == WRITE) {
		stats -> write_misses++;
		ava_block -> tag = tag;
		ava_block -> valid = 1;
		ava_block -> dirty = 1;
		ava_block -> time_stamp = my_cache -> timer; 
	}
}

cache_block* get_victim(cache_line *current_line, uint64_t tag, char rw, struct cache_stats_t *stats) {
	int i;
	cache_block *least_used;
	uint64_t least_used_time_stamp = INT_MAX;
	for (i = 0; i < my_cache -> num_blocks_set; i++) {
		if (current_line -> blocks[i].time_stamp <= least_used_time_stamp) {
			least_used = &(current_line -> blocks[i]);
			least_used_time_stamp = current_line -> blocks[i].time_stamp;
		}
	}
	if (least_used -> dirty) {
		stats -> write_backs++;
		least_used -> dirty = 0;
	}
	return least_used;
}

/**
 * Subroutine for cleaning up memory operations and doing any calculations
 * Make sure to free malloced memory here.
 *
 */
void cache_cleanup (struct cache_stats_t *stats) {
	int i;
	for (i = 0; i < my_cache -> num_lines; i++) {
		free(my_cache -> lines[i].blocks);
	}
	free(my_cache->lines);
	free(my_cache);
	stats->miss_rate = stats->misses /(1.0*stats->accesses);
	stats->avg_access_time = 2 + stats->miss_rate * 100; 
}

uint64_t get_tag(uint64_t address){
	uint64_t C = my_cache->c;
	uint64_t S = my_cache->s;

	return address >> (C - S);
}

uint64_t get_line_number(uint64_t address){
	uint64_t C = my_cache->c;
	uint64_t S = my_cache->s;
	uint64_t B = my_cache->b;

	return (address >> B) & ((1 << (C - S - B)) - 1);
}

uint64_t get_offset(uint64_t address){
	return address & ((1 << my_cache -> b) - 1);//get the right most b bits
}


