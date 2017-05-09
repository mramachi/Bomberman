#ifndef H_UTIL
#define H_UTIL

typedef enum {EVENT_TIMER, EVENT_KEY} EVENT_TYPE;

typedef struct {
	EVENT_TYPE type;
} TimerEvent;

typedef struct {
	EVENT_TYPE type;
	int key;
} KeyEvent;

typedef union {
	EVENT_TYPE type;
	TimerEvent timerEvent;
	KeyEvent keyEvent;
} Event;

#endif