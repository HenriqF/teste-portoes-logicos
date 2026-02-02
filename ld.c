#include "rlib/raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1000
#define HEIGHT 1000

#define OBJECT_LIMIT 50
#define PIN_FRACTION 3

#define OBJECT_SIZE 30
#define INPUT_COOLDOWN 0.5

typedef enum {
    NOT,

    OR,
    AND,
    XOR
} Tipo;

typedef struct{
    int width, height, x, y, being_moved;

    int inputs;
    int* ins;
    int outputs;
    int* ous;

    Tipo tipo;
} Objeto;


int max(int a, int b){
    return a > b ? a : b;
}

Objeto* objetos;
int objetos_count = 0;

Vector2 mouse_pos;

int newObjeto(int x, int y, int size, Tipo tipo){ // 0 -> ok, nao0 -> merda.
    if (objetos_count == OBJECT_LIMIT){
        return 1;
    }

    int ins = (tipo != NOT) ? 2 : 1;
    int ous = 1;

    Objeto new_ob = {
        size, (size/PIN_FRACTION)* (2*max(ins, ous) - 1), x, y, 0, 
        ins,
        malloc(sizeof(int)*ins),
        ous,
        malloc(sizeof(int)*ous),
        tipo
    };

    objetos[objetos_count] = new_ob;
    objetos_count++;

    return 0;
}


void renderJogo(){
    BeginDrawing();

    ClearBackground(WHITE);

    for (int i = 0; i < objetos_count; i++){
        if (objetos[i].being_moved == 1){
            objetos[i].x = 0;
            objetos[i].y = 0;
        }

        Color color = BLACK;

        switch (objetos[i].tipo){
            case NOT:
                color = RED;
                break;
            
            case OR:
                color = BLACK;
                break;
            
            case AND:
                color = BLACK;
                break;

            case XOR:
                color = BLACK;
                break;

            default:
                break;
        }
        
        DrawRectangle(objetos[i].x, objetos[i].y, objetos[i].width, objetos[i].height, color);


        int pw = OBJECT_SIZE / PIN_FRACTION;
        int ph = OBJECT_SIZE / PIN_FRACTION;

        //pins out
        int spaces = 2*objetos[i].outputs - 1;
        int hsp = objetos[i].height/spaces;
        for (int j = 0; j < spaces; j++){
            if (j % 2 == 0) DrawRectangle(objetos[i].x+objetos[i].width, objetos[i].y+(hsp*j), pw, ph, GRAY);
        }

        //pins in
        
        spaces = 2*objetos[i].inputs - 1;
        hsp = objetos[i].height/spaces;
        for (int j = 0; j < spaces; j++){
            if (j % 2 == 0) DrawRectangle(objetos[i].x-pw, objetos[i].y+(hsp*j), pw, ph, GRAY);
        }

        ph = objetos[i].height/ spaces;
    }
    
    EndDrawing();
}


int main(){
    double last_input_time = GetTime();
    objetos = malloc(OBJECT_LIMIT*sizeof(Objeto));

    Objeto* obj_no_mouse = NULL;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WIDTH, HEIGHT, "jogodoavida");

    while (!WindowShouldClose()){
        double current_time = GetTime();
        mouse_pos = GetMousePosition();

        renderJogo();
        
        if (obj_no_mouse == NULL){
            for (int i = 0; i < objetos_count; i++){
                if (mouse_pos.x >= objetos[i].x && mouse_pos.x <= objetos[i].x+objetos[i].width){
                    if (mouse_pos.y >= objetos[i].y && mouse_pos.y <= objetos[i].y+objetos[i].height){
                        obj_no_mouse = &objetos[i];
                        break;
                    } 
                }  
            }
        }

        if (current_time-last_input_time > INPUT_COOLDOWN){
            double time_holder = last_input_time;
            last_input_time = current_time;

            if (IsKeyDown(KEY_O)){
                newObjeto((int)mouse_pos.x- OBJECT_SIZE/2, (int)mouse_pos.y- OBJECT_SIZE/2, OBJECT_SIZE, OR);
            }
            else if (IsKeyDown(KEY_N)){
                newObjeto((int)mouse_pos.x- OBJECT_SIZE/2, (int)mouse_pos.y- OBJECT_SIZE/2, OBJECT_SIZE, NOT);
            }
            else if (IsKeyDown(KEY_A)){
                newObjeto((int)mouse_pos.x- OBJECT_SIZE/2, (int)mouse_pos.y- OBJECT_SIZE/2, OBJECT_SIZE, AND);
            }
            else if (IsKeyDown(KEY_X)){
                newObjeto((int)mouse_pos.x- OBJECT_SIZE/2, (int)mouse_pos.y- OBJECT_SIZE/2, OBJECT_SIZE, XOR);
            }
            else{
                last_input_time = time_holder;
            }
        }


        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            if (obj_no_mouse){
                obj_no_mouse -> x = (int) mouse_pos.x - OBJECT_SIZE/2;
                obj_no_mouse -> y = (int) mouse_pos.y - OBJECT_SIZE/2;
            }
        }
        if (IsMouseButtonUp(MOUSE_LEFT_BUTTON) && obj_no_mouse != NULL){
            obj_no_mouse = NULL;
        }
    }

    CloseWindow();
    return 0;
}