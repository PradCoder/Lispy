/* This part is essentially building a simple garbage collector*/
typedef enum {
  OBJ_INT,
  OBJ_PAIR
} ObjectType;

typedef struct sObject {
  unsigned char marked;
  ObjectType type;
  struct sObject* next;
  
  union {
    /* OBJ_INT */
    int value;

    /* OBJ_PAIR */
    struct {
      struct sObject* head;
      struct sObject* tail;
    };
  };
} Object;

/*This part is for writing what I want the VM*/
#define STACK_MAX 256

typedef struct {
  Object* stack[STACK_MAX];
  int stackSize;
} VM;

VM* newVM() {
  VM* vm = malloc(sizeof(VM));
  vm->stackSize = 0;
  return vm;
}

void push(VM* vm, Object* value) {
  assert(vm->stackSize < STACK_MAX, "Stack ovreflow!");
  vm->stack[vm->stackSize++] = value;
}

Object* pop(VM* vm) {
  assert(vm->stackSize > 0, "Stack underflow!");
  return vm->stack[--vm->stackSize];
}

Object* newObject(VM* vm, ObjectType type) {
  Object* object = malloc(sizeof(Object));
  object->type = type;
  object->marked = 0;

  /*Insert it into the list of allocated objects. */
  object->next = vm->firstObject;
  vm->firstObject = object;
  
  return object;
}

void pushInt(VM* vm, int intValue) {
  Object* object = newObject(vm, OBJ_INT);
  object->value = intValue;
  push(vm, object);
}

Object* pushPair(VM* vm) {
  Object* object = newObject(vm, OBJ_PAIR);
  object->tail = pop(vm);
  object->head = pop(vm);

  push(vm, object);
  return object;
}

/*Mark and Sweep implementation*/
void markAll(VM* vm){
  for (int i = 0; i < vm->stackSize; i++) {
    mark(vm->stack[i]);
  }
}

void mark(Object* object) {
  if (object->marked) return;
  
  object->marked = 1;

  if (object->type == OBJ_PAIR){
    mark(object->head);
    mark(object->tail);
  }
}

void sweep(VM* vm){
  Object** object = &vm->firstObject;
  while (*object){
    if(!(*object)->marked){
      Object* unreached = *object;

      *object = unreached->next;
      free(unreached);
    } else {
      (*object)->marked = 0;
      object = &(*object)->next;
    }
  }
}

void gc(VM* vm) {
  markAll(vm);
  sweep(vm);
}
