/*
* Author: asciibase64
*/

#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>

#define BUF_SIZE 20

typedef struct _cacheLine
{
    bool valid;
    unsigned int tag;
    char *bytes;    // We don't actually need this variable here, since we just simulate the operation.
    unsigned int callTime;
}cacheLine;

typedef struct _cacheSet
{
    cacheLine *line;
}cacheSet;

typedef struct _dataCache
{
    unsigned int s;
    unsigned int E;
    unsigned int b;
    cacheSet *set;
}dataCache;

void ShowHelp(){
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file> \n");
    printf("Options: \n");
    printf("  -h\t\tPrint this help message.\n");
    printf("  -v\t\tOptional verbose flag.\n");
    printf("  -s <num>\tNumber of set index bits.\n");
    printf("  -E <num>\tNumber of lines per set.\n");
    printf("  -b <num>\tNumber of block offset bits.\n");
    printf("  -t <file>\tTrace file.\n");
    printf("\n");
    printf("Examples: \n");
    printf("  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

void PrintCache(dataCache *cache) {
    printf("****** Cache Info ******\n");
    for (int i = 0; i < (1 << cache->s); i++) {
        printf("set%d: \n", i);
        for (int j = 0; j < cache->E; j++)
            printf("\tline%d: %d %d %s %d\n", j, cache->set[i].line[j].valid, cache->set[i].line[j].tag, cache->set[i].line[j].bytes, cache->set[i].line[j].callTime);
    }
    printf("************************\n");
}

unsigned int FindTag(dataCache *cache, char *addr) {
    unsigned int d_addr = strtol(addr, NULL, 16);

    // Recall: addr_{bin} = tag - set - byte
    // Get tag => right shift (set + byte) times
    unsigned int tag = d_addr >> ((cache->s) + (cache->b));

    return tag;
}

unsigned int FindSet(dataCache *cache, char *addr) {
    unsigned int d_addr = strtol(addr, NULL, 16);

    // Recall: addr_{bin} = tag - set - byte
    // Get set => mask + right shift
    unsigned int mask = (1 << ((cache->s) + (cache->b))) - 1;
    unsigned int set = (d_addr & mask) >> (cache->b);

    return set;
}

unsigned int FindLine(dataCache *cache, unsigned int set, unsigned int tag, unsigned int timer) {
    unsigned int line = -1;
    for (int i = 0; i < cache->E; i++) {
        if (cache->set[set].line[i].tag == tag && cache->set[set].line[i].valid) {
            line = i;
            cache->set[set].line[i].callTime = timer;
        }            
    }
    return line; 
}

bool IsSetFull(dataCache *cache, unsigned int set) {
    bool flag = true;
    for (int i = 0; i < cache->E; i++) {
        if (!cache->set[set].line[i].valid)
            flag = false;
    }
    return flag;
}

void StoreCache(dataCache *cache, unsigned int set, unsigned int tag, unsigned int timer) {
    // Because we checked IsSetFull() before this function, 
    // we can believe that there's always a blank line prepared for us. 
    for (int i = 0; i < cache->E; i++) {
        if (cache->set[set].line[i].valid == false) {
            cache->set[set].line[i].valid = true;
            cache->set[set].line[i].tag = tag;
            cache->set[set].line[i].callTime = timer;
            break;
        }
    }
}

void EvictLine(dataCache *cache, unsigned int set) {
    // We use least-recently used replacement policy to evict a line. 
    unsigned int minIndex = 0;
    for (int i = 0; i < cache->E; i++)
        minIndex = cache->set[set].line[i].callTime < cache->set[set].line[minIndex].callTime ? i : minIndex;

    cache->set[set].line[minIndex].valid = false;
}

void ShowDetail(char *op, char *addr, char *bytes, char *msg) {
    printf("%s %s %s %s", op, addr, bytes, msg);
}

bool Simulate(char *tracefile, dataCache *cache, bool v, int *hit_count, int *miss_count, int *evict_count) {
    FILE *file = fopen(tracefile, "r");
    char buf[BUF_SIZE];

    char *op, *addr, *bytes; // We have to split each line in tracefile to three part. 

    if(file == NULL) {
        printf("%s: No such file or directory.\n", tracefile);
        return false;
    }

    // In order to implement LRU, we have to set a timer and store it to line[i].callTime
    unsigned int timer = 0;

    while(fgets(buf, BUF_SIZE, file) != NULL) {
        // printf("%s", buf);
        if (buf[0] == 'I')
            continue;;
            
        timer++;

        op = strtok(buf, " ");
        addr = strtok(NULL, ",");
        bytes = strtok(NULL, " \n");
        
        unsigned int set, tag, line;
        set = FindSet(cache, addr);
        tag = FindTag(cache, addr);

        switch (*op)
        {
        case 'L':
            if ( (line = FindLine(cache, set, tag, timer)) != -1) {
                *hit_count += 1;
                if (v) {
                    ShowDetail(op, addr, bytes, "hit ");
                }
            }
            else {
                *miss_count += 1;
                if (!IsSetFull(cache, set)) {
                    StoreCache(cache, set, tag, timer);
                    if (v) {
                        ShowDetail(op, addr, bytes, "miss ");
                    }
                }                    
                else {
                    *evict_count += 1;
                    EvictLine(cache, set);
                    StoreCache(cache, set, tag, timer);
                    if (v) {
                        ShowDetail(op, addr, bytes, "miss eviction ");
                    }
                }
            }
            if (v)
                printf("\n");
            break;
        case 'S':
            if ( (line = FindLine(cache, set, tag, timer)) != -1) {
                *hit_count += 1;
                if (v) {
                    ShowDetail(op, addr, bytes, "hit ");
                }
            }
            else {
                *miss_count += 1;
                if (!IsSetFull(cache, set)) {
                    StoreCache(cache, set, tag, timer);
                    if (v) {
                        ShowDetail(op, addr, bytes, "miss ");
                    }
                }                    
                else {
                    *evict_count += 1;
                    EvictLine(cache, set);
                    StoreCache(cache, set, tag, timer);
                    if (v) {
                        ShowDetail(op, addr, bytes, "miss eviction ");
                    }
                }
            }
            if (v)
                printf("\n");
            break;
        case 'M':
            if ( (line = FindLine(cache, set, tag, timer)) != -1) {
                *hit_count += 1;
                if (v) {
                    ShowDetail(op, addr, bytes, "hit ");
                }
            }
            else {
                *miss_count += 1;
                if (!IsSetFull(cache, set)) {
                    StoreCache(cache, set, tag, timer);
                    if (v) {
                        ShowDetail(op, addr, bytes, "miss ");
                    }
                }                    
                else {
                    *evict_count += 1;
                    EvictLine(cache, set);
                    StoreCache(cache, set, tag, timer);
                    if (v) {
                        ShowDetail(op, addr, bytes, "miss eviction ");
                    }
                }
            }

            if ( (line = FindLine(cache, set, tag, timer)) != -1) {
                *hit_count += 1;
                if (v) {
                    printf("hit ");
                }
            }
            else {
                *miss_count += 1;
                if (!IsSetFull(cache, set)) {
                    StoreCache(cache, set, tag, timer);
                    if (v) {
                        printf("miss ");
                    }
                }                    
                else {
                    *evict_count += 1;
                    EvictLine(cache, set);
                    StoreCache(cache, set, tag, timer);
                    if (v) {
                        printf("miss eviction ");
                    }
                }
            }
            if (v)
                printf("\n");
            break;
        default:
            break;
        }
    }

    fclose(file);

    return true;
}

int main(int argc, char *argv[])
{
    int hit_count = 0, miss_count = 0, eviction_count = 0;

    // Variables for command line parsing
    int opt;
    bool v = false;
    int s, E, b;
    char *tracefile;

    if (argc == 2 && argv[1][1] == 'h') {
        ShowHelp();
        return 0;
    }

    if(argc != 10 && argc != 9) {
        printf("./csim: Missing required command line argument\n");
        ShowHelp();
        return -1;
    }

    // Command line parsing
    while( (opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch(opt) {
        case 'h':
            ShowHelp();
            return 0;
        case 'v':
            v = true;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            tracefile = optarg;
            break;
        default:
            break;
        }
    }

    unsigned int S = 1 << s;
    // Initialize cache for simulation
    dataCache cache;
    cache.s = s;
    cache.E = E;
    cache.b = b;
    cache.set = malloc(sizeof(cacheSet) * S);
    for (int i = 0; i < S; i++) {
        cache.set[i].line = malloc(sizeof(cacheLine) * E);
        cache.set[i].line->valid = false;
        cache.set[i].line->tag = -1;
        cache.set[i].line->callTime = 0;
    }

    if (!Simulate(tracefile, &cache, v, &hit_count, &miss_count, &eviction_count))
        return -1;

    printSummary(hit_count, miss_count, eviction_count);

    free(cache.set->line);
    free(cache.set);
    
    return 0;
}