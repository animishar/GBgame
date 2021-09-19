#include<gb/gb.h>
#include<stdio.h>
#include<stdlib.h>
#include"GameCharacter.c"
#include"Player_sprite.c"
#include"textures.c"
#include"map1.c"
#include"map2.c"
#include"splashscreen_data.c"
#include"splashscreen_map.c"

struct GameCharacter player;
const char blank[1] = {0x00};
UBYTE flag = 0;
INT16 current_spY;
INT8 gravity = -2;
UINT8 spritesize = 8;
BYTE jump_state; 
BYTE facing = 1;
UBYTE Debug;
UINT8 level = 1;
UBYTE hasKey= 0;
void performantdelay(UINT8 numloops){
    UINT8 i;
    for(i = 0; i < numloops; i++){
        wait_vbl_done();
    }     
}

void movegamecharacter(struct GameCharacter* character, UINT8 x, UINT8 y){
    move_sprite(character->SpID[0], x, y);
    move_sprite(character->SpID[1], x, y + spritesize);
}
INT8 canMove(UINT8 x, UINT8 y){
    UINT16 xi,yi,TI;
    unsigned char *map;
    if(level == 1)
        map = map1;
    else if(level == 2)
        map = map2;
    else 
        map = map2;
    xi = (x-8)/8;
    yi = (y)/8;
    TI = 20*yi+xi;
    map += TI; 
    if(Debug){
        printf("%u %u\n",(UINT16)(x),(UINT16)(y));
        printf("%u %u %u\n",(UINT16)xi,(UINT16)yi,(UINT16)TI);   
    } 
    if((TI == 74) && (level == 2)){
        hasKey = 1;
        set_bkg_tiles(14,3,1,1,blank[0]);
        return 1;
    }
    else if((TI == 259) && (hasKey) && (level == 2)) {
        set_bkg_tiles(19,12,1,1,blank[0]);
        set_bkg_tiles(19,11,1,1,blank[0]);
        set_bkg_tiles(19,10,1,1,blank[0]);
        return 1;
    }
    return (*map == blank[0]);
}
INT8 isGrounded(UINT8 x, UINT8 y){
    UINT16 xi,yi,TI;
    char *map = map1;
    if(level == 1)
        map = map1;
    else if(level == 2)
        map = map2;
    else
        map = map2;
    xi = (x-8)/8;
    yi = (y)/8;
    TI = 20*yi+xi;
    map += TI;
    if(*map == blank[0]){
        return -1;
    }
    else
        return(y-8);
}

void jump(struct GameCharacter* character){
    INT8 possiblesurfaceY;
    character->y -= 5;
    if(jump_state == 0){
        jump_state = 1;
        current_spY = 10;
    }
    current_spY = current_spY + gravity;    

    character->y = character->y - current_spY;

    possiblesurfaceY = isGrounded(character->x,character->y + spritesize);
    if(possiblesurfaceY != -1){
        jump_state = 0;
        move_sprite(character->SpID[0],character->x,possiblesurfaceY);
        move_sprite(character->SpID[1],character->x,possiblesurfaceY+spritesize);
    }
    else{
        move_sprite(character->SpID[0],character->x,character->y);
        move_sprite(character->SpID[1],character->x,character->y+spritesize);
    }

}
void exertGravity(struct GameCharacter* character){
    while(isGrounded(character->x, character->y - 2) == -1){
        //current_spY += gravity;
        character->y += 1;
        movegamecharacter(&player,player.x,character->y);
        
    }
}
void setup_player(){
    player.x = 20;
    player.y = 104;
    player.width = 8;
    player.height = 16;
    set_sprite_tile(0,0);
    player.SpID[0] = 0;
    set_sprite_tile(1,1);
    player.SpID[1] = 1;
}


void main(){
    set_bkg_data(0,99,splashscreen_data);
    set_bkg_tiles(0, 0, 20, 18,splashscreen_map);
    SHOW_SPRITES;
    SHOW_BKG;
    DISPLAY_ON;
    waitpad(J_A);

    set_bkg_data(0,12,textures);
    set_bkg_tiles(0,0,20,18,map1);
    set_sprite_data(0,8,PlayerSprite);
    setup_player();
    jump_state = 0;
    movegamecharacter(&player, player.x, player.y);
    NR52_REG = 0x80; 
    NR50_REG = 0x77; 
    NR51_REG = 0xFF;
    while(!((player.x >= 104 && player.x <= 135) && (player.y >= 130))){

        if(player.x >=155){
            set_bkg_tiles(0,0,20,18,map2);
            player.x = 20;
            player.y = 80;
            movegamecharacter(&player,player.x,player.y);
            if(level == 2 && hasKey == 1)
                printf("\t\t\n\n\n\n\nYou win");
            level++;
        }
        if(jump_state == 0)
            exertGravity(&player);
       if((joypad() & J_LEFT)){
           if(canMove(player.x-2,player.y-8)){
           player.x -= 2;
           movegamecharacter(&player, player.x, player.y);

           if(facing == 1){
               facing = 0;
               set_sprite_tile(0,6);
               set_sprite_tile(1,7);
                }
            }  
       }  
       if((joypad() & J_RIGHT)){
           if(canMove(player.x+ 2 +player.width ,player.y-8)){
           player.x += 2;
           movegamecharacter(&player, player.x, player.y);
           
           if(facing == 0){
               facing = 1;
               set_sprite_tile(0,0);
               set_sprite_tile(1,1);
               
                }
           }
       }
       if((joypad() & J_A ) || jump_state == 1){
           jump(&player);
           if(jump_state == 0){
           NR10_REG = 0x16;
           NR11_REG = 0x40;
           NR12_REG = 0x73;
           NR13_REG = 0x00;
           NR14_REG = 0xC3;	
           }
       }   
       if(joypad() & J_B){
            Debug = 1;
        }        
       performantdelay(3);      
    }
    if(flag !=1)
        {
            printf("game over");
            NR41_REG = 0x3A;
            NR42_REG = 0x94;
            NR43_REG = 0x6F;
            NR44_REG = 0xC0; 
        }

}
