


typedef struct mon {
    int data;
    void (*add)(struct mon *self, int value);
} Mon;

static void mon_add(Mon *self, int value) {
    self->data += value;
}

Mon Construct_Mon(int data) {
    Mon mon;
    mon.data = data;
    mon.add = mon_add;
    return mon;
}

void TestFunction(void) {
    Mon mon = Construct_Mon(0);
    mon.add(&mon, 5);
    // printf("Mon data: %d\n", mon.data);
}