#ifndef TINBOT_H
#define TINBOT_H

typedef struct TinBot {

} TinBot;

void setup(TinBot* tinbot);
void loop(TinBot* tinbot);

void update_proximity(TinBot* tinbot, int proximity[8]);
void update_ir(TinBot* tinbot, int ir[6]);

#endif
