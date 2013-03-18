
#include <stdlib.h>

#include "tcl.h"
#include "tpool.h"


TPoolWork *work(void *data) {
    printf("Here %p\n", data);
    sleep(data);
    printf("Thread %p\n", data);
}



int main() {

    TPool *tp = TPoolInit(5);

    TPoolThreadStart(tp, (TPoolWork) work, (void *) 1);
    TPoolThreadStart(tp, (TPoolWork) work, (void *) 2);
    TPoolThreadStart(tp, (TPoolWork) work, (void *) 3);
    TPoolThreadStart(tp, (TPoolWork) work, (void *) 4);
    TPoolThreadStart(tp, (TPoolWork) work, (void *) 5);
    TPoolThreadStart(tp, (TPoolWork) work, (void *) 6);
    TPoolThreadStart(tp, (TPoolWork) work, (void *) 7);

    sleep(10);


    return 0;
}
