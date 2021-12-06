/*
 * yuleduino.ino
 *
 * A Rainbowduino Yule Log.  Brown is a difficult color.  Merry Christmas!
 *
 */
#include <Rainbowduino.h>

#define ANIMATION_SPEED   181
#define MAX_FRAMES        5
#define MAX_X             8
#define MAX_Y             8
#define NUM_SPRITES       2
#define PALETTE_SIZE      8
#define MAX_ALTITUDE      16
#define NUM_PARTICLES     8

struct Pixel {
  int r, g, b;
};

typedef struct Pixel Palette[4];

struct Particle {
  int x, y, palette, color;
};

struct Row {
  unsigned char mask, color, shift;
};

typedef struct Row Frame[MAX_Y];

struct Sprite {
  size_t   num_frames, current;
  int      palette;
  Frame    frames[MAX_FRAMES];
};

Particle particles[NUM_PARTICLES];
Sprite   sprites[NUM_SPRITES];
Pixel    buffer[MAX_X][MAX_Y];

const Palette palettes[3] = {
  {
    {200,50,0},
    {200,110,0},
    {0,0,0},
    {254,202,0}
  },
  {
    {0,0,0},
    {10,0,0},
    {20,0,0},
    {30,10,0}
  },
  {
    {2,1,0},
    {0,0,0},
    {0,0,0},
    {0,0,0}
  }
};

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

  /* Sparks */
  for(int p=0; p<NUM_PARTICLES ;p++) {
    Particle_init(&particles[p], 1);
  }

  /* Flames */
  sprites[0] = {
    .num_frames = 3,
    .current = 0,
    .palette = 0,
    .frames = {
      {
        {0x00, 0x00, 0x00},
        {0x10, 0x00, 0x00},
        {0x3A, 0x00, 0x00},
        {0x1E, 0x01, 0x00},
        {0x3F, 0x1E, 0x0C},
        {0x7F, 0x3E, 0x1E},
        {0xFF, 0x7E, 0x3C},
        {0xFF, 0x7E, 0x3C}
      },
      {
        {0x00, 0x00, 0x00},
        {0x10, 0x00, 0x00},
        {0x1C, 0x00, 0x00},
        {0x3A, 0x18, 0x10},
        {0x7E, 0x38, 0x10},
        {0xFE, 0x3C, 0x18},
        {0xFF, 0x7E, 0x3C},
        {0xFF, 0x7E, 0x3C}
      },
      {
        {0x00, 0x00, 0x00},
        {0x20, 0x00, 0x00},
        {0x30, 0x20, 0x00},
        {0x78, 0x30, 0x00},
        {0x7C, 0x31, 0x20},
        {0xFE, 0x7C, 0x30},
        {0xFF, 0x7E, 0x3C},
        {0xFF, 0x7E, 0x3C}
      }
    }};

  /* Log */
  sprites[1] = {
    .num_frames = 1,
    .current = 0,
    .palette = 2,
    .frames = {
      {
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x3C, 0x00, 0x00},
        {0x3C, 0x00, 0x00}
      }
    }};

}

void loop() {

  /* Clear buffer */
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      buffer[x][y] = {0,0,0};
    }
  }

  /* Draw sprites on to buffer */
  for(int s=0; s<NUM_SPRITES ;s++) {
    Sprite_draw(&sprites[s]);
  }

  /* Draw particles on to buffer */
  for(int p=0; p<NUM_PARTICLES ;p++) {
    Particle_draw(&particles[p]);
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

  /* Advance particles */
  for(int p=0; p<NUM_PARTICLES ;p++) {
    Particle_advance(&particles[p]);
  }

  delay(ANIMATION_SPEED);
}

void Particle_init(struct Particle *p, int palette) {
  p->x       = rand() % (MAX_X);
  p->y       = rand() % (MAX_ALTITUDE);
  p->color   = rand() % (4) + 0;
  p->palette = palette;
}

void Particle_draw(struct Particle *p) {
  if (p->y < MAX_Y)
    buffer[p->x][p->y] = palettes[p->palette][p->color];
}

void Particle_advance(struct Particle *p) {
  if (p->y < MAX_ALTITUDE) {
    p->y++;
  } else {
    Particle_init(p, p->palette);
  }
}

void Sprite_draw(struct Sprite *s) {

  int offset, color, shift = 0;

  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {

      offset = abs(x - (MAX_X - 1));

      if (s->frames[s->current][y].mask & (1 << offset)) {

        color = ((s->frames[s->current][y].color >> offset) & 1);

        if (x > 6) {
          shift = ((s->frames[s->current][y].shift << (offset + 1)) & 2);
        } else {
          shift = ((s->frames[s->current][y].shift >> (offset - 1)) & 2);
        }

        buffer[x][abs(y-7)] = palettes[s->palette][color | shift];
      }

    }
  }

}

void Sprite_advance(struct Sprite *s) {
  s->current = (s->current + 1) % s->num_frames;
}
