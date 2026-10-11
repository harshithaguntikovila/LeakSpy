
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "leakspy.h"

static int read_number(const char *prompt, long *result)
{
    char line[128];
    char *end;

    printf("%s", prompt);

    if (fgets(line, sizeof(line), stdin) == NULL)
        return 0;

    errno = 0;
    long value = strtol(line, &end, 10);

    if (end == line || errno == ERANGE ||
        value < 0 || value > INT_MAX)
    {
        printf("Invalid input. Enter a valid number.\n");
        return -1;
    }

    while (*end == ' ' || *end == '\t' ||
           *end == '\n' || *end == '\r')
        end++;

    if (*end != '\0')
    {
        printf("Invalid input. Enter numbers only.\n");
        return -1;
    }

    *result = value;
    return 1;
}

static void show_menu(void)
{
    printf("\n========== LeakSpy ==========\n");
    printf("1. Allocate Memory\n");
    printf("2. Free Memory\n");
    printf("3. Show Current Allocations\n");
    printf("4. Detect Memory Leaks\n");
    printf("5. Exit\n");
    printf("=============================\n");
}

int main(void)
{
    long choice;
    long value;

    printf("\n********************************\n");
    printf("       LEAKSPY MEMORY PROFILER\n");
    printf("********************************\n");

    while (1)
    {
        show_menu();

        int status = read_number("Enter your choice: ", &choice);

        if (status == 0)
            break;

        if (status == -1)
            continue;

        switch (choice)
        {
            case 1:
                status = read_number(
                    "Enter memory size in bytes: ", &value);

                if (status == 0)
                    goto exit_program;

                if (status == -1)
                    break;

                if (value == 0)
                {
                    printf("Size must be greater than zero.\n");
                    break;
                }

                leakspy_malloc((size_t)value);
                break;

            case 2:
                status = read_number(
                    "Enter Allocation ID to free: ", &value);

                if (status == 0)
                    goto exit_program;

                if (status == -1)
                    break;

                if (value == 0)
                {
                    printf("Allocation ID must be positive.\n");
                    break;
                }

                leakspy_free_by_id((int)value);
                break;

            case 3:
                leakspy_show_allocations();
                break;

            case 4:
                leakspy_report();
                break;

            case 5:
                goto exit_program;

            default:
                printf("Invalid choice. Select 1 to 5.\n");
        }
    }

exit_program:
    printf("\nCleaning up remaining allocations...\n");
    leakspy_cleanup();
    printf("LeakSpy terminated.\n");

    return 0;
}
