#include <Carbon/Carbon.h>
#include <stdio.h>
#include <getopt.h>

#define KRP_MAJOR 1
#define KRP_MINOR 0
#define KRP_PATCH 0

//
// these extern declarations are necessary to compile on macOS Big Sur
//

extern void LMSetKeyThresh(int16_t rate);
extern void LMSetKeyRepThresh(int16_t rate);

void change_key_repeat_delay(char *arg)
{
    int16_t rate;

    if (sscanf(arg, "%hd", &rate) != 1)
    {
        fprintf(stderr, "invalid keyrepeat delay '%s'\n", arg);
        return;
    }

    int16_t old_rate = LMGetKeyThresh();
    LMSetKeyThresh(rate);

    char command[256];
    snprintf(command, sizeof(command), "defaults write NSGlobalDomain InitialKeyRepeat -int %hd", rate);
    system(command);

    printf("delay until repeat: %hd -> %hd\n", old_rate, rate);
}

void change_key_repeat_rate(char *arg)
{
    int16_t rate;

    if (sscanf(arg, "%hd", &rate) != 1)
    {
        fprintf(stderr, "invalid keyrepeat rate '%s'\n", arg);
        return;
    }

    int16_t old_rate = LMGetKeyRepThresh();
    LMSetKeyRepThresh(rate);

    char command[256];
    snprintf(command, sizeof(command), "defaults write NSGlobalDomain KeyRepeat -int %hd", rate);
    system(command);

    printf("keyrepeat rate: %hd -> %hd\n", old_rate, rate);
}

void print_current_settings()
{
    int16_t old_thresh_rate = LMGetKeyThresh();
    int16_t old_rep_rate = LMGetKeyRepThresh();

    printf("delay until repeat [ -d ]: %3hd\n", old_thresh_rate);
    printf("repeat rate        [ -r ]: %3hd\n", old_rep_rate);
    printf("\n");
    printf("<level>: 1 = 15ms, 2 = 30ms etc..\n");
}

int parse_arguments(int count, char **args)
{
    int option;
    const char *short_options = "r:d:v:p";
    struct option long_options[] = {
        {"repeat-rate", required_argument, NULL, 'r'},
        {"delay-until-repeat", required_argument, NULL, 'd'},
        {"version", no_argument, NULL, 'v'},
        {"print", no_argument, NULL, 'p'},
        {NULL, 0, NULL, 0}};

    while ((option = getopt_long(count, args, short_options, long_options, NULL)) != -1)
    {
        switch (option)
        {
        case 'r':
        {
            change_key_repeat_rate(optarg);
        }
        break;
        case 'd':
        {
            change_key_repeat_delay(optarg);
        }
        break;
        case 'v':
        {
            printf("krp version %d.%d.%d\n", KRP_MAJOR, KRP_MINOR, KRP_PATCH);
        }
        break;
        case 'p':
        {
            print_current_settings();
        }
        break;
        }
    }

    return 0;
}

int main(int count, char **args)
{
    if (count < 2)
    {
        fprintf(stderr, "Usage: krp -r | --repeat-rate <level>\n"
                        "       krp -d | --delay-until-repeat <level>\n"
                        "       krp -p | --print \n\n"
                        "<level>: 1 = 15ms, 2 = 30ms etc..\n");
        exit(1);
    }

    parse_arguments(count, args);
    return 0;
}
