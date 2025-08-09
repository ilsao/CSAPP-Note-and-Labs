/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * We use segregated free list for better performance. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "DoomNoob",
    /* First member's full name */
    "asciibase64",
    /* First member's email address */
    "asciibase64.cs14@nycu.edu.tw",
    /* Second member's full name (leave blank if none) */
    "ilsao",
    /* Second member's email address (leave blank if none) */
    "ilsao@outlook.com(actually both are me)"
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* basic constants */
#define WSIZE 4
#define DSIZE 8
#define CLASS_SIZE 32

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

/* extend heap by this amout */
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define MAX(x, y)           ((x) > (y) ? (x) : (y))
#define MIN(x, y)           ((x) > (y) ? (y) : (x))

/* pack a size and allocated bit into a word */
#define PACK(size, alloc)   ((size) | (alloc))

/*read and write a word in p */
#define GET(p)          (*(unsigned int *) (p))
#define PUT(p, val)     (*(unsigned int *) (p) = (val))

/* read the size and allocated bields from address p (that point to a block header) */
#define GETSIZE(p)  (GET(p) & (~0x7))
#define GETALLOC(p) (GET(p) & 0x1)

/* given block ptr bp, compute address of its header and footer */
/* we have to convert bp to (char *), this ensure pointer arithematic to btye level */
#define HDRP(bp) ((char *) (bp) - WSIZE)
#define FTRP(bp) ((char *) (bp) + GETSIZE(HDRP(bp)) - 2 * WSIZE)

/* find next and previous block in the heap */
#define PREV_BLKP(bp) ((char *) (bp) - GETSIZE(HDRP(bp) - WSIZE))
#define NEXT_BLKP(bp) ((char *) (bp) + GETSIZE(HDRP(bp)))

/* for each given index, return free_list[i] */
#define GETLIST(n) (*((void **) ((char *) (heap_listp + (n) * WSIZE))))

/* find next and previous blocks in the segregated free list */
#define LIST_PREVP(bp) (*((void **) (bp)))
#define LIST_NEXTP(bp) (*((void **) ((char *) (bp) + WSIZE)))

static char *heap_listp;

int mm_check();

/*
* find_class - Find the match class for the free block
*/
static unsigned int find_class(size_t size)
{
    size_t counter = 0;
    size_t i;
    for (i = 1; i <= CLASS_SIZE - 1; i++) {
        if (size <= (1ULL << i))
            return counter;
        counter++;
    }
    return CLASS_SIZE - 1;
}

/*
* insert_list - Insert the block to the proper position in the list
* Return: 
*   0:  success
*   -1: failed
*/
static int insert_list(void *bp)
{
    if (GETALLOC(HDRP(bp))) {
        printf("[-] trying to insert allocated block to free list\n");
        return -1;
    }

    unsigned int class = find_class(GETSIZE(HDRP(bp)));

    /* LIFO */
    void *next = GETLIST(class);
    GETLIST(class) = bp;
    LIST_NEXTP(bp) = next;
    LIST_PREVP(bp) = NULL;
    if (next)
        LIST_PREVP(next) = bp;
    
    return 0;
}

/*
* remove_list - Remove a specific block bp from the corresponding class
* CAUTION: ! ! ! MUST CALL insert_list BEFORE CALLING remove_list ! ! !
* Return: 
*   0:  success
*   -1: failed
*/
static int remove_list(char *bp)
{
    if (GETALLOC(HDRP(bp))) {
        printf("[-] trying to remove allocated block from free list\n");
        return -1;
    }

    unsigned int class = find_class(GETSIZE(HDRP(bp)));

    if (GETLIST(class) == NULL) {
        printf("[-] remove_list error: class: %d is NULL\n", class);
        return -1;
    }

    /* 
    * connect prev and next blocks to delete bp
    * since we will coalesce two blocks, we don't need to free(bp)
    */
    void *prev, *next;
    prev = LIST_PREVP(bp);
    next = LIST_NEXTP(bp);
    if (prev)
        LIST_NEXTP(prev) = next;
    else
        GETLIST(class) = next;
    if (next)
        LIST_PREVP(next) = prev;
    LIST_NEXTP(bp) = NULL;
    LIST_PREVP(bp) = NULL;
    return 0;
}

/*
* coalesce - Coalesce prev and next blocks if possible
*/
static void *coalesce(char *bp)
{
    /* be cautious of cases where the bp class has changed after coalesce */
    unsigned int prev_alloc = GETALLOC(FTRP(PREV_BLKP(bp)));
    unsigned int next_alloc = GETALLOC(HDRP(NEXT_BLKP(bp)));
    char *prev = PREV_BLKP(bp), *next = NEXT_BLKP(bp);

    size_t size = GETSIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {
        insert_list(bp);
        return bp;
    }
    else if (prev_alloc && !next_alloc) {
        remove_list(next);
        size += GETSIZE(HDRP(next));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        insert_list(bp);
    }
    else if (!prev_alloc && next_alloc) {
        remove_list(prev);
        size_t prev_s = GETSIZE(FTRP(prev));
        bp -= prev_s;
        size += prev_s;
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        insert_list(bp);
    }
    else {
        remove_list(prev);
        remove_list(next);
        size_t prev_s = GETSIZE(FTRP(prev));
        bp -= prev_s;
        size += prev_s + GETSIZE(HDRP(next));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        insert_list(bp);
    }
    return bp;
}

/*
* extend_heap - Ask kernel for extra heap space
* * Return: 
*   Addr:   success
*     -1:   failed
*/
static void *extend_heap(size_t words)
{
    size_t size; 
    /* Allocate an even number to maintain alignment */
    size = !(words % 2) ? (words * WSIZE) : ((words + 1) * WSIZE);
    char *bp;
    if ((long) (bp = mem_sbrk(size)) == -1)     /* extend the heap */
        return (void *)-1;
    PUT(HDRP(bp), PACK(size, 0));               /* header */
    PUT(FTRP(bp), PACK(size, 0));               /* footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));       /* epilogue */

    return coalesce(bp);
}

/*
* find_fit - Find the fit free block in the free list
*/
static void *find_fit(size_t asize)
{
    unsigned int class = find_class(asize);

    size_t result_size = (1ULL << 31);
    size_t min_split = 16;

    void *cur;
    void *result = NULL;

    while (class < CLASS_SIZE) {
        cur = GETLIST(class);
        while (cur) {
            if (GETSIZE(HDRP(cur)) == asize || GETSIZE(HDRP(cur)) >= asize + min_split) {
                if (GETSIZE(HDRP(cur)) < result_size) {
                    result_size = GETSIZE(HDRP(cur));
                    result = cur;
                }
            }
            cur = LIST_NEXTP(cur);
        }
        class++;
    }

    return result;
}

static void place(void *bp, size_t asize)
{
    remove_list(bp);
    size_t prev_size = GETSIZE(HDRP(bp));

    /* split if remain > minimum block size */
    if (prev_size - asize >= 2 * DSIZE) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(prev_size - asize, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(prev_size - asize, 0));

        insert_list(NEXT_BLKP(bp));
    }
    /* use entire block */
    else {
        PUT(HDRP(bp), PACK(prev_size, 1));
        PUT(FTRP(bp), PACK(prev_size, 1));
    }
}

/* 
 * mm_init - Initialize the malloc package.
 * we organize our heap like this:
 *    padding
 *    free_listp[0]
 *    ...
 *    free_listp[19]
 *    prologue
 *    payload
 *    padding
 *    epilogue
 */
int mm_init(void)
{
    //                    (4)  (class size * 4)  (2 * 4)    (4)
    /* initialize like: padding   fre_listps     prologue epilogue */
    if ((heap_listp = mem_sbrk(CLASS_SIZE * WSIZE + 4 * WSIZE)) == (void *)-1)
        return -1;

    PUT(heap_listp, 0);                                         /* Alignment padding */
    int i;
    for (i = 0; i < CLASS_SIZE; i++) {                          /* segregated free list pointers */
        PUT(heap_listp + 4 + (i * WSIZE), 0);
    }
    PUT(heap_listp + 4 + (CLASS_SIZE * WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + 8 + (CLASS_SIZE * WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + 12 + (CLASS_SIZE * WSIZE), PACK(0, 1));    /* Epilogue header */

    heap_listp += WSIZE;
    if (extend_heap(2 * DSIZE / WSIZE) == (void *)-1)
        return -1;
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;       /* adjusted block size */
    char *bp;

    /* ignore suprious requests */
    if (size <= 0)
        return NULL;

    /* adjust block size to include overhead and alignment requests */
    if (size <= DSIZE) 
        asize = 2 * DSIZE;  /* minimum block size = ALIGN(header + footer + payload) = 16 */
    else
        asize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);
    
    /* search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    /* no fit found. Get more memory and place the block */
    if ((bp = extend_heap(asize / WSIZE)) == (void *)-1)
        return NULL;
    place(bp, asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    if (ptr == NULL)
        return;

    size_t size = GETSIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    coalesce(ptr);
}

/*
 * mm_realloc - resize the allocated block
 */
void *mm_realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
        return mm_malloc(size);
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    size_t prev_size = GETSIZE(HDRP(ptr)), asize;
    void *newptr;

    if (size <= DSIZE) 
        asize = 2 * DSIZE;  /* minimum block size = ALIGN(header + footer + payload) = 16 */
    else
        asize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);
    
    newptr = ptr;
    
    unsigned int prev_alloc = GETALLOC(FTRP(PREV_BLKP(newptr)));
    unsigned int next_alloc = GETALLOC(HDRP(NEXT_BLKP(newptr)));

    /* resize on place */
    if (prev_size >= asize) {
        /* split if remain > minimum block size */
        if (prev_size - asize >= 2 * DSIZE) {
            PUT(HDRP(newptr), PACK(asize, 1));
            PUT(FTRP(newptr), PACK(asize, 1));
            PUT(HDRP(NEXT_BLKP(newptr)), PACK(prev_size - asize, 0));
            PUT(FTRP(NEXT_BLKP(newptr)), PACK(prev_size - asize, 0));

            coalesce(NEXT_BLKP(newptr));
        }
        /* use entire block */
        else {
            PUT(HDRP(newptr), PACK(prev_size, 1));
            PUT(FTRP(newptr), PACK(prev_size, 1));
        }
    }
    /* coalesce if next block is free */
    else if (prev_alloc && !next_alloc && prev_size + GETSIZE(HDRP(NEXT_BLKP(newptr))) >= asize) {
        remove_list(NEXT_BLKP(newptr));
        prev_size += GETSIZE(HDRP(NEXT_BLKP(newptr)));
        if (prev_size - asize >= 2 * DSIZE) {
            PUT(HDRP(newptr), PACK(asize, 1));
            PUT(FTRP(newptr), PACK(asize, 1));
            PUT(HDRP(NEXT_BLKP(newptr)), PACK(prev_size - asize, 0));
            PUT(FTRP(NEXT_BLKP(newptr)), PACK(prev_size - asize, 0));
    
            insert_list(NEXT_BLKP(newptr));
        }
        /* use entire block */
        else {
            PUT(HDRP(newptr), PACK(prev_size, 1));
            PUT(FTRP(newptr), PACK(prev_size, 1));
        }
    }
    else {
        /* resize by finding another free block */
        if ((newptr = mm_malloc(asize)) == NULL)
            return NULL;
        memcpy(newptr, ptr, prev_size - 2 * WSIZE);
        mm_free(ptr);
    }

    return newptr;
}

/*
* mm_check - Check for program correctness
* Return: 
*   0:  success
*   -1: failed
*/
int mm_check()
{
    int result = 0;

    /* check 1: Do pointer prev and next symmetric? */
    void *first_block = NEXT_BLKP((char *)heap_listp + CLASS_SIZE * WSIZE);
    void *prev, *cur;
    for (int i = 0; i < CLASS_SIZE; i++) {
        prev = GETLIST(i);
        if (prev == NULL)   continue;
        cur = LIST_NEXTP(prev);
        while (cur) {
            if (LIST_NEXTP(prev) != cur || LIST_PREVP(cur) != prev) {
                printf("check 1 failed: pointer prev and next are not sysmetric\n");
                result = -1;
            }
            prev = cur;
            cur = LIST_NEXTP(cur);
        }
    }

    /* check 2: Are the information in the header and footer the same? */
    cur = first_block;
    size_t count = 0;
    while (GETSIZE(HDRP(cur)) > 0) {
        count++;
        if (GETSIZE(HDRP(cur)) != GETSIZE(FTRP(cur))) {
            printf("[-] check 2 failed: the block sizes stored in the header and footer are different\n");
            printf("[-] count: %d\n", count);
            result = -1;
        }
        if (GETALLOC(HDRP(cur)) != GETALLOC(FTRP(cur))) {
            printf("[-] check 2 failed: the allocated bit stored in the header and footer are different\n");
            printf("[-] count: %d\n", count);
            result = -1;
        }
        cur = NEXT_BLKP(cur);
    }

    /* check 3: Is every block in the free list marked as free? */
    for (int i = 0; i < CLASS_SIZE; i++) {
        cur = GETLIST(i);
        while (cur) {
            if (GETALLOC(HDRP(cur))) {
                printf("[-] Check 3 failed: blocks in class %d aren't all free\n", i);
                result = -1;
            }
            cur = LIST_NEXTP(cur);
        }
    }

    /* check 4: Are there any contiguous free blocks that somehow escaped coalescing? */
    prev = first_block;
    cur = NEXT_BLKP(prev);
    while (GETSIZE(HDRP(cur)) > 0) {
        if (!GETALLOC(HDRP(prev)) && !GETALLOC(HDRP(cur))) {
            printf("[-] check 4 failed: contiguous free blocks escaped coalescing\n");
            result = -1;
        }
        prev = cur;
        cur = NEXT_BLKP(cur);
    }

    /* check 5: Is every free block actually in the free list? */
    cur = first_block;
    while (GETSIZE(HDRP(cur)) > 0) {     /* size of epilogue = 0 */
        if (!GETALLOC(HDRP(cur))) {      /* find free block */
            int ok = 0;
            void *it = GETLIST(find_class(GETSIZE(HDRP(cur))));
            while (it) {
                if (it == cur)
                    ok = 1;
                it = LIST_NEXTP(it);
            }
            if (!ok) {
                printf("[-] check 5 failed: found a free block that is not in the free list\n");
                result = -1;
            }
        }
        cur = NEXT_BLKP(cur);
    }

    /* check 6: Do any allocated blocks overlap? */
    /* this loop can coalesce into check 4. For clarity, we have chosen to write separatly */
    prev = first_block;
    cur = NEXT_BLKP(prev);
    while (GETSIZE(HDRP(cur)) > 0) {
        if ((char *)cur < (char *)prev + GETSIZE(HDRP(prev))) {
            printf("[-] check 6 failed: blocks overlapped\n");
            result = -1;
        }
        prev = cur;
        cur = NEXT_BLKP(cur);
    }

    /* check 7: Are blocks in the heap range? */
    cur = first_block;
    while (GETSIZE(HDRP(cur)) > 0) {
        if (cur <= mem_heap_lo() || cur > mem_heap_hi() + 1) {
            printf("[-] check 7 failed: blocks out of heap range\n");
            printf("[-] cur: %p, mem_heap_lo: %p, mem_heap_hi + 1: %p\n", cur, mem_heap_lo, mem_heap_hi+1);
            result = -1;
        }
        cur = NEXT_BLKP(cur);
    }

    if (result == 0)
        printf("\n[+] mm_check() passed! \n");

    return result;
}