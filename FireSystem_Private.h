

#ifndef FIRESYSTEM_PRIVATE_H_
#define FIRESYSTEM_PRIVATE_H_

typedef enum {
	FINE=0,
	HEAT,
	FIRE
}FireSystem_State_t;

typedef enum {
	REVERSABLE=0,
	IRREVERSABLE
}FireSystem_Damage_t;

typedef enum {
	NORMAL,
	PASSWORD,
	WRONG_PASSWORD
}GUI_State_t;

static void FireSystem_GUI(GUI_State_t state);
static u8 string_compare(c8 *str1,c8*str2);
static u8 string_len(c8*str);
#endif /* FIRESYSTEM_PRIVATE_H_ */