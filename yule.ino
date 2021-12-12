/*
 * yule.ino
 *
 * A Rainbowduino Yule Log.  Brown is a difficult color.  Merry Christmas!
 *
 */
#include <Rainbowduino.h>

#define ANIMATION_SPEED   181
#define MAX_FRAMES        5
#define MAX_X             8
#define MAX_Y             8
#define MAX_SPRITES       2
#define MAX_ALTITUDE      16
#define MAX_PARTICLES     8

struct Pixel {
  int r, g, b;
};

typedef struct Pixel Palette[4];

struct Particle {
  int x, y, palette, color;
};

struct Row {
  unsigned char mask, low_color_bit, high_color_bit;
};

typedef struct Row Frame[MAX_Y];

struct Sprite {
  size_t   num_frames, current;
  int      palette;
  Frame    frames[MAX_FRAMES];
};

size_t num_sprites, num_particles;

Particle particles[MAX_PARTICLES];
Sprite   sprites[MAX_SPRITES];
Pixel    buffer[MAX_X][MAX_Y];

const Palette palettes[3] = {
  {
    {200,50,0},
    {200,110,0},
    {0,0,0},
    {254,202,0}
  },{
    {0,0,0},
    {10,0,0},
    {20,0,0},
    {30,10,0}
  },{
    {8,1,0},
    {8,2,0},
    {8,3,0},
    {0,0,0}
  }
};

void setup() {

  /* Initialize Rainbowduino driver */
  Rb.init();

  /* A Roaring Fire */
  num_sprites = 2;
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
        {0x7E, 0x7E, 0x3C}
      },
      {
        {0x00, 0x00, 0x00},
        {0x10, 0x00, 0x00},
        {0x1C, 0x00, 0x00},
        {0x3A, 0x18, 0x10},
        {0x7E, 0x38, 0x10},
        {0xFE, 0x3C, 0x18},
        {0xFF, 0x7E, 0x3C},
        {0x7E, 0x7E, 0x3C}
      },
      {
        {0x00, 0x00, 0x00},
        {0x20, 0x00, 0x00},
        {0x30, 0x20, 0x00},
        {0x78, 0x30, 0x00},
        {0x7C, 0x31, 0x20},
        {0xFE, 0x7C, 0x30},
        {0xFF, 0x7E, 0x3C},
        {0x7E, 0x7E, 0x3C}
      }
    }};

  /* A Crackling Log */
  sprites[1] = {
    .num_frames = 4,
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
        {0x7E, 0x00, 0x00},
        {0x7E, 0x00, 0x00}
      },
      {
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x7E, 0xFF, 0x00},
        {0x7E, 0x00, 0x00}
      },
      {
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x7E, 0x00, 0xFF},
        {0x7E, 0x00, 0x00}
      },
      {
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00},
        {0x7E, 0xFF, 0x00},
        {0x7E, 0x00, 0x00}
      }
    }};

  /* Some Shooting Sparks */
  num_particles = 8;

  for(int p = 0; p < num_particles; p++) {
    Particle_init(&particles[p], 1);
  }
}

void loop() {

  /* Clear buffer */
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      buffer[x][y] = {0,0,0};
    }
  }

  /* Draw sprites on to buffer */
  for(int s = 0; s < num_sprites; s++) {
    Sprite_draw(&sprites[s]);
  }

  /* Draw particles on to buffer */
  for(int p = 0; p < num_particles; p++) {
    Particle_draw(&particles[p]);
  }

  /* Copy buffer to Rainbowduino */
  for (int x = 0; x < MAX_X; x++) {
    for (int y = 0; y < MAX_Y; y++) {
      Rb.setPixelXY(x,y,buffer[x][y].r,buffer[x][y].g,buffer[x][y].b);
    }
  }

  /* Advance sprites */
  for(int s = 0; s < num_sprites; s++) {
    Sprite_advance(&sprites[s]);
  }

  /* Advance particles */
  for(int p = 0; p < num_particles; p++) {
    Particle_advance(&particles[p]);
  }

  delay(ANIMATION_SPEED);
}

void Particle_init(struct Particle *p, int palette) {
  p->x       = rand() % (MAX_X);
  p->y       = rand() % (MAX_ALTITUDE) + 2;
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

  int position = 0;

  for (int y = 0, z = (MAX_Y - 1); y < MAX_Y; y++, z--) {
    for (int x = 0; x < MAX_X; x++) {

      position = (1 << (MAX_X - 1)) >> x;

      if (s->frames[s->current][y].mask & position) {

        /* If the Sprite masks this position, then color it in the buffer. */
        buffer[x][z] = palettes[s->palette][
          !!(s->frames[s->current][y].high_color_bit & position) << 1
          | !!(s->frames[s->current][y].low_color_bit & position)
        ];
      }
    }
  }
}

void Sprite_advance(struct Sprite *s) {
  s->current = (s->current + 1) % s->num_frames;
}
