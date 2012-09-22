#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

typedef enum keymodifier
{
	KM_LSHIFT = 1,
	KM_CTRL = 2,
	KM_ALT = 4
} keymodifier_t;

typedef enum keytype
{
	KT_NONE,
	KT_NORMAL,
	KT_LSHIFT,
	KT_LCTRL,
	KT_RSHIFT,
	KT_RCTRL,
	KT_CAPSLOCK,
	KT_CURSOR,
	KT_MINUS,
	KT_ALT,
	KT_FUNC,
	KT_NUMLOCK,
	KT_SCROLL
} keytype_t;

void kb_install(void);

#endif /* _KEYBOARD_H_ */
