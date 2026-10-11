
#include <stdio.h>
#include <stdlib.h>
#include "leakspy.h"

typedef struct AllocationNode
{
    int id;
    void *address;
    size_t size;
    struct AllocationNode *next;
} AllocationNode;

static AllocationNode *head = NULL;
static int next_id = 1;
static size_t total_allocations = 0;
static size_t total_deallocations = 0;
static size_t current_bytes = 0;
static size_t peak_bytes = 0;

void *leakspy_malloc(size_t size)
{
    if (size == 0)
    {
        printf("Error: Size must be greater than zero.\n");
        return NULL;
    }

    if (next_id <= 0)
    {
        printf("Error: Allocation ID limit reached.\n");
        return NULL;
    }

    void *ptr = malloc(size);

    if (ptr == NULL)
    {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    AllocationNode *node = malloc(sizeof(AllocationNode));

    if (node == NULL)
    {
        printf("Tracker allocation failed!\n");
        free(ptr);
        return NULL;
    }

    node->id = next_id++;
    node->address = ptr;
    node->size = size;
    node->next = head;
    head = node;

    total_allocations++;
    current_bytes += size;

    if (current_bytes > peak_bytes)
        peak_bytes = current_bytes;

    printf("\nMemory allocated successfully!\n");
    printf("Allocation ID : %d\n", node->id);
    printf("Size          : %zu bytes\n", size);
    printf("Address       : %p\n", ptr);

    return ptr;
}

int leakspy_free_by_id(int id)
{
    AllocationNode *current = head;
    AllocationNode *previous = NULL;

    while (current != NULL)
    {
        if (current->id == id)
        {
            if (previous == NULL)
                head = current->next;
            else
                previous->next = current->next;

            current_bytes -= current->size;
            free(current->address);
            free(current);
            total_deallocations++;

            printf("Memory with ID %d freed successfully.\n", id);
            return 1;
        }

        previous = current;
        current = current->next;
    }

    printf("Invalid ID or memory already freed.\n");
    return 0;
}

void leakspy_show_allocations(void)
{
    AllocationNode *current = head;

    printf("\n========== Current Allocations ==========\n");

    if (current == NULL)
        printf("No active allocations.\n");

    while (current != NULL)
    {
        printf("ID      : %d\n", current->id);
        printf("Address : %p\n", current->address);
        printf("Size    : %zu bytes\n", current->size);
        printf("-----------------------------------------\n");

        current = current->next;
    }

    printf("Current memory usage: %zu bytes\n", current_bytes);
    printf("=========================================\n");
}

void leakspy_report(void)
{
    AllocationNode *current = head;
    size_t leak_count = 0;
    size_t leaked_bytes = 0;

    printf("\n========== LeakSpy Report ==========\n");
    printf("Total Allocations   : %zu\n", total_allocations);
    printf("Total Deallocations : %zu\n", total_deallocations);
    printf("Current Memory      : %zu bytes\n", current_bytes);
    printf("Peak Memory         : %zu bytes\n", peak_bytes);

    while (current != NULL)
    {
        printf("\nPotential Leak %zu\n", leak_count + 1);
        printf("Allocation ID : %d\n", current->id);
        printf("Size          : %zu bytes\n", current->size);

        leaked_bytes += current->size;
        leak_count++;
        current = current->next;
    }

    printf("\nPotential Leaks     : %zu\n", leak_count);
    printf("Unfreed Memory      : %zu bytes\n", leaked_bytes);
    printf("====================================\n");
}

void leakspy_cleanup(void)
{
    AllocationNode *current = head;

    while (current != NULL)
    {
        AllocationNode *next = current->next;
        free(current->address);
        free(current);
        current = next;
        total_deallocations++;
    }

    head = NULL;
    current_bytes = 0;
}
