#include "rlib/raylib.h"
#include <stdio.h>
#include <stdlib.h>

// tudo será quadrado. assim, tamanhos w e h serao sempre iguais.
#define WIDTH 1000
#define HEIGHT 1000

#define GATE_LIMIT 500
#define CONEX_LIMIT 5000

#define GATE_SIZE 60
#define PIN_SIZE 20
#define WIRE_THICK 5

#define INPUT_COOLDOWN 0.2

typedef enum{
    NONE,
    GATE,
    PIN,
} ObjTipo;

typedef enum{
    NOT,

    OR,
    AND,
    XOR,
} Tipo;

typedef struct{
    int size, x, y;
} SizeCoord;


typedef struct Pin{
    SizeCoord sc;
    int power;

    struct Pin* conexoes;
    size_t count_conex;
} Pin;

typedef struct{
    SizeCoord sc;

    Pin* pins_in;
    size_t count_pi;

    Pin* pins_out;
    size_t count_po;

    Tipo tipo;  
} Portao;







Portao* portoes;
size_t count_portoes;


//merdamoujse
Vector2 mouse_coords;

void* objeto_no_mouse;
ObjTipo onm_tipo;

void* objeto_clicado; 
ObjTipo onm_clicado;
int new_interaction = 0;
//-----


int withinBounds(int x, int y, SizeCoord sc){
    if ((x >= sc.x && x <= sc.x+sc.size) && (y >= sc.y && y <= sc.y+sc.size)){
        return 1;
    }
    return 0;
}

int newPortao(int x, int y, Tipo tipo){ // 0 = suces
    if (count_portoes == GATE_LIMIT){
        return 1;
    }

    size_t cpi = (tipo == NOT) ? 1 : 2;
    Pin* pi = malloc(cpi*sizeof(Pin));

    for (size_t i = 0 ; i < cpi; i++){
        SizeCoord sc = {PIN_SIZE, x-PIN_SIZE, y+(2*GATE_SIZE/3)*i};
        pi[i] = (Pin){sc, 0, NULL, 0};
    }
    
    size_t cpo = 1;
    Pin* po = malloc(cpo*sizeof(Pin));

    for (size_t i = 0 ; i < cpo; i++){
        SizeCoord sc = {PIN_SIZE, x+GATE_SIZE, y+(GATE_SIZE/3)*(i+1)};
        po[i] = (Pin){sc, 0, NULL, 0};
    }

    SizeCoord sc = {50,x,y};
    Portao np = {
        sc,

        pi,
        cpi,
        po,
        cpo,

        tipo
    };

    portoes[count_portoes] = np;

    count_portoes++;
    return 0;
}


//-----
void renderGame(){
    BeginDrawing();

    ClearBackground(WHITE);
    char po[100];
    snprintf(po, 100, "por: %lld", count_portoes);
    DrawText(po , 0, 0, 30, BLACK);


    for (size_t i = 0; i < count_portoes; i++){
        Portao* p = &portoes[i];

        for (size_t j = 0; j < p->count_pi; j++){
            DrawRectangle(p->pins_in[j].sc.x, p->pins_in[j].sc.y, p->pins_in[j].sc.size, p->pins_in[j].sc.size, GRAY);
        }
        for (size_t j = 0; j < p->count_po; j++){
            DrawRectangle(p->pins_out[j].sc.x, p->pins_out[j].sc.y, p->pins_out[j].sc.size, p->pins_out[j].sc.size, GRAY);
        }

        DrawRectangle(p->sc.x, p->sc.y, GATE_SIZE, GATE_SIZE, BLACK);

        char nome[5];
        switch (p->tipo){
            case NOT:
                snprintf(nome, 10, "not");
                break;
            case OR:
                snprintf(nome, 10, "or");
                break;
            case AND:
                snprintf(nome, 10, "and");
                break;
            case XOR:
                snprintf(nome, 10, "xor");
                break;
            default:
                snprintf(nome, 10, "?");
                break;
        }

        DrawText(nome, p->sc.x+5, p->sc.y+5, GATE_SIZE/3, WHITE);
    }

    EndDrawing();
}

void handleKeyboardInput(){
    if (IsKeyDown(KEY_O)){
        newPortao((int)mouse_coords.x, (int)mouse_coords.y, OR);
    }
    if (IsKeyDown(KEY_A)){
        newPortao((int)mouse_coords.x, (int)mouse_coords.y, AND);
    }
    if (IsKeyDown(KEY_X)){
        newPortao((int)mouse_coords.x, (int)mouse_coords.y, XOR);
    }
    if (IsKeyDown(KEY_N)){
        newPortao((int)mouse_coords.x, (int)mouse_coords.y, NOT);
    }

}

void detectMouseObj(){
    int mX = mouse_coords.x;
    int mY = mouse_coords.y;
    objeto_no_mouse = NULL;
    onm_tipo = NONE;

    if (objeto_no_mouse != NULL){
        goto fora_deteccao_obj;
    }

    for (size_t i = 0; i < count_portoes; i++){
        Portao* p = &portoes[i];
        if (withinBounds(mX, mY, portoes[i].sc)){
            objeto_no_mouse = p;
            onm_tipo = GATE;
            goto fora_deteccao_obj;
        }

        for (size_t j = 0; j < p->count_pi; j++){
            if (withinBounds(mX, mY, portoes[i].pins_in[j].sc)){
                objeto_no_mouse = &portoes[i].pins_in[j];
                onm_tipo = PIN;
                goto fora_deteccao_obj;
            }
        }

        for (size_t j = 0; j < p->count_po; j++){
            if (withinBounds(mX, mY, portoes[i].pins_out[j].sc)){
                objeto_no_mouse = &portoes[i].pins_out[j];
                onm_tipo = PIN;
                goto fora_deteccao_obj;
            }
        }
    }
    fora_deteccao_obj:
}

void handleMouseInput(){
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
        if (new_interaction == 0){
            objeto_clicado = objeto_no_mouse; 
            onm_clicado = onm_tipo;
            new_interaction++;
        }

        if (onm_clicado == GATE){
            Portao* g = objeto_clicado;
            float mx = mouse_coords.x-(g->sc.size/2);
            float my = mouse_coords.y-(g->sc.size/2);


            g->sc.x = mx;
            g->sc.y = my;

            for (size_t i = 0; i < g->count_pi; i++){
                SizeCoord sc = {PIN_SIZE, mx-PIN_SIZE, my+(2*GATE_SIZE/3)*i};
                g->pins_in[i].sc = sc;

            }

            for (size_t i = 0; i < g->count_po; i++){
                SizeCoord sc = {PIN_SIZE, mx+GATE_SIZE, my+(GATE_SIZE/3)*(i+1)};
                g->pins_out[i].sc = sc;
            }

        }
    
        if (onm_clicado == PIN){
            Pin* p = objeto_clicado;

            Vector2 start = {p->sc.x+(p->sc.size/2), p->sc.y+(p->sc.size/2)};
            Vector2 end = {mouse_coords.x, mouse_coords.y};

            DrawLineEx(start, end, WIRE_THICK, GOLD);
        }
    }

    if (IsMouseButtonUp(MOUSE_LEFT_BUTTON)){
        objeto_clicado = NULL; 
        onm_clicado = NONE;
        new_interaction = 0;
    }
}


//-----



int main(){
    portoes = malloc(GATE_LIMIT*sizeof(Portao));
    count_portoes = 0;


    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WIDTH, HEIGHT, "jogodoavida");
    //-----

    double last_input_time = GetTime();
    while (!WindowShouldClose()){
        double current_time = GetTime();
        mouse_coords = GetMousePosition();
        
        if (current_time-last_input_time > INPUT_COOLDOWN){
            handleKeyboardInput();
            last_input_time = current_time;
        }
        
        detectMouseObj();
        
        handleMouseInput();

        
        printf("(%p, %p)\n", objeto_no_mouse, objeto_clicado);
        renderGame();
        
    }

    CloseWindow();
    return 0;
}