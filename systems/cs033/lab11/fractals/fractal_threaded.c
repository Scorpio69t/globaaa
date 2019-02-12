#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "./fractal.h"
#include "./image.h"

/*    TODO: define a struct which wraps the arguments to generate_fractal_region()    */
typedef struct args {
  color_t* image_data;
  float width;
  float height;
  float start_y;
  int rows;
} args;

/*
    generates part of the fractal region, computing data for a number of rows
    beginning from start_y

    arguments:    data: a struct wrapping the data needed to generated the fractal
                    - a pointer to the color data region of memory
                    - the width of the image
                    - the height of the image
                    - the starting row for filling out data
                    - the number of rows to fill out
    returns:    NULL
*/
void *gen_fractal_region(void *data) {
    /* TODO: unpack the data struct and call generate_fractal_region()    */
    args* arg = (args*) data;
    generate_fractal_region(arg->image_data, arg->width, arg->height, arg->start_y, arg->rows);
    pthread_exit((void *) 0);

    return NULL;
}



int generate_fractal(char *file, float width, float height, int workers) {
    /* TODO: initialize several threads which will compute a unique
             region of the fractal data, and have them each execute
             gen_fractal_region().    */

    // create fractal_data
    color_t* fractal_data = malloc(sizeof(color_t)*width*height);

    // create an array of threads and args
    pthread_t thr[workers];
    args array[workers];

    // initialize threads
    for(int i = 0; i < workers; i++) {
      int error;

      // create a wrapper for arguments
      args* ptr = &array[i];

      ptr->image_data = fractal_data;
      ptr->width = width;
      ptr->height = height;
      ptr->start_y = i * (height / workers);
      ptr->rows = height / workers;

      if ((error = pthread_create(&thr[i], 0, gen_fractal_region, (void *) ptr))) {
	fprintf(stderr, "pthread_create: %s", strerror(error));
	exit(1);
      }
    }

    // wait for all threads to complete
    for(int i = 0; i < workers; i++) {
      pthread_join(thr[i], 0);
    }

    if (save_image_data(file, fractal_data, width, height)) {
        fprintf(stderr, "error saving to image file.\n");
        free(fractal_data);
        return 1;
    }

    free(fractal_data);

    printf("Complete.\n");
    return 0;
}
