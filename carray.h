typedef struct {
  int size;
  int current;
  char* data;
} DynamicArray;

DynamicArray* makeDynamicArray(int size) {
  char* buffer = malloc(sizeof(char)*size);
  DynamicArray* a = malloc(sizeof(DynamicArray));
  a->size = size;
  a->current = 0;
  a->data = buffer;
  return a;
}

void append(DynamicArray* da, char* text, int length) {
  memcpy((void*)&da->data[da->current], (void*)text, sizeof(char)*length);
  da->current += length;
  da->data[da->current] = '\0';
}

void print(DynamicArray* da) {
  printf("%s", da->data);
}
