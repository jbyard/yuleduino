/*
 * yuleduino.ino
 *
 * A Rainbowduino Yule Log.  Brown is a difficult color.  Merry Christmas!
 *
 */
#include <Rainbowduino.h>

#define ANIMATION_SPEED   128
#define MAX_FRAMES        5
#define MAX_X             8
#define MAX_Y             8
#define NUM_SPRITES       2
#define PALETTE_SIZE      8
#define MAX_ALTITUDE      16
#define NUM_SPARKS        16

struct Pixel {
  int r,g,b;
};

struct Spark {
  int x, y, color;
};

struct Sprite {
  size_t  num_frames;
  size_t  current;
  char    frames[MAX_FRAMES][MAX_X][MAX_Y];
};

Spark    sparks[NUM_SPARKS];
Sprite   sprites[NUM_SPRITES];
Pixel    buffer[MAX_X][MAX_Y];

const Pixel palette[PALETTE_SIZE] = {
  {0,0,0},       /* Black */
  {5,0,0},       /* Crimson */
  {139,0,0},     /* Red */
  {254,50,0},    /* Orange */
  {254,202,0},   /* Yellow */
  {2,1,0},       /* Brown */
  {10,10,0},     /* Tan */
  {0,0,0}
};

void setup() {

  /* Initialize Rainbowduino driver */
  Rb.init();

  /* Initialize an array of Sparks */
  for(int s=0; s<NUM_SPARKS ;s++) {
    Spark_init(&sparks[s]);
  }

  /* Flames */
  sprites[0] = { .num_frames = 4, .current = 0, .frames = {
    {
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,3,0,0,0,0},
      {0,0,3,4,3,0,3,0},
      {0,3,3,4,4,3,3,0},
      {3,0,0,0,0,0,4,0},
      {0,0,0,0,0,0,0,0}
    },
    {
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,3,0,0,0},
      {0,0,0,3,4,3,3,0},
      {0,3,3,4,4,3,3,0},
      {3,0,0,0,0,0,4,0},
      {0,0,0,0,0,0,0,0}
    },
    {
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,3,0,0},
      {0,0,0,3,3,3,3,0},
      {0,3,3,4,4,4,3,0},
      {3,0,0,0,0,0,4,0},
      {0,0,0,0,0,0,0,0}
    },
    {
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,3,0,0,0},
      {0,0,0,3,4,3,3,0},
      {0,3,3,4,4,3,3,0},
      {3,0,0,0,0,0,4,0},
      {0,0,0,0,0,0,0,0}
    }
  }};

  /* Log */
  sprites[1] = { .num_frames = 1, .current = 0, .frames = {
    {
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,0,0,0,0,0,0},
      {0,0,5,5,0,6,0,0},
      {0,6,5,6,5,5,5,5}
    }
  }};

}

void loop() {

  /* Clear buffer */
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      buffer[x][y] = palette[0];
    }
  }

  /* Draw sprites on to buffer */
  for(int s=0; s<NUM_SPRITES ;s++) {
    Sprite_draw(&sprites[s]);
  }

  /* Draw Sparks on to buffer */
  for(int s=0; s<NUM_SPARKS ;s++) {
    Spark_draw(&sparks[s]);
  }

  /* Copy buffer to Rainbowduino */
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      Rb.setPixelXY(x,y,buffer[x][y].r,buffer[x][y].g,buffer[x][y].b);
    }
  }

  /* Advance sprites */
  for(int s=0; s<NUM_SPRITES ;s++) {
    Sprite_advance(&sprites[s]);
  }

  /* Advance sparks */
  for(int s=0; s<NUM_SPARKS ;s++) {
    Spark_advance(&sparks[s]);
  }

  delay(ANIMATION_SPEED);
}

void Spark_init(struct Spark *s) {
  s->x       = rand() % (MAX_X);
  s->y       = rand() % (MAX_ALTITUDE);
  s->color   = rand() % (4) + 0;
}

void Spark_draw(struct Spark *s) {
  if (s->y < MAX_Y)
    buffer[s->x][s->y] = palette[s->color];
}

void Spark_advance(struct Spark *s) {
  if (s->y < MAX_ALTITUDE) {
    s->y++;
  } else {
    Spark_init(s);
  }
}

void Sprite_draw(struct Sprite *s) {
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      /* Rotate Sprite 90 degrees */
      if (s->frames[s->current][x][y] != 0 )
        buffer[y][abs(x-7)] = palette[(int) s->frames[s->current][x][y]];
    }
  }
}

void Sprite_advance(struct Sprite *s) {
  if (s->current + 1 < s->num_frames) {
    s->current++;
  } else {
    s->current = 0;
  }
}
