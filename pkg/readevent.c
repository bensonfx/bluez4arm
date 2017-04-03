/*
 * Copyright 2002 Red Hat Inc., Durham, North Carolina.
 *
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation on the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT.  IN NO EVENT SHALL RED HAT AND/OR THEIR SUPPLIERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This is a simple test program that reads from /dev/input/event*,
 * decoding events into a human readable form.
 */

/*
 * Authors:
 *   Rickard E. (Rik) Faith <faith@redhat.com>
 *
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <linux/input.h>

#define MAX_LINE 4096

struct list_head {
      struct list_head *next, *prev;
};

typedef struct event_attr {
    char id;
    char name[100];
    char phys[64];
    char uniq[64];
    char ev_nr;
    struct list_head list;
} event_attr;

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

#define offsetof(s, m)   (size_t)&(((s *)0)->m)

#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

#define list_for_each_entry(pos, head, member)				\
	for (pos = list_first_entry(head, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_next_entry(pos, member))



LIST_HEAD(event_lists);

static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static int parsing_proc_input(void){
	const char input[] = "/proc/bus/input/devices";
	char line[MAX_LINE];
	FILE *fp;
    char *ptr,*pos;
	int result, find_flag,ev_nr;
    event_attr *evr;

	if(NULL == (fp = fopen(input, "rt")) ){
		return -1;
	}
    while (!feof(fp)) {
        fgets(&line[0],MAX_LINE,fp);
        if (line[0] == 'N') {
            evr = malloc(sizeof(event_attr));
            ptr = strchr(line,'"') + 1;
            pos = strchr(ptr,'"');
            *pos = '\0';
            strcpy(evr->name,ptr);
        }
        if (line[0] == 'H') {
            ptr = strstr(line,"event") + strlen("event");
            evr->ev_nr = (u_int8_t)atoi(ptr);
            list_add_tail(&evr->list,&event_lists);
        }
        if (line[0] == 'P') {
            ptr = strchr(line,'=') + 1;
            pos = strchr(ptr,'\n');
            *pos = '\0';
            strcpy(evr->phys,ptr);

        }
        if (line[0] == 'U') {
            ptr = strchr(line,'=') + 1;
            pos = strchr(ptr,'\n');
            *pos = '\0';
            strcpy(evr->uniq,ptr);
        }

    }
	fclose(fp);
    return 0;
}


int main(int argc, char **argv)
{
    char          name[64];           /* RATS: Use ok, but could be better */
    char          buf[256] = { 0, };  /* RATS: Use ok */
    unsigned char mask[EV_MAX/8 + 1]; /* RATS: Use ok */
    int           version;
    int           fd = 0;
    int           rc;
    int           i, j;
    char          *tmp;
    struct input_event event;
/** #define test_bit(bit) (mask[(bit)/8] & (1 << ((bit)%8))) */
/**     for (i = 0; i < 32; i++) { */
/**         sprintf(name, "/dev/input/event%d", i); */
/**         if ((fd = open(name, O_RDONLY, 0)) >= 0) { */
/**             ioctl(fd, EVIOCGVERSION, &version); */
/**             ioctl(fd, EVIOCGNAME(sizeof(buf)), buf); */
/**             ioctl(fd, EVIOCGBIT(0, sizeof(mask)), mask); */
/**             printf("%s\n", name); */
/**             printf("    evdev version: %d.%d.%d\n", */
/**                    version >> 16, (version >> 8) & 0xff, version & 0xff); */
/**             printf("    name: %s\n", buf); */
/**             printf("    features:"); */
/**             for (j = 0; j < EV_MAX; j++) { */
/**                 if (test_bit(j)) { */
/**                     const char *type = "unknown"; */
/**                     switch(j) { */
/**                     case EV_KEY: type = "keys/buttons"; break; */
/**                     case EV_REL: type = "relative";     break; */
/**                     case EV_ABS: type = "absolute";     break; */
/**                     case EV_MSC: type = "reserved";     break; */
/**                     case EV_LED: type = "leds";         break; */
/**                     case EV_SND: type = "sound";        break; */
/**                     case EV_REP: type = "repeat";       break; */
/**                     case EV_FF:  type = "feedback";     break; */
/**                     } */
/**                     printf(" %s", type); */
/**                 } */
/**             } */
/**             printf("\n"); */
/**             close(fd); */
/**         } */
/**     } */
    char nr;
    if (argc > 1) {
        nr = atoi(argv[1]);
    } else {
        parsing_proc_input();
        event_attr *ev;
        printf("list of input device,choise event id to read input event:\n");
        list_for_each_entry(ev, &event_lists, list) {
            printf("%d:",ev->ev_nr);
            printf("\tname:%s\n",ev->name);
            printf("\tevent:%d\n",ev->ev_nr);
            printf("\tuniq:%s\n",ev->uniq);
            printf("\tphys:%s\n",ev->phys);
        }
        uint8_t nr_str[10];
        fgets(nr_str, sizeof(nr_str), stdin);
        nr = atoi(nr_str);
    }
    sprintf(name, "/dev/input/event%d", nr);
    if ((fd = open(name, O_RDWR, 0)) >= 0) {
        printf("%s: open, fd = %d\n", name, fd);
        for(;;) {
            while ((rc = read(fd, &event, sizeof(event))) > 0) {
                switch (event.type) {
                    case EV_KEY:
                        printf("%-24.24s-->type 0x%04x; code 0x%04x; value 0x%08x--->",
                            ctime(&event.time.tv_sec),event.type, event.code, event.value);
                        if (event.code > BTN_MISC) {
                            printf("Button %d %s",
                                event.code & 0xff,
                                event.value ? "press" : "release");
                        } else {
                            printf("Key %d (0x%x) %s",
                                event.code & 0xff,
                                event.code & 0xff,
                                event.value ? "press" : "release");
                        }
                        printf("\n");
                        break;
                    case EV_REL:
                        switch (event.code) {
                        case REL_X:      tmp = "X";       break;
                        case REL_Y:      tmp = "Y";       break;
                        case REL_HWHEEL: tmp = "HWHEEL";  break;
                        case REL_DIAL:   tmp = "DIAL";    break;
                        case REL_WHEEL:  tmp = "WHEEL";   break;
                        case REL_MISC:   tmp = "MISC";    break;
                        default:
                            sprintf(tmp,"%x",event.code);
                            break;
                        }
                        printf("Relative %s %d\n", tmp, event.value);
                        break;
                    case EV_ABS:
                            switch (event.code) {
                                case ABS_X:                 tmp = "X";      break;
                                case ABS_Y:                 tmp = "Y";      break;
                                case ABS_MT_POSITION_X:     tmp = "X";      break;
                                case ABS_MT_POSITION_Y:     tmp = "Y";      break;
                                case ABS_MT_TOUCH_MAJOR:    tmp = "major";  break;
                                case ABS_MT_TOUCH_MINOR:    tmp = "major";  break;
                                case ABS_MT_WIDTH_MAJOR:    tmp = "width major";  break;
                                case ABS_MT_WIDTH_MINOR:    tmp = "width major";  break;
                                case ABS_MT_SLOT:           tmp = "slot";      break;
                                case ABS_MT_PRESSURE:       tmp = "pressure";  break;
                                case ABS_MT_TRACKING_ID:    tmp = "track_id";  break;
                                default:
                                    /** tmp = "***"; */
                                    sprintf(tmp,"%x",event.code);
                                    break;
                            }
                        printf("Absolute %s:%d\n", tmp, event.value);
                        break;
                    case EV_MSC: printf("Misc\t"); break;
                    case EV_LED: printf("Led");  break;
                    case EV_SND: printf("Snd");  break;
                    case EV_REP: printf("Rep");  break;
                    case EV_FF:  printf("FF");   break;
                }
            }
            printf("rc = %d, (%s)\n", rc, strerror(errno));
            close(fd);
        }
    } else {
        printf("can not open %s\n",name);
    }
    return 0;
}
