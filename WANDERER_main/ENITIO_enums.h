#ifndef ENITIO_enums_h
#define ENITIO_enums_h

typedef enum {
    ALATAR = 0,
    DRACHEN = 1,
    EVA = 2,
    INVICTA = 3,
} OG_id ;

typedef enum {
    do_nothing = 0,
    attack = 1,
    collect = 2,
    heal = 3,
    heal_request = 4
} action_id ;

typedef enum {
    idle = 0,
    button = 1,
    right = 2,
    up = 3,
    left = 4,
    down = 5,
} joystick_pos ; 

typedef enum {
    bonus6HP = 1,
    bonus1MaxEn = 2,
    bonus1MANA = 3,
    fiveminx2EnRegen = 4,
    bomb = 5,
} power_up; 

#endif
