#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#define WIDTH 620
#define HEIGHT 720
#define SPEED 9
#define FONT_SIZE 32
#define BALL_SPEED 8
#define SIZE 16
#define COL 7
#define ROW 5
#define PI 3.14
#define SPACING 16

SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font* font;
bool running;
int frameCount, timerFPS, lastFrame, fps; /* limit amount of redrawing done in a sec */
SDL_Color color;
SDL_Rect paddle, ball, lives, brick; /* "chand-zeili" */
float velY, velX;
int liveCount; /* joon ha */
bool bricks[ROW*COL]; /* ajoor ha */


void resetBricks() {
for(int i=0; i<COL*ROW; i++) bricks[i]=1; /* set all bricks to true */
liveCount=3; /* 3 ta joon mide */
paddle.x=(WIDTH/2)-(paddle.w/2); /* makan pedal ro set mikone */
ball.y=paddle.y-(paddle.h*4); /* makan y toop ro set mikone */
ball.x=WIDTH/2-(SIZE/2); /* makan x toop ro set mikone */
velY=BALL_SPEED/2; 
velX=0;
}

/* rasm ajoor ha */
void setBricks(int i) {
 brick.x=(((i%COL)+1)*SPACING)+((i%COL)*brick.w)-(SPACING/2);
 brick.y=brick.h*3+(((i%ROW)+1)*SPACING)+((i%ROW)*brick.h)-(SPACING/2);
}

/* yek matn ra dar safeh neshan mi dahad */
void write(std::string text, int x, int y) {
SDL_Surface *surface;
SDL_Texture *texture;
const char* t=text.c_str();
surface = TTF_RenderText_Solid(font, t, color);
texture = SDL_CreateTextureFromSurface(renderer, surface);
lives.w=surface->w;
lives.h=surface->h;
lives.x=x-lives.w;
lives.y=y-lives.h;
SDL_FreeSurface(surface);
SDL_RenderCopy(renderer, texture, NULL, &lives);
SDL_DestroyTexture(texture);
}
/* 1 lahze haraket mikond (redraw mikonad)*/
void update() {
 if(liveCount<=0) resetBricks();  /* check mikonad ke joon ha az 0 bishtar bashad */
 if(SDL_HasIntersection(&ball, &paddle)) { /* check mikonad ke toop be pedal korde bashad */
  double rel=(paddle.x+(paddle.w/2))-(ball.x+(SIZE/2));
  double norm=rel/(paddle.w/2);
  double bounce = norm* (5*PI/12);
  velY=-BALL_SPEED*cos(bounce);
  velX=BALL_SPEED*-sin(bounce);
 }
 if(ball.y<=0) velY=-velY; /* in khat va khat hai badi barrsi mikonnad ke toop be zamin nakhorde bashad */
 if(ball.y+SIZE>=HEIGHT) {velY=-velY;liveCount--;} /* joon ro -1 kon */
 if(ball.x<=0 || ball.x+SIZE>=WIDTH) velX=-velX;
 ball.x+=velX; /* toop ro bebar vasat */
 ball.y+=velY;
 if(paddle.x<0) paddle.x=0; /* pedal ra vasat bebar */
 if(paddle.x+paddle.w>WIDTH) paddle.x=WIDTH-paddle.w; /* XXX */

 bool reset=1;
 for(int i=0; i<COL*ROW; i++) { /* 35 ajoor ro rasm kon */
  setBricks(i); 
  if(SDL_HasIntersection(&ball, &brick) && bricks[i]) { /* agr toop be ajoor ha khorde bashad, an ajoor ra false mikonad */
   bricks[i]=0;
   if(ball.x >= brick.x) {velX=-velX; ball.x-=20;} /* jahat toop ra avaz mikonad */
   if(ball.x <= brick.x) {velX=-velX; ball.x+=20;}
   if(ball.y <= brick.y) {velY=-velY; ball.y-=20;}
   if(ball.y >= brick.y) {velY=-velY; ball.y+=20;}
  }
  if(bricks[i]) reset=0; /* agr ajoori monde bashad bazi ra edame bede */
 }
 if(reset) resetBricks(); /* You win */
}
/* chap o rast ba keyboard pedal ra je ba ja mikonad */
void input() {
SDL_Event e;
const Uint8 *keystates = SDL_GetKeyboardState(NULL); /* XXX */
while(SDL_PollEvent(&e)) if(e.type==SDL_QUIT) running=false; /* pause */
if(keystates[SDL_SCANCODE_ESCAPE]) running=false; /* pause */
if(keystates[SDL_SCANCODE_LEFT]) paddle.x-=SPEED; /* chap */
if(keystates[SDL_SCANCODE_RIGHT]) paddle.x+=SPEED; /* rast */

}
/* XXX: fps ro set mikonad  */
void render() {
SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
SDL_RenderClear(renderer);

frameCount++; 
timerFPS=SDL_GetTicks()-lastFrame;
if(timerFPS<(1000/60)) {
 SDL_Delay((1000/60)-timerFPS);
}

SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
SDL_RenderFillRect(renderer, &paddle); /* rang kardan pedal */
SDL_RenderFillRect(renderer, &ball); /* rang kardan toop */
write(std::to_string(liveCount), WIDTH/2+FONT_SIZE/2, FONT_SIZE*1.5); /* neshan dadan joon ha ro safeh */

for(int i=0; i<COL*ROW; i++) { /* rang kardan ajoor ha */
 SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
 if(i%2==0)SDL_SetRenderDrawColor(renderer, 0,255, 0, 255);
 if(bricks[i]) {
  setBricks(i);
  SDL_RenderFillRect(renderer, &brick);
 }
}

SDL_RenderPresent(renderer);
}


int main() {
if(SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "Failed at SDL_Init()" << std::endl; 
if(SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cout << "Failed at SDL_CreateWindowAndRenderer()" << std::endl;
SDL_SetWindowTitle(window, "Brick Breaker"); /* title panjre ro set mikonad */
TTF_Init(); 
font = TTF_OpenFont("font.ttf", FONT_SIZE); /* font ro load mikonim */
running = 1;  /* bazi ro shoro mikonad */
static int lastTime=0;
color.r=color.g=color.b=255;
paddle.h=12; paddle.w=WIDTH/4; /* tool o arz pedal */
paddle.y=HEIGHT-paddle.h-32;
ball.w=ball.h=SIZE; /* tool o arz toop */
brick.w=(WIDTH-(SPACING*COL))/COL; /* tool o arz ajoor */
brick.h=22;

resetBricks(); /* ajoor ha ro set mikonad */

while(running) { /* agr game pause nashode bood */
 lastFrame=SDL_GetTicks();
 if(lastFrame>=lastTime+1000) {
  lastTime=lastFrame;
  fps=frameCount;
  frameCount=0;
 }
 update();
 input();
 render();

}

/* mi ravad */
TTF_CloseFont(font);
SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);
SDL_Quit();
TTF_Quit();





return 0;
}
