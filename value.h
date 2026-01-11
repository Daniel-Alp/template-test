enum ValTag { VAL_NULL, VAL_BOOL, VAL_NUM, VAL_OBJ };

struct Value {
    ValTag tag;
    union {
        bool boolean;
        double number;
    } as;
};

#define MK_NULL      ((Value){VAL_NULL, {.number = 0}})
#define MK_BOOL(val) ((Value){VAL_BOOL, {.boolean = val}})
#define MK_NUM(val)  ((Value){VAL_NUM, {.number = val}})
#define MK_OBJ(val)  ((Value){VAL_OBJ, {.obj = val}})

#define AS_BOOL(val) ((val).as.boolean)
#define AS_NUM(val)  ((val).as.number)
#define AS_OBJ(val)  ((val).as.obj)

enum InterpResultTag { INTERP_OK, INTERP_ERR };

struct InterpResult {
    InterpResultTag tag;
    union {
        const Value val;
        const char *message;
    };
};
