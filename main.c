#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <winsock.h>
#include <mysql.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define NOIR al_map_rgb( 0,0,0)
#define ROUGE al_map_rgb( 255,0,0)
#define BLANC al_map_rgb( 255,255,255)
#define VERT al_map_rgb( 0,255,0)


void findData(char variableName[],char value[]);

//Variable Globale
char loginDACTYLO[10];
char fontName[255];
char backgroundName[255];
int redColor;
int blueColor;
int greenColor;

ALLEGRO_DISPLAY *display;
ALLEGRO_BITMAP*image;
ALLEGRO_BITMAP*coursImg;
ALLEGRO_FONT*arial;
ALLEGRO_FONT*arial2;
ALLEGRO_EVENT_QUEUE *queue;
ALLEGRO_EVENT event;
ALLEGRO_TIMER*timer;

MYSQL mysql;

void changescreen(int screen);

void erreur(const char*txt){
    printf("%s", txt);
}

void findData(char variableName[],char value[]){
    int i;
    int counter=1;
    int finish=0;
    int encounter=0;
    int antiLoop=0;
    //config.txt
    FILE *file=fopen("config.txt", "r");

    while(feof(file)==0 && finish==0) {
        if(variableName[0] == fgetc(file)){
            for(i=1;i < (strlen(variableName));i++){
                if(variableName[i] == fgetc(file)){
                    counter++;
                    //printf("coutener dans le for %d \n",counter);
                }
            }
        }
        //printf("counter %d",counter);
        if(counter == strlen(variableName)-1){
            while(finish==0 && antiLoop<10){
                if ('"'==fgetc(file)) {
                    i=0;
                    do {
                        value[i]=fgetc(file);
                        //printf("\n%c",value[i]);
                        if(value[i]=='"' || value[i]=='\n'){
                            value[i]='\0';
                            encounter=1;
                            finish=1;
                        }
                        i++;
                    } while (encounter==0);
                }
                if(antiLoop==9){
                    printf("\nVous avez oublié de mettre les guillemets");
                    finish=1;
                }
                ++antiLoop;
            }
            antiLoop=0;
        }
        counter =0;
    }

}

//fonction pour se connecter a la BDD
int mysql_connection(){
    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, "localhost", "root", "root", "cproject", 3306, NULL, 0)){
        // error
        printf("\nMYSQL CONNECT ERROR");
        return -1;
    } else {
        printf("\nMYSQL CONNECT SUCCESS");
        return 0;
    }
}

//fonction pour ce connecter
int connexion(char login[],char pwd[]){
    int resCon = mysql_connection();
    char query[255];
    if(resCon==0){
        strcpy(query,"SELECT id FROM USER WHERE username=\"");
        strcat(query,login);
        strcat(query,"\" AND password=\"");
        strcat(query,pwd);
        strcat(query,"\";");
        if (mysql_query(&mysql, query)) {//si sa marche pas
            printf("\nRETRIEVING DATA ERROR1");
            mysql_close(&mysql);
        }else{
            MYSQL_RES *result = mysql_store_result(&mysql);
            if(result == NULL){
                fprintf(stderr, "%s\n", mysql_error(&mysql));
            }else{
                MYSQL_ROW row;
                row = mysql_fetch_row(result);
                if(row==NULL){
                    printf("\nTHERE IS NO ONE WITH THIS USERNAME");
                    mysql_close(&mysql);
                    mysql_free_result(result);
                    return -1;
                }else{
                    mysql_close(&mysql);
                    mysql_free_result(result);
                    return 1;
                }
            }
        }
    }
    return -1;
}

//fonction pour s'inscrire
int inscription(char login[],char pwd[]){
    mysql_init(&mysql);
    int resCon = mysql_connection();
    char query[255]="";
    if(resCon==0){

        strcpy(query,"SELECT username FROM USER WHERE username=\"");
        strcat(query,login);
        strcat(query,"\";");

       if (mysql_query(&mysql, query)) {//si sa marche pas
           printf("\nRETRIEVING DATA ERROR1");
           mysql_close(&mysql);
       }else{
           MYSQL_RES *result = mysql_store_result(&mysql);
           if(result == NULL){
               fprintf(stderr, "%s\n", mysql_error(&mysql));
           }else{

               MYSQL_ROW row;
               row = mysql_fetch_row(result);

               if(row==NULL){
                   strcpy(query,"INSERT INTO USER (username,password) VALUES (\"");
                   strcat(query,login);
                   strcat(query,"\",\"");
                   strcat(query,pwd);
                   strcat(query,"\");");
                   if (mysql_query(&mysql, query)) {//si sa marche pas
                       printf("\nINSERT ERROR");
                       fprintf(stderr, "%s\n", mysql_error(&mysql));
                       mysql_close(&mysql);
                       mysql_free_result(result);
                       return -2;
                   }else{
                       mysql_close(&mysql);
                       mysql_free_result(result);
                       return 1;
                   }
               }
               mysql_free_result(result);
               return -2;
           }
       }
    }
    return -3;
}

//fonction pour récuperer l'id d'un utilisateur
char *selectUser(char *username,char buffer[]){
    int result = mysql_connection();
    char *query;
    if(result==0){
       MYSQL_ROW row;
       query = malloc(sizeof(char)*130);
       strcpy(query,"SELECT id FROM USER WHERE username = \"");
       strcat(query, username);
       strcat(query,"\";");
       if (mysql_query(&mysql, query)) {//si sa marche pas
           free(query);
           printf("\nRETRIEVING DATA ERROR1");
           mysql_close(&mysql);
       }else{
           free(query);
           MYSQL_RES *result = mysql_store_result(&mysql);
           if(result == NULL){
               printf("\nTHERE IS NO ONE WITH THIS USERNAME");
               mysql_close(&mysql);
           }
           row = mysql_fetch_row(result);
           strcpy(buffer,row[0]);
           mysql_free_result(result);
           return buffer;
       }
    }
    return buffer;
}

//fonction pour inserer un score
void insertScore(int score, char *username){
    int resCon = mysql_connection();
    char query[255];
    char tmp[12];
    if(resCon==0){
        strcpy(query,"INSERT INTO SCORE(valeur,date,idUser) VALUES (");
        sprintf(tmp,"%d",score);
        strcat(query,tmp);
        strcat(query, ",CURDATE(),");
        strcat(query, selectUser(username,tmp));
        strcat(query,");");
        if (mysql_query(&mysql, query)) {
            printf("\nINSERT ERROR");
            mysql_close(&mysql);
        }else{
            printf("\nINSERT SUCCESS");
        }
    }
}

void processText(int *count,char *text,int keycode,int max){
    char let[76][2][250]={{"97", "a"},{"98", "b"},{"99", "c"},{"100", "d"},{"101", "e"},{"102", "f"},{"103", "g"},{"104", "h"},{"105", "i"},{"106", "j"},{"107", "k"},{"108", "l"},{"109", "m"},
    {"110", "n"},{"111", "o"},{"112", "p"},{"113", "q"},{"114", "r"},{"115", "s"},{"116", "t"},{"117", "u"},{"118", "v"},{"119", "w"},{"120", "x"},{"121", "y"},{"122", "z"},
    {"65", "A"},{"66", "B"},{"67", "C"},{"68", "D"},{"69", "E"},{"70", "F"},{"71", "G"},{"72", "H"},{"73", "I"},{"74", "J"},{"75", "K"},{"76", "L"},{"77", "M"},
    {"78", "N"},{"79", "O"},{"80", "P"},{"81", "Q"},{"82", "R"},{"83", "S"},{"84", "T"},{"85", "U"},{"86", "V"},{"87", "W"},{"88", "X"},{"89", "Y"},{"90", "Z"},
    {"48", "0"},{"49", "1"},{"50", "2"},{"51", "3"},{"52", "4"},{"53", "5"},{"54", "6"},{"55", "7"},{"56", "8"},{"57", "9"},
    {"233", "é"},{"232", "è"},{"224", "à"},
    {"46", "."},{"33", "!"},{"63", "?"},{"58", ":"},{"44", ","},{"39", "'"},{"45", "-"},{"40", "("},{"41", ")"},{"59", ";"},{"34", "\""}
    };
    if (max == 8 && (keycode == 233 || keycode == 232 || keycode == 224)){
        return;
    }
    if(keycode == 8||keycode == 127){ //backspace
        if(*count > 0){
            text[strlen(text) - 1]  = '\0';
            *count -=1;
        }
    }else{
        for (int i=0; i<=76; i++)
        {
            if(keycode == atoi(let[i][0]) && (*count < max)){
                strcat(text, let[i][1]);
                *count += 1;
            }
        }
    }
}

int compareText(int keycode,char testchar){
    char let[73][2][250]={{"97", "a"},{"98", "b"},{"99", "c"},{"100", "d"},{"101", "e"},{"102", "f"},{"103", "g"},{"104", "h"},{"105", "i"},{"106", "j"},{"107", "k"},{"108", "l"},{"109", "m"},
    {"110", "n"},{"111", "o"},{"112", "p"},{"113", "q"},{"114", "r"},{"115", "s"},{"116", "t"},{"117", "u"},{"118", "v"},{"119", "w"},{"120", "x"},{"121", "y"},{"122", "z"},
    {"65", "A"},{"66", "B"},{"67", "C"},{"68", "D"},{"69", "E"},{"70", "F"},{"71", "G"},{"72", "H"},{"73", "I"},{"74", "J"},{"75", "K"},{"76", "L"},{"77", "M"},
    {"78", "N"},{"79", "O"},{"80", "P"},{"81", "Q"},{"82", "R"},{"83", "S"},{"84", "T"},{"85", "U"},{"86", "V"},{"87", "W"},{"88", "X"},{"89", "Y"},{"90", "Z"},
    {"48", "0"},{"49", "1"},{"50", "2"},{"51", "3"},{"52", "4"},{"53", "5"},{"54", "6"},{"55", "7"},{"56", "8"},{"57", "9"},
    {"46", "."},{"33", "!"},{"63", "?"},{"58", ":"},{"44", ","},{"39", "'"},{"45", "-"},{"40", "("},{"41", ")"},{"59", ";"},{"34", "\""}
    };

    if(keycode == 32 && testchar == ' '){
        return 1;
    }else if (keycode == 32 && testchar != ' '){
        return -1;
    }
    else{
    for (int i=0; i<=73; i++)
        {
            if(keycode == atoi(let[i][0])){
                if(let[i][1][0] == testchar){
                    return 1;
                }else{
                    return -1;
                }
            }

        }
    }
    return 0;
}

void errorIA(char testchar,int error[]){
    char number[10][5] ={"0","1","2","3","4","5","6","7","8","9"};
    char special[11][5]={".","!","?",":",",","'","-","(",")",";","\""};
    char auriculaire[10][5]={"a","q","w","p","m","A","Q","W","P","M"};
    char annulaire[10][5]={"z","s","x","o","l","Z","S","X","O","L"};
    char majeur[10][5]={"e","d","c","i","k","E","D","C","I","K"};
    char index[22][5]={"r","f","v","t","g","b","y","h","n","u","j","R","F","V","T","G","B","Y","H","N","U","J"};

    int i;

    for(i = 0;i<10;i++){
        if (testchar == number[i][0]){
            error[0]+=1;
            return;
        }
    }

    for(i = 0;i<11;i++){
        if (testchar == special[i][0]){
            error[1]+=1;
            return;
        }
    }

    for(i = 0;i<10;i++){
        if (testchar == auriculaire[i][0]){
            error[2]+=1;
            return ;
        }
    }

    for(i = 0;i<10;i++){
        if (testchar == annulaire[i][0]){
            error[3]+=1;
            return;
        }
    }

    for(i = 0;i<10;i++){
        if (testchar == majeur[i][0]){
            error[4]+=1;
            return;
        }
    }

    for(i = 0;i<22;i++){
        if (testchar == index[i][0]){
            error[5]+=1;
            return;
        }
    }
    return;
}


void incErrorDb(int error[]){ // si test général
/*
error[0] -> number
error[1] -> speciale
error[2] -> auriculaire
error[3] -> annulaire
error[4] -> majeur
error[5] -> index
error[6] -> accent
*/
    int resCon = mysql_connection();
    char tmp[12];
    char query[255];
    int i;
    if(resCon==0){
        //INSERT INTO COMETTRE (gravite,idUser,idErreur) VALUES (error[0]);
        //SELECT gravite FROM COMETTRE INNER JOIN USER ON idUser = USER.id WHERE USER.id =
        strcpy(query, "SELECT gravite FROM COMETTRE INNER JOIN USER ON idUser = USER.id WHERE USER.id =");
        strcat(query, selectUser(loginDACTYLO,tmp));
        strcat(query, ";");
        if (mysql_query(&mysql, query)) {
            printf("\nRETRIEVE DATA ERROR");
            mysql_close(&mysql);
        }else{
            MYSQL_RES *result = mysql_store_result(&mysql);
            if(result == NULL){
               fprintf(stderr, "%s\n", mysql_error(&mysql));
            }else{
                MYSQL_ROW row;
                row = mysql_fetch_row(result);
                if(row==NULL){
                    for(i=0;i<7;++i){
                        strcpy(query,"INSERT INTO COMETTRE (gravite,idUser,idErreur) VALUES (");
                        sprintf(tmp,"%d",error[i]);
                        strcat(query,tmp);
                        strcat(query, ",");
                        strcat(query, selectUser(loginDACTYLO,tmp));
                        strcat(query, ",");
                        sprintf(tmp,"%d",(i+1));
                        strcat(query, tmp);
                        strcat(query,");");
                        //printf("\n%s",query);
                        if (mysql_query(&mysql, query)) {
                            printf("\nINSERT ERROR %d",i+1);
                            mysql_close(&mysql);
                        }else{
                            printf("\nINSERT SUCCESS %d",i+1);
                        }
                    }
                }else{
                    //SELECT gravite FROM COMETTRE WHERE idUser=9;
                    strcpy(query, "SELECT gravite FROM COMETTRE WHERE idUser=");
                    strcat(query, selectUser(loginDACTYLO,tmp));
                    strcat(query,";");
                    if (mysql_query(&mysql, query)) {
                        printf("\nRETRIEVE DATA ERROR");
                        mysql_close(&mysql);
                    }else{
                        MYSQL_RES *result = mysql_store_result(&mysql);
                        if(result == NULL){
                            fprintf(stderr, "%s\n", mysql_error(&mysql));
                            mysql_free_result(result);
                        }else{
                            MYSQL_ROW row;
                            for(i=0;i<7;++i){
                                row = mysql_fetch_row(result);
                                error[i]=error[i]+atoi(row[0]);
                            }
                            mysql_free_result(result);
                            //UPDATE COMETTRE SET gravite=error[i] WHERE idUser=9 AND idErreur=i+1;
                            for(i=0;i<7;++i){
                                strcpy(query, "UPDATE COMETTRE SET gravite=");
                                sprintf(tmp,"%d",error[i]);
                                strcat(query,tmp);
                                strcat(query, " WHERE idUser=");
                                strcat(query, selectUser(loginDACTYLO,tmp));
                                strcat(query, " AND idErreur=");
                                sprintf(tmp,"%d",(i+1));
                                strcat(query, tmp);
                                strcat(query, ";");
                                if (mysql_query(&mysql, query)) {
                                    printf("\nUPDATE ERROR %d",i+1);
                                    mysql_close(&mysql);
                                }else{
                                    printf("\nUPDATE SUCCESS %d",i+1);
                                }
                            }

                        }
                    }
                }

            }
        }
    }
}


void redErrorDb(int error[], int courId, int mpm) { //si teste cours
    //printf("\n JE SUIS COURS ID %d ",courId);
    int errorCount;
    int gravite=0;
    errorCount = error[courId-1];
    int resCon = mysql_connection();
    char tmp[12];
    char query[255];
    if(resCon==0){
        strcpy(query,"SELECT gravite FROM COMETTRE  WHERE idUser=");
        strcat(query, selectUser(loginDACTYLO,tmp));
        strcat(query, " AND idErreur=");
        sprintf(tmp,"%d",courId);
        strcat(query,tmp);
        strcat(query,";");
        //printf("\n%s",query);
        if (mysql_query(&mysql, query)) {//si sa marche pas
           printf("\nRETRIEVING DATA ERROR1");
           mysql_close(&mysql);

       }else{
           MYSQL_RES *result = mysql_store_result(&mysql);
           if(result == NULL){
               printf("\nTHERE IS NO ERROR WITH THIS ID");
               mysql_free_result(result);
               mysql_close(&mysql);
           }else{
               MYSQL_ROW row;
               if((row = mysql_fetch_row(result))){
                   gravite = atoi(row[0]);
                   //printf("\n\n%d",gravite);
                   if(mpm > 20){
                       if(errorCount<3){
                           gravite= gravite - gravite/2;
                       }else if(errorCount<7){
                           gravite = gravite - gravite/4;
                       }
                   }else if(mpm >30){
                        if(errorCount<3){
                           gravite= gravite - (gravite/4)*3;
                       }else if(errorCount<7){
                           gravite = gravite - gravite/2;
                       }
                   }else if (mpm > 40){
                        if(errorCount<10){
                           gravite= 0;
                       }else{
                           gravite = gravite - gravite/2;
                       }
                   }
                   strcpy(query, "UPDATE COMETTRE SET gravite =");
                   sprintf(tmp,"%d",gravite);
                   strcat(query, tmp);
                   strcat(query, " WHERE idUser=");
                   strcat(query, selectUser(loginDACTYLO,tmp));
                   strcat(query, " AND idErreur=");
                   sprintf(tmp,"%d",courId);
                   strcat(query,tmp);
                   strcat(query, ";");
                   //printf("\n%s",query);
                   if (mysql_query(&mysql, query)) {//si sa marche pas
                       printf("\nUPDATE DATA ERROR1");
                       mysql_close(&mysql);
                   }else{
                       printf("\n UPDATE SUCCESS");
                   }
                   mysql_free_result(result);
               }

           }
       }
    }
}

int recommandation(){
    char query[255];
    char tmp[12];
    int max=0;
    int counter=0;
    int resCount=0;
    int resCon = mysql_connection();
    if (resCon==0){
        //SELECT gravite FROM COMETTRE WHERE idUser=2,
        strcpy(query,"SELECT gravite FROM COMETTRE WHERE idUser=" );
        strcat(query, selectUser(loginDACTYLO,tmp));
        strcat(query, ";");
        if (mysql_query(&mysql, query)) {//si sa marche pas
            printf("\nRETRIEVING DATA ERROR1");
            mysql_close(&mysql);
            return 8;
        }else{
            MYSQL_RES *result = mysql_store_result(&mysql);
            if(result == NULL){
                printf("\nTHERE IS NO DATA");
                mysql_free_result(result);
                mysql_close(&mysql);
                return 8;
            }else{
                MYSQL_ROW row;
                row = mysql_fetch_row(result);
                if (row==NULL) {
                    return 8;
                }else{
                    max = atoi(row[0]);
                    ++counter;
                    ++resCount;
                    while((row = mysql_fetch_row(result))){
                        ++counter;
                        if( max < atoi(row[0]) ){
                            max =  atoi(row[0]);
                            resCount=counter;
                        }
                    }
                    mysql_free_result(result);
                    return resCount;
                }
            }
        }

    }
    return 8;
}

void firstScreen(){
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    int tx, ty, screenx, screeny;

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    screenx = al_get_display_width(display);
    screeny = al_get_display_height(display);

    arial = al_load_font(fontName, 50, 0);

    image = al_load_bitmap(backgroundName);
    if(!image)
        erreur("error");

    tx = al_get_bitmap_width(image);
    ty = al_get_bitmap_height(image);

    al_set_window_title( display, "DACTYLOC");

    int end = 0;
    int screen=-1;

    while(!end){

        al_draw_scaled_bitmap(image, 0, 0, tx, ty, 0, 0, screenx, screeny, 0);

        //affichage
        al_draw_rectangle(screenx-75,25,screenx-25,75,ROUGE,2);
        al_draw_text(arial,ROUGE,screenx-50,25,ALLEGRO_ALIGN_CENTRE,"X");
        al_draw_text(arial,NOIR,screenx-(screenx/2),50,ALLEGRO_ALIGN_CENTRE,"DACTYLOC");

        al_draw_rectangle(screenx-(screenx/3)*2,300,screenx-(screenx/3),375,NOIR,2);
        al_draw_text(arial,NOIR,screenx-(screenx/2),310,ALLEGRO_ALIGN_CENTRE,"CONNEXION");
        al_draw_rectangle(screenx-(screenx/3)*2,425,screenx-(screenx/3),500,NOIR,2);
        al_draw_text(arial,NOIR,screenx-(screenx/2),435,ALLEGRO_ALIGN_CENTRE,"INSCRIPTION");

        al_flip_display();


        al_wait_for_event(queue, &event);

        //event

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            if (event.mouse.x >= (screenx-75) && event.mouse.x < (screenx-25) &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
              end= 1;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 300 && event.mouse.y < 375)
            {
                screen = 1;
              end= 1;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 425 && event.mouse.y < 500)
            {
              screen = 2;
              end= 1;
            }

        }
    }

    al_destroy_bitmap(image);
    al_destroy_event_queue(queue);
    changescreen(screen);
}

void loginScreen(){

    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    int tx, ty, screenx, screeny;

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    screenx = al_get_display_width(display);
    screeny = al_get_display_height(display);

    arial = al_load_font(fontName, 50, 0);

    image = al_load_bitmap(backgroundName);
    if(!image)
        erreur("error");

    tx = al_get_bitmap_width(image);
    ty = al_get_bitmap_height(image);

    al_set_window_title( display, "CONNEXION");

    int end = 0;
    int screen= -1;
    char login[255]="";
    char mdp[255]="";
    int countMdp = 0;
    int countLog = 0;
    int nbBox = -1;
    int maxSize = 8;
    int errorDisplay = 0;

    while(!end){

        al_draw_scaled_bitmap(image, 0, 0, tx, ty, 0, 0, screenx, screeny, 0);

        //affichage
        al_draw_rectangle(screenx-75,25,screenx-25,75,ROUGE,2);
        al_draw_text(arial,ROUGE,screenx-50,25,ALLEGRO_ALIGN_CENTRE,"X");
        al_draw_rectangle(75,25,25,75,NOIR,2);
        al_draw_text(arial,NOIR,50,25,ALLEGRO_ALIGN_CENTRE,"<");
        al_draw_text(arial,NOIR,screenx-(screenx/2),50,ALLEGRO_ALIGN_CENTRE,"CONNEXION");

        if(nbBox == 1){
            al_draw_rectangle(screenx-(screenx/3)*2,300,screenx-(screenx/3),375,ROUGE,2);
            al_draw_text(arial,ROUGE,screenx-(screenx/3-25),310,0,"ID");
        }else{
            al_draw_rectangle(screenx-(screenx/3)*2,300,screenx-(screenx/3),375,NOIR,2);
            al_draw_text(arial,NOIR,screenx-(screenx/3-25),310,0,"ID");
        }

        if (nbBox == 2){
            al_draw_rectangle(screenx-(screenx/3)*2,425,screenx-(screenx/3),500,ROUGE,2);
            al_draw_text(arial,ROUGE,screenx-(screenx/3-25),435,0,"MDP");
        }else{
            al_draw_rectangle(screenx-(screenx/3)*2,425,screenx-(screenx/3),500,NOIR,2);
            al_draw_text(arial,NOIR,screenx-(screenx/3-25),435,0,"MDP");
        }

        al_draw_rectangle(screenx-(screenx/3)*2,550,screenx-(screenx/3),625,NOIR,2);
        al_draw_text(arial,NOIR,screenx-(screenx/2),560,ALLEGRO_ALIGN_CENTRE,"Se connecter");

        al_draw_textf(arial, NOIR, screenx/2, 310,ALLEGRO_ALIGN_CENTER, "%s",login);
        al_draw_textf(arial, NOIR, screenx/2, 435,ALLEGRO_ALIGN_CENTER, "%s",mdp);

        if(errorDisplay == 1){
            al_draw_text(arial,ROUGE,screenx-(screenx/10)*5,200,ALLEGRO_ALIGN_CENTRE,"L'IDENTIFIANT/LE MOT DE PASSE EST INCORRECT");
        }

        al_flip_display();

        al_wait_for_event(queue, &event);

        //event
        if (event.type == ALLEGRO_EVENT_KEY_CHAR){
            errorDisplay = 0;
            if(nbBox == 1){
                processText(&countLog,&login,event.keyboard.unichar,maxSize);
                //printf("%d \n",event.keyboard.unichar);
            }else if (nbBox == 2){
                processText(&countMdp,&mdp,event.keyboard.unichar,maxSize);
            }
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            if (event.mouse.x >= (screenx-75) && event.mouse.x < (screenx-25) &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
              end= 1;
            }
            else if (event.mouse.x >= 25 && event.mouse.x < 75 &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
                screen = 0;
              end= 1;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 300 && event.mouse.y < 375)
            {
                nbBox = 1;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 425 && event.mouse.y < 500)
            {
                nbBox = 2;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 550 && event.mouse.y < 625)
            {
                if(connexion(login,mdp)==1){
                    strcpy(loginDACTYLO,login);
                    screen = 3;
                    end = 1;
                }else{
                    errorDisplay = 1;
                }
            }

        }
    }

    al_destroy_bitmap(image);
    al_destroy_event_queue(queue);
    changescreen(screen);
}

void registerScreen(){

    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    int tx, ty, screenx, screeny;

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    screenx = al_get_display_width(display);
    screeny = al_get_display_height(display);

    arial = al_load_font(fontName, 50, 0);

    image = al_load_bitmap(backgroundName);
    if(!image)
        erreur("error");

    tx = al_get_bitmap_width(image);
    ty = al_get_bitmap_height(image);

    al_set_window_title( display, "INSCRIPTION");

    int end = 0;
    int screen= -1;
    char login[255]="";
    char mdp[255]="";
    int countMdp = 0;
    int countLog = 0;
    int nbBox = -1;
    int maxSize = 8;
    int errorDisplay = 0;
    int result;

    while(!end){

        al_draw_scaled_bitmap(image, 0, 0, tx, ty, 0, 0, screenx, screeny, 0);

        //affichage
        al_draw_rectangle(screenx-75,25,screenx-25,75,ROUGE,2);
        al_draw_text(arial,ROUGE,screenx-50,25,ALLEGRO_ALIGN_CENTRE,"X");
        al_draw_rectangle(75,25,25,75,NOIR,2);
        al_draw_text(arial,NOIR,50,25,ALLEGRO_ALIGN_CENTRE,"<");
        al_draw_text(arial,NOIR,screenx-(screenx/2),50,ALLEGRO_ALIGN_CENTRE,"INSCRIPTION");

        if(nbBox == 1){
            al_draw_rectangle(screenx-(screenx/3)*2,300,screenx-(screenx/3),375,ROUGE,2);
            al_draw_text(arial,ROUGE,screenx-(screenx/3-25),310,0,"ID");
        }else{
            al_draw_rectangle(screenx-(screenx/3)*2,300,screenx-(screenx/3),375,NOIR,2);
            al_draw_text(arial,NOIR,screenx-(screenx/3-25),310,0,"ID");
        }

        if (nbBox == 2){
            al_draw_rectangle(screenx-(screenx/3)*2,425,screenx-(screenx/3),500,ROUGE,2);
            al_draw_text(arial,ROUGE,screenx-(screenx/3-25),435,0,"MDP");
        }else{
            al_draw_rectangle(screenx-(screenx/3)*2,425,screenx-(screenx/3),500,NOIR,2);
            al_draw_text(arial,NOIR,screenx-(screenx/3-25),435,0,"MDP");
        }

        al_draw_rectangle(screenx-(screenx/3)*2,550,screenx-(screenx/3),625,NOIR,2);
        al_draw_text(arial,NOIR,screenx-(screenx/2),560,ALLEGRO_ALIGN_CENTRE,"S'inscrire");

        al_draw_textf(arial, NOIR, screenx/2, 310,ALLEGRO_ALIGN_CENTER, "%s",login);
        al_draw_textf(arial, NOIR, screenx/2, 435,ALLEGRO_ALIGN_CENTER, "%s",mdp);

        if(errorDisplay == 1){
            al_draw_text(arial,ROUGE,screenx-(screenx/10)*5,200,ALLEGRO_ALIGN_CENTRE,"CONNEXION MYSQL ERROR");
        }else if(errorDisplay == 2){
            al_draw_text(arial,ROUGE,screenx-(screenx/10)*5,200,ALLEGRO_ALIGN_CENTRE,"IDENTIFIANT INDISPONIBLE");
        }

        al_flip_display();

        al_wait_for_event(queue, &event);

        //event
        if (event.type == ALLEGRO_EVENT_KEY_CHAR){
            errorDisplay = 0;
            if(nbBox == 1){
                processText(&countLog,&login,event.keyboard.unichar,maxSize);
            }else if (nbBox == 2){
                processText(&countMdp,&mdp,event.keyboard.unichar,maxSize);
            }
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            if (event.mouse.x >= (screenx-75) && event.mouse.x < (screenx-25) &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
              end= 1;
            }
            else if (event.mouse.x >= 25 && event.mouse.x < 75 &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
                screen = 0;
              end= 1;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 300 && event.mouse.y < 375)
            {
                nbBox = 1;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 425 && event.mouse.y < 500)
            {
                nbBox = 2;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 550 && event.mouse.y < 625)
            {
               result = inscription(login, mdp);
                if(result ==-3){
                    errorDisplay = 1;
                }else if(result == -2 || strcmp(login,"")==0) {
                    errorDisplay = 2;
                }else{
                    strcpy(loginDACTYLO,login);
                    screen = 3;
                    end = 1;
                }
            }

        }
    }

    al_destroy_bitmap(image);
    al_destroy_event_queue(queue);
    changescreen(screen);
}

void menuScreen(){

    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    int tx, ty, screenx, screeny;

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    screenx = al_get_display_width(display);
    screeny = al_get_display_height(display);

    arial = al_load_font(fontName, 50, 0);

    image = al_load_bitmap(backgroundName);
    if(!image)
        erreur("error");

    tx = al_get_bitmap_width(image);
    ty = al_get_bitmap_height(image);

    al_set_window_title( display, "MENU");

    int end = 0;
    int screen = -1;
    char buffer[20]="Bienvenue ";
    strcat(buffer, loginDACTYLO);

    while(!end){

        al_draw_scaled_bitmap(image, 0, 0, tx, ty, 0, 0, screenx, screeny, 0);

        //affichage
        al_draw_rectangle(screenx-75,25,screenx-25,75,ROUGE,2);
        al_draw_text(arial,ROUGE,screenx-50,25,ALLEGRO_ALIGN_CENTRE,"X");
        al_draw_text(arial,NOIR,screenx-(screenx/2),50,ALLEGRO_ALIGN_CENTRE,buffer);

        al_draw_rectangle(screenx-(screenx/3)*2,200,screenx-(screenx/3),275,NOIR,2);
        al_draw_text(arial,NOIR,screenx-(screenx/2),210,ALLEGRO_ALIGN_CENTRE,"SE TESTER");

        al_draw_rectangle(screenx-(screenx/3)*2,325,screenx-(screenx/3),400,NOIR,2);
        al_draw_text(arial,NOIR,screenx-(screenx/2),335,ALLEGRO_ALIGN_CENTRE,"APPRENDRE");

        al_draw_rectangle(screenx-(screenx/3)*2,450,screenx-(screenx/3),525,NOIR,2);
        al_draw_text(arial,NOIR,screenx-(screenx/2),460,ALLEGRO_ALIGN_CENTRE,"STATISTIQUES");

        al_draw_rectangle(screenx-(screenx/3)*2,575,screenx-(screenx/3),650,NOIR,2);
        al_draw_text(arial,NOIR,screenx-(screenx/2),585,ALLEGRO_ALIGN_CENTRE,"DECONNEXION");

        al_flip_display();

        al_wait_for_event(queue, &event);

        //event

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            if (event.mouse.x >= (screenx-75) && event.mouse.x < (screenx-25) &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
              end= 1;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 200 && event.mouse.y < 275)
            {
                screen = 4;
                end = 1;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 325 && event.mouse.y < 400)
            {
                screen = 5;
                end = 1;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 450 && event.mouse.y < 525)
            {
                screen = 6;
                end = 1;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 575 && event.mouse.y < 650)
            {
                strcpy(loginDACTYLO,"");
                screen = 0;
                end = 1;
            }

        }
    }

    al_destroy_bitmap(image);
    al_destroy_event_queue(queue);
    changescreen(screen);
}

void testScreen(int id){
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    int tx, ty, screenx, screeny;

    timer = al_create_timer(1);

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue,al_get_timer_event_source(timer));

    screenx = al_get_display_width(display);
    screeny = al_get_display_height(display);

    arial = al_load_font(fontName, 50, 0);
    arial2 = al_load_font(fontName, 20, 0);

    image = al_load_bitmap(backgroundName);
    if(!image)
        erreur("error");

    tx = al_get_bitmap_width(image);
    ty = al_get_bitmap_height(image);

    al_set_window_title( display, "TEST");

    int antiLoop = 0;
    int end = 0;
    int count = 170;
    int screen = -1;
    int testBool= 0;

     char filePathName[255];
    switch (id) {
        case 1: strcpy(filePathName,"asset/test/cours1.txt");
            break;
        case 2: strcpy(filePathName,"asset/test/cours2.txt");;
            break;
        case 3: strcpy(filePathName,"asset/test/cours3.txt");;
            break;
        case 4: strcpy(filePathName,"asset/test/cours4.txt");;
            break;
        case 5: strcpy(filePathName,"asset/test/cours5.txt");;
            break;
        case 6: strcpy(filePathName,"asset/test/cours6.txt");;
            break;
        case 7: strcpy(filePathName,"asset/test/cours7.txt");;
            break;
        case 8: strcpy(filePathName,"asset/test/general.txt");;
            break;
    }
    FILE *file=fopen(filePathName, "r");

    //fseek(file, 0, SEEK_END);
    //long fSize = ftell(file);
    //printf("%ld",fSize);
    //fseek(file, 0, SEEK_SET);

    int n=0;
    int m = 1;
    char text[10][100]={" "," "," "," "," "," "," "," "," "," "};
    char temp;
    char test[255]="";
    int max = 15;
    int testCount = 0;
    int start = 0;
    int mpm = 0;
    char accent[10]="";
    int accentBool = -1;
    int error[7]={0,0,0,0,0,0,0};

    do{
         text[n][m] = fgetc(file);
         m+=1;
         if(m == 99){
            text[n][m]='\0';
            n+=1;
            m=1;
         }
    }while (feof(file) == 0);


    text[0][0] = '_';
    text[1][0] = ' ';
    text[2][0] = ' ';
    text[3][0] = ' ';
    text[4][0] = ' ';
    text[5][0] = ' ';
    text[6][0] = ' ';
    text[7][0] = ' ';
    text[8][0] = ' ';
    text[9][0] = ' ';

    n = 0;
    m=0;
    fclose(file);

    while(!end){

        al_draw_scaled_bitmap(image, 0, 0, tx, ty, 0, 0, screenx, screeny, 0);

        //affichage
        al_draw_rectangle(screenx-75,25,screenx-25,75,ROUGE,2);
        al_draw_text(arial,ROUGE,screenx-50,25,ALLEGRO_ALIGN_CENTRE,"X");
        al_draw_rectangle(75,25,25,75,NOIR,2);
        al_draw_text(arial,NOIR,50,25,ALLEGRO_ALIGN_CENTRE,"<");
        al_draw_text(arial,NOIR,screenx-(screenx/2),50,ALLEGRO_ALIGN_CENTRE,"TEST");

        al_draw_filled_rectangle(screenx-(screenx/20)*19,150,screenx-(screenx/20),500,BLANC);

        if(start == 0){
            al_draw_rectangle(screenx-(screenx/4)*3,550,screenx-(screenx/4),625,NOIR,2);
        }else if (testBool == 1){
            al_draw_rectangle(screenx-(screenx/4)*3,550,screenx-(screenx/4),625,VERT,2);
        }else{
            al_draw_rectangle(screenx-(screenx/4)*3,550,screenx-(screenx/4),625,ROUGE,2);
        }
        al_draw_textf(arial, NOIR, screenx-(screenx/2), 560,ALLEGRO_ALIGN_CENTRE, "%s",test);

        al_draw_textf(arial2, al_map_rgb(redColor,greenColor,blueColor), screenx-(screenx/2), count,ALLEGRO_ALIGN_CENTRE, "%s",text[0]);
        al_draw_textf(arial2, al_map_rgb(redColor,greenColor,blueColor), screenx-(screenx/2), count+30,ALLEGRO_ALIGN_CENTRE, "%s",text[1]);
        al_draw_textf(arial2, al_map_rgb(redColor,greenColor,blueColor), screenx-(screenx/2), count+60,ALLEGRO_ALIGN_CENTRE, "%s",text[2]);
        al_draw_textf(arial2, al_map_rgb(redColor,greenColor,blueColor), screenx-(screenx/2), count+90,ALLEGRO_ALIGN_CENTRE, "%s",text[3]);
        al_draw_textf(arial2, al_map_rgb(redColor,greenColor,blueColor), screenx-(screenx/2), count+120,ALLEGRO_ALIGN_CENTRE, "%s",text[4]);
        al_draw_textf(arial2, al_map_rgb(redColor,greenColor,blueColor), screenx-(screenx/2), count+150,ALLEGRO_ALIGN_CENTRE, "%s",text[5]);
        al_draw_textf(arial2, al_map_rgb(redColor,greenColor,blueColor), screenx-(screenx/2), count+180,ALLEGRO_ALIGN_CENTRE, "%s",text[6]);
        al_draw_textf(arial2, al_map_rgb(redColor,greenColor,blueColor), screenx-(screenx/2), count+210,ALLEGRO_ALIGN_CENTRE, "%s",text[7]);
        al_draw_textf(arial2, al_map_rgb(redColor,greenColor,blueColor), screenx-(screenx/2), count+240,ALLEGRO_ALIGN_CENTRE, "%s",text[8]);
        al_draw_textf(arial2, al_map_rgb(redColor,greenColor,blueColor), screenx-(screenx/2), count+270,ALLEGRO_ALIGN_CENTRE, "%s",text[9]);


        al_draw_textf(arial, NOIR, screenx-(screenx/4), 50,ALLEGRO_ALIGN_CENTRE, "%d/60s",al_get_timer_count(timer));

        if(id == 8){
            al_draw_textf(arial, NOIR, screenx-(screenx/4)*3, 50,ALLEGRO_ALIGN_CENTRE, "MPM : %d",mpm);
        }else{
            al_draw_textf(arial, NOIR, screenx-(screenx/4)*3, 50,ALLEGRO_ALIGN_CENTRE, "FAUTES : %d",error[id-1]);
        }

        if (al_get_timer_count(timer) == 60 && start == 1){
            al_stop_timer(timer);

            if(antiLoop==0){
                if(id == 8){
                    incErrorDb(error);
                    insertScore(mpm, loginDACTYLO);
                    al_flip_display();
                    al_wait_for_event(queue, &event);

                }else{
                    redErrorDb(error, id,mpm);
                }
                antiLoop=1;
            }
            start = 0;
        }

        if (al_get_timer_count(timer) == 60){
            if(id == 8){
                al_draw_filled_rectangle(screenx-(screenx/10)*9,150,screenx-(screenx/10),500,BLANC);
                    al_draw_text(arial,NOIR,screenx-(screenx/2),210,ALLEGRO_ALIGN_CENTRE,"FINI");
                    al_draw_text(arial,NOIR,screenx-(screenx/2),270,ALLEGRO_ALIGN_CENTRE,"SCORE ENREGISTRÉ");
            }else{
                al_draw_filled_rectangle(screenx-(screenx/10)*9,150,screenx-(screenx/10),500,BLANC);
                al_draw_text(arial,NOIR,screenx-(screenx/2),210,ALLEGRO_ALIGN_CENTRE,"FINI");
            }
        }

        al_flip_display();

        al_wait_for_event(queue, &event);

        //event

        if (event.type == ALLEGRO_EVENT_KEY_CHAR){
            //printf("%d",event.keyboard.unichar);
            if(start == 0 && al_get_timer_count(timer) != 60){
                al_start_timer(timer);
                start = 1;
                antiLoop = 0;
            }
            if(start == 1){

                if(m<98){
                    accent[0] = text[n][m+1];
                    accent[1] = text[n][m+2];
                    accent[2]='\0';
                    //printf("%s",accent);
                }

                if(strcmp(accent,"é")== NULL && event.keyboard.unichar == 233){
                    testBool = 1;
                    accentBool = 1;
                }else if(strcmp(accent,"è")== NULL && event.keyboard.unichar == 232){
                    testBool = 1;
                    accentBool = 1;
                }else if(strcmp(accent,"à")== NULL && event.keyboard.unichar == 224){
                    testBool = 1;
                    accentBool = 1;
                }
                else if(strcmp(accent,"é")== NULL && event.keyboard.unichar != 233){
                    testBool = -1;
                    accentBool = 1;
                    error[6]+=1;
                }else if(strcmp(accent,"è")== NULL && event.keyboard.unichar != 232){
                    testBool = -1;
                    accentBool = 1;
                    error[6]+=1;
                }else if(strcmp(accent,"à")== NULL && event.keyboard.unichar != 224){
                    testBool = -1;
                    accentBool = 1;
                    error[6]+=1;
                }
                else{
                    testBool = -1;
                }

                if(testBool != 1){
                    if (m==98){
                        testBool = compareText(event.keyboard.unichar,text[n+1][1]);
                        //printf("%c | %d \n",text[n+1][1],m);
                    }else{
                        testBool = compareText(event.keyboard.unichar,text[n][m+1]);
                        //printf("%c | %d \n",text[n][m+1],m);
                    }
                }

                if(testBool == 1){
                    if(event.keyboard.unichar == 32 && text[n][m+1] == ' '){
                        test[0] = '\0';
                        mpm +=1;
                        testCount = 0;
                    }
                    if(accentBool != 1){
                        temp = text[n][m+1];
                        text[n][m+1] = text[n][m];
                        text[n][m] = temp;
                        m+=1;
                    }else{
                        temp = text[n][m];
                        text[n][m] = text[n][m+1];
                        text[n][m+1] = text[n][m+2];
                        text[n][m+2] = temp;
                        accentBool =-1;
                        m+=2;
                    }
                    if(m == 99){
                        temp=text[n][m];
                        n+=1;
                        m=1;
                        text[n][m-1] = text[n][m];
                        text[n][m] = temp;

                    }
                    processText(&testCount,&test,event.keyboard.unichar,max);
                }else if(accentBool == -1 && m == 98){
                    errorIA(text[n+1][1],error);
                }else if (accentBool == -1 && m!=98){
                    errorIA(text[n][m+1],error);
                }
                /*for(int j = 0; j<7;j++){
                    printf("%d |",error[j]);
                }*/
            }
        }
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            if (event.mouse.x >= (screenx-75) && event.mouse.x < (screenx-25) &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
              end= 1;
            }
		else if (event.mouse.x >= 25 && event.mouse.x < 75 &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
                screen = 3;
                end= 1;
            }
        }
    }

    al_destroy_bitmap(image);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
	changescreen(screen);
}

void interStatScreen(){
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    int tx, ty, screenx, screeny;

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    screenx = al_get_display_width(display);
    screeny = al_get_display_height(display);

    arial = al_load_font(fontName, 50, 0);

    image = al_load_bitmap(backgroundName);
    if(!image)
        erreur("error");

    tx = al_get_bitmap_width(image);
    ty = al_get_bitmap_height(image);

    al_set_window_title( display, "STATISTIQUES");

    int end = 0;
    int screen=-1;

    while(!end){

        al_draw_scaled_bitmap(image, 0, 0, tx, ty, 0, 0, screenx, screeny, 0);

        //affichage
        al_draw_rectangle(screenx-75,25,screenx-25,75,ROUGE,2);
        al_draw_text(arial,ROUGE,screenx-50,25,ALLEGRO_ALIGN_CENTRE,"X");
        al_draw_rectangle(75,25,25,75,NOIR,2);
        al_draw_text(arial,NOIR,50,25,ALLEGRO_ALIGN_CENTRE,"<");
        al_draw_text(arial,NOIR,screenx-(screenx/2),50,ALLEGRO_ALIGN_CENTRE,"STATISTIQUES");

        al_draw_rectangle(screenx-(screenx/3)*2,300,screenx-(screenx/3),375,NOIR,2);
        al_draw_text(arial,NOIR,screenx-(screenx/2),310,ALLEGRO_ALIGN_CENTRE,"MES SCORES");
        al_draw_rectangle(screenx-(screenx/3)*2,425,screenx-(screenx/3),500,NOIR,2);
        al_draw_text(arial,NOIR,screenx-(screenx/2),435,ALLEGRO_ALIGN_CENTRE,"LEADERBOARD");

        al_flip_display();

        al_wait_for_event(queue, &event);

        //event

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            if (event.mouse.x >= (screenx-75) && event.mouse.x < (screenx-25) &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
              end= 1;
            }
            else if (event.mouse.x >= 25 && event.mouse.x < 75 &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
                screen = 3;
                end= 1;
            }

            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 300 && event.mouse.y < 375)
            {
                screen = 7;
              end= 1;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 425 && event.mouse.y < 500)
            {
              screen = 8;
              end= 1;
            }

        }
    }

    al_destroy_bitmap(image);
    al_destroy_event_queue(queue);
    changescreen(screen);
}

void interCoursScreen(){
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    int tx, ty, screenx, screeny;

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    screenx = al_get_display_width(display);
    screeny = al_get_display_height(display);

    arial = al_load_font(fontName, 50, 0);

    image = al_load_bitmap(backgroundName);
    if(!image)
        erreur("error");

    tx = al_get_bitmap_width(image);
    ty = al_get_bitmap_height(image);

    al_set_window_title( display, "APPRENDRE");

    int end = 0;
    int screen=-1;
    int resFunction;

    while(!end){

        al_draw_scaled_bitmap(image, 0, 0, tx, ty, 0, 0, screenx, screeny, 0);

        //affichage
        al_draw_rectangle(screenx-75,25,screenx-25,75,ROUGE,2);
        al_draw_text(arial,ROUGE,screenx-50,25,ALLEGRO_ALIGN_CENTRE,"X");
        al_draw_rectangle(75,25,25,75,NOIR,2);
        al_draw_text(arial,NOIR,50,25,ALLEGRO_ALIGN_CENTRE,"<");
        al_draw_text(arial,NOIR,screenx-(screenx/2),50,ALLEGRO_ALIGN_CENTRE,"APPRENDRE");

        al_draw_rectangle(screenx-(screenx/3)*2,300,screenx-(screenx/3),375,NOIR,2);
        al_draw_text(arial,NOIR,screenx-(screenx/2),310,ALLEGRO_ALIGN_CENTRE,"RECOMMANDE");
        al_draw_rectangle(screenx-(screenx/3)*2,425,screenx-(screenx/3),500,NOIR,2);
        al_draw_text(arial,NOIR,screenx-(screenx/2),435,ALLEGRO_ALIGN_CENTRE,"LISTE");

        al_flip_display();

        al_wait_for_event(queue, &event);

        //event

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            if (event.mouse.x >= (screenx-75) && event.mouse.x < (screenx-25) &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
              end= 1;
            }
            else if (event.mouse.x >= 25 && event.mouse.x < 75 &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
                screen = 3;
                end= 1;
            }

            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 300 && event.mouse.y < 375)
            {
                resFunction = recommandation();
                switch (resFunction) {
                    case 1: screen = 20;
                        break;
                    case 2: screen = 21;
                        break;
                    case 3: screen = 22;
                        break;
                    case 4: screen = 23;
                        break;
                    case 5: screen = 24;
                        break;
                    case 6: screen = 25;
                        break;
                    case 7: screen = 26;
                        break;
                    case 8: screen = 57;
                        break;
                }
                end= 1;
            }
            else if (event.mouse.x >= (screenx-(screenx/3)*2) && event.mouse.x < (screenx-(screenx/3)) &&
                event.mouse.y >= 425 && event.mouse.y < 500)
            {
              screen = 10;
              end= 1;
            }

        }
    }

    al_destroy_bitmap(image);
    al_destroy_event_queue(queue);
    changescreen(screen);
}

void scoreScreen(){
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    MYSQL_ROW row;

    int tx, ty, screenx, screeny;

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    screenx = al_get_display_width(display);
    screeny = al_get_display_height(display);

    arial = al_load_font(fontName, 50, 0);

    image = al_load_bitmap(backgroundName);
    if(!image)
        erreur("error");

    tx = al_get_bitmap_width(image);
    ty = al_get_bitmap_height(image);

    al_set_window_title( display, "MES SCORES");

    int end = 0;
    int screen = -1;

    al_draw_scaled_bitmap(image, 0, 0, tx, ty, 0, 0, screenx, screeny, 0);

    int resCon = mysql_connection();
    if(resCon==0){
        char query[255]="";
        strcpy(query, "SELECT valeur,date FROM SCORE INNER JOIN USER ON USER.id = idUser WHERE username=\"");
        strcat(query, loginDACTYLO);
        strcat(query, "\" ORDER BY valeur DESC LIMIT 5;");
        if (mysql_query(&mysql,query)) {
               printf("\nSELECT ERROR");
               mysql_close(&mysql);
        }else{
        MYSQL_RES *result = mysql_store_result(&mysql);
        if(result == NULL){
            printf("\nNO SCORE YET");
            mysql_free_result(result);
            mysql_close(&mysql);
        }
        if((row = mysql_fetch_row(result))){
            al_draw_text(arial,NOIR,screenx-(screenx/10)*7,280,ALLEGRO_ALIGN_CENTRE,"1");
            al_draw_text(arial,NOIR,screenx-(screenx/10)*5,280,ALLEGRO_ALIGN_CENTRE,row[0]);
            al_draw_text(arial,NOIR,screenx-(screenx/10)*3,280,ALLEGRO_ALIGN_CENTRE,row[1]);
        }
        if((row = mysql_fetch_row(result))){
        al_draw_text(arial,NOIR,screenx-(screenx/10)*7,360,ALLEGRO_ALIGN_CENTRE,"2");
        al_draw_text(arial,NOIR,screenx-(screenx/10)*5,360,ALLEGRO_ALIGN_CENTRE,row[0]);
        al_draw_text(arial,NOIR,screenx-(screenx/10)*3,360,ALLEGRO_ALIGN_CENTRE,row[1]);
        }
        if((row = mysql_fetch_row(result))){
        al_draw_text(arial,NOIR,screenx-(screenx/10)*7,440,ALLEGRO_ALIGN_CENTRE,"3");
        al_draw_text(arial,NOIR,screenx-(screenx/10)*5,440,ALLEGRO_ALIGN_CENTRE,row[0]);
        al_draw_text(arial,NOIR,screenx-(screenx/10)*3,440,ALLEGRO_ALIGN_CENTRE,row[1]);
        }
        if((row = mysql_fetch_row(result))){
        al_draw_text(arial,NOIR,screenx-(screenx/10)*7,520,ALLEGRO_ALIGN_CENTRE,"4");
        al_draw_text(arial,NOIR,screenx-(screenx/10)*5,520,ALLEGRO_ALIGN_CENTRE,row[0]);
        al_draw_text(arial,NOIR,screenx-(screenx/10)*3,520,ALLEGRO_ALIGN_CENTRE,row[1]);
        }
        if((row = mysql_fetch_row(result))){
        al_draw_text(arial,NOIR,screenx-(screenx/10)*7,600,ALLEGRO_ALIGN_CENTRE,"5");
        al_draw_text(arial,NOIR,screenx-(screenx/10)*5,600,ALLEGRO_ALIGN_CENTRE,row[0]);
        al_draw_text(arial,NOIR,screenx-(screenx/10)*3,600,ALLEGRO_ALIGN_CENTRE,row[1]);
        }
        mysql_free_result(result);
        mysql_close(&mysql);
        }
    }
    //affichage
    al_draw_rectangle(screenx-75,25,screenx-25,75,ROUGE,2);
    al_draw_text(arial,ROUGE,screenx-50,25,ALLEGRO_ALIGN_CENTRE,"X");
    al_draw_rectangle(75,25,25,75,NOIR,2);
    al_draw_text(arial,NOIR,50,25,ALLEGRO_ALIGN_CENTRE,"<");
    al_draw_text(arial,NOIR,screenx-(screenx/2),50,ALLEGRO_ALIGN_CENTRE,"MES SCORES");

    al_draw_line(screenx-(screenx/5)*3,200,screenx-(screenx/5)*3,650,NOIR,3);
    al_draw_line(screenx-(screenx/5)*2,200,screenx-(screenx/5)*2,650,NOIR,3);
    al_draw_line(screenx-(screenx/5)*4,250,screenx-(screenx/5),250,NOIR,3);
    al_draw_text(arial,NOIR,screenx-(screenx/10)*7,200,ALLEGRO_ALIGN_CENTRE,"#");
    al_draw_text(arial,NOIR,screenx-(screenx/10)*5,200,ALLEGRO_ALIGN_CENTRE,"MPM");
    al_draw_text(arial,NOIR,screenx-(screenx/10)*3,200,ALLEGRO_ALIGN_CENTRE,"DATE");

    al_flip_display();

    while(!end){


        al_wait_for_event(queue, &event);

        //event

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            if (event.mouse.x >= (screenx-75) && event.mouse.x < (screenx-25) &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
              end= 1;
            }
		else if (event.mouse.x >= 25 && event.mouse.x < 75 &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
                screen = 6;
                end= 1;
            }
        }
    }

    al_destroy_bitmap(image);
    al_destroy_event_queue(queue);
	changescreen(screen);
}

void leaderboardScreen(){
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    MYSQL_ROW row;

    int tx, ty, screenx, screeny;

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    screenx = al_get_display_width(display);
    screeny = al_get_display_height(display);

    arial = al_load_font(fontName, 50, 0);

    image = al_load_bitmap(backgroundName);
    if(!image)
        erreur("error");

    tx = al_get_bitmap_width(image);
    ty = al_get_bitmap_height(image);

    al_set_window_title( display, "LEADERBOARD");

    int end = 0;
    int screen = -1;

    al_draw_scaled_bitmap(image, 0, 0, tx, ty, 0, 0, screenx, screeny, 0);

    int resCon = mysql_connection();
    if(resCon==0){
        if (mysql_query(&mysql,"SELECT username,valeur FROM (SELECT username,MAX(valeur) AS valeur FROM SCORE INNER JOIN USER ON USER.id = idUser GROUP BY idUser)AS valeur1 ORDER BY valeur DESC LIMIT 5;")) {
               printf("\nSELECT ERROR");
               mysql_close(&mysql);
        }else{
        MYSQL_RES *result = mysql_store_result(&mysql);
        if(result == NULL){
            printf("\nTHERE IS NO LEADERBOARD YET");
            mysql_free_result(result);
            mysql_close(&mysql);
        }
		
		char array[5][10]={"1","2","3","4","5"};
		for(int i = 0; i < 5; i++) {
			if((row = mysql_fetch_row(result))){
				al_draw_text(arial,NOIR,screenx-(screenx/10)*7,280,ALLEGRO_ALIGN_CENTRE,array[i]);
				al_draw_text(arial,NOIR,screenx-(screenx/10)*5,280,ALLEGRO_ALIGN_CENTRE,row[0]);
				al_draw_text(arial,NOIR,screenx-(screenx/10)*3,280,ALLEGRO_ALIGN_CENTRE,row[1]);
			}
        }
		/*
		
		if((row = mysql_fetch_row(result))){
        al_draw_text(arial,NOIR,screenx-(screenx/10)*7,360,ALLEGRO_ALIGN_CENTRE,"2");
        al_draw_text(arial,NOIR,screenx-(screenx/10)*5,360,ALLEGRO_ALIGN_CENTRE,row[0]);
        al_draw_text(arial,NOIR,screenx-(screenx/10)*3,360,ALLEGRO_ALIGN_CENTRE,row[1]);
        }
        if((row = mysql_fetch_row(result))){
        al_draw_text(arial,NOIR,screenx-(screenx/10)*7,440,ALLEGRO_ALIGN_CENTRE,"3");
        al_draw_text(arial,NOIR,screenx-(screenx/10)*5,440,ALLEGRO_ALIGN_CENTRE,row[0]);
        al_draw_text(arial,NOIR,screenx-(screenx/10)*3,440,ALLEGRO_ALIGN_CENTRE,row[1]);
        }
        if((row = mysql_fetch_row(result))){
        al_draw_text(arial,NOIR,screenx-(screenx/10)*7,520,ALLEGRO_ALIGN_CENTRE,"4");
        al_draw_text(arial,NOIR,screenx-(screenx/10)*5,520,ALLEGRO_ALIGN_CENTRE,row[0]);
        al_draw_text(arial,NOIR,screenx-(screenx/10)*3,520,ALLEGRO_ALIGN_CENTRE,row[1]);
        }
        if((row = mysql_fetch_row(result))){
        al_draw_text(arial,NOIR,screenx-(screenx/10)*7,600,ALLEGRO_ALIGN_CENTRE,"5");
        al_draw_text(arial,NOIR,screenx-(screenx/10)*5,600,ALLEGRO_ALIGN_CENTRE,row[0]);
        al_draw_text(arial,NOIR,screenx-(screenx/10)*3,600,ALLEGRO_ALIGN_CENTRE,row[1]);
        }
		*/
        mysql_free_result(result);
        mysql_close(&mysql);
        }
    }

    //affichage
    al_draw_rectangle(screenx-75,25,screenx-25,75,ROUGE,2);
    al_draw_text(arial,ROUGE,screenx-50,25,ALLEGRO_ALIGN_CENTRE,"X");
    al_draw_rectangle(75,25,25,75,NOIR,2);
    al_draw_text(arial,NOIR,50,25,ALLEGRO_ALIGN_CENTRE,"<");
    al_draw_text(arial,NOIR,screenx-(screenx/2),50,ALLEGRO_ALIGN_CENTRE,"LEADERBOARD");

    al_draw_line(screenx-(screenx/5)*3,200,screenx-(screenx/5)*3,650,NOIR,3);
    al_draw_line(screenx-(screenx/5)*2,200,screenx-(screenx/5)*2,650,NOIR,3);
    al_draw_line(screenx-(screenx/5)*4,250,screenx-(screenx/5),250,NOIR,3);
    al_draw_text(arial,NOIR,screenx-(screenx/10)*7,200,ALLEGRO_ALIGN_CENTRE,"#");
    al_draw_text(arial,NOIR,screenx-(screenx/10)*5,200,ALLEGRO_ALIGN_CENTRE,"NOM");
    al_draw_text(arial,NOIR,screenx-(screenx/10)*3,200,ALLEGRO_ALIGN_CENTRE,"MPM");

    al_flip_display();

    while(!end){

        al_wait_for_event(queue, &event);

        //event

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            if (event.mouse.x >= (screenx-75) && event.mouse.x < (screenx-25) &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
              end= 1;
            }
		else if (event.mouse.x >= 25 && event.mouse.x < 75 &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
                screen = 6;
                end= 1;
            }
        }
    }

    al_destroy_bitmap(image);
    al_destroy_event_queue(queue);
	changescreen(screen);
}

void listeScreen(){
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    MYSQL_ROW row;

    int tx, ty, screenx, screeny;

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    screenx = al_get_display_width(display);
    screeny = al_get_display_height(display);

    arial = al_load_font(fontName, 50, 0);

    image = al_load_bitmap(backgroundName);
    if(!image)
        erreur("error");

    tx = al_get_bitmap_width(image);
    ty = al_get_bitmap_height(image);

    al_set_window_title( display, "LISTE");

    int end = 0;
    int screen = -1;
    int countCours =0;

    al_draw_scaled_bitmap(image, 0, 0, tx, ty, 0, 0, screenx, screeny, 0);

    //affichage
    al_draw_rectangle(screenx-75,25,screenx-25,75,ROUGE,2);
    al_draw_text(arial,ROUGE,screenx-50,25,ALLEGRO_ALIGN_CENTRE,"X");
    al_draw_rectangle(75,25,25,75,NOIR,2);
    al_draw_text(arial,NOIR,50,25,ALLEGRO_ALIGN_CENTRE,"<");
    al_draw_text(arial,NOIR,screenx-(screenx/2),50,ALLEGRO_ALIGN_CENTRE,"LISTE");

    int resCon = mysql_connection();

    if(resCon==0){

        if (mysql_query(&mysql,"SELECT idCours FROM CONTIENT ORDER BY idCours ASC ")) {
               printf("\nSELECT ERROR");
               mysql_close(&mysql);
        }else{
        MYSQL_RES *result = mysql_store_result(&mysql);
        if(result == NULL){
            printf("\nTHERE IS NO LESSON YET");
            mysql_close(&mysql);
        }
        if((row = mysql_fetch_row(result))){
            al_draw_rectangle(screenx-(screenx/4)*3-50,200,screenx-(screenx/4)*2-50,275,NOIR,2);
            al_draw_text(arial,NOIR,screenx-(screenx/8)*5-50,210,ALLEGRO_ALIGN_CENTRE,"COURS 1");


            countCours =1;
        }
        if((row = mysql_fetch_row(result))){
            al_draw_rectangle(screenx-(screenx/4)*2+50,200,screenx-(screenx/4)+50,275,NOIR,2);
            al_draw_text(arial,NOIR,screenx-(screenx/8)*3+50,210,ALLEGRO_ALIGN_CENTRE,"COURS 2");

            countCours =2;
        }
        if((row = mysql_fetch_row(result))){
            al_draw_rectangle(screenx-(screenx/4)*3-50,325,screenx-(screenx/4)*2-50,400,NOIR,2);
            al_draw_text(arial,NOIR,screenx-(screenx/8)*5-50,335,ALLEGRO_ALIGN_CENTRE,"COURS 3");

            countCours =3;
        }
        if((row = mysql_fetch_row(result))){
            al_draw_rectangle(screenx-(screenx/4)*2+50,325,screenx-(screenx/4)+50,400,NOIR,2);
            al_draw_text(arial,NOIR,screenx-(screenx/8)*3+50,335,ALLEGRO_ALIGN_CENTRE,"COURS 4");


            countCours =4;
        }
        if((row = mysql_fetch_row(result))){
            al_draw_rectangle(screenx-(screenx/4)*3-50,450,screenx-(screenx/4)*2-50,525,NOIR,2);
            al_draw_text(arial,NOIR,screenx-(screenx/8)*5-50,460,ALLEGRO_ALIGN_CENTRE,"COURS 5");


            countCours =5;
        }
        if((row = mysql_fetch_row(result))){
            al_draw_rectangle(screenx-(screenx/4)*2+50,450,screenx-(screenx/4)+50,525,NOIR,2);
            al_draw_text(arial,NOIR,screenx-(screenx/8)*3+50,460,ALLEGRO_ALIGN_CENTRE,"COURS 6");

            countCours =6;
        }
        if((row = mysql_fetch_row(result))){
            al_draw_rectangle(screenx-(screenx/4)*3-50,575,screenx-(screenx/4)*2-50,650,NOIR,2);
            al_draw_text(arial,NOIR,screenx-(screenx/8)*5-50,585,ALLEGRO_ALIGN_CENTRE,"COURS 7");

            countCours =7;
        }
        if((row = mysql_fetch_row(result))){
            al_draw_rectangle(screenx-(screenx/4)*2+50,575,screenx-(screenx/4)+50,650,NOIR,2);
            al_draw_text(arial,NOIR,screenx-(screenx/8)*3+50,585,ALLEGRO_ALIGN_CENTRE,"COURS 8");

            countCours =8;
        }
        mysql_close(&mysql);
        }

    }

    al_flip_display();

    while(!end){


        al_wait_for_event(queue, &event);

        //event

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            if (event.mouse.x >= (screenx-75) && event.mouse.x < (screenx-25) &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
              end= 1;
            }
            else if (event.mouse.x >= 25 && event.mouse.x < 75 &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
                screen = 5;
                end= 1;
            }
            else if (event.mouse.x >= screenx-(screenx/4)*3-50 && event.mouse.x < screenx-(screenx/4)*2-50 &&
                event.mouse.y >= 200 && event.mouse.y < 275 && countCours >=1)
            {
                screen = 20;
                end= 1;
            }
            else if (event.mouse.x >= screenx-(screenx/4)*2+50 && event.mouse.x < screenx-(screenx/4)+50 &&
                event.mouse.y >= 200 && event.mouse.y < 275 && countCours >=2)
            {
                screen = 21;
                end= 1;
            }
            else if (event.mouse.x >= screenx-(screenx/4)*3-50 && event.mouse.x < screenx-(screenx/4)*2-50 &&
                event.mouse.y >= 325 && event.mouse.y < 400 && countCours >=3)
            {
                screen = 22;
                end= 1;
            }
            else if (event.mouse.x >= screenx-(screenx/4)*2+50 && event.mouse.x < screenx-(screenx/4)+50 &&
                event.mouse.y >= 325 && event.mouse.y < 400 && countCours >=4)
            {
                screen = 23;
                end= 1;
            }
            else if (event.mouse.x >= screenx-(screenx/4)*3-50 && event.mouse.x < screenx-(screenx/4)*2-50 &&
                event.mouse.y >= 450 && event.mouse.y < 525 && countCours >=5)
            {
                screen = 24;
                end= 1;
            }
            else if (event.mouse.x >= screenx-(screenx/4)*2+50 && event.mouse.x < screenx-(screenx/4)+50 &&
                event.mouse.y >= 450 && event.mouse.y < 525 && countCours >=6)
            {
                screen = 25;
                end= 1;
            }
            else if (event.mouse.x >= screenx-(screenx/4)*3-50 && event.mouse.x < screenx-(screenx/4)*2-50 &&
                event.mouse.y >= 575 && event.mouse.y < 650 && countCours >=7)
            {
                screen = 26;
                end= 1;
            }
            else if (event.mouse.x >= screenx-(screenx/4)*2+50 && event.mouse.x < screenx-(screenx/4)+50 &&
                event.mouse.y >= 575 && event.mouse.y < 650 && countCours >=8)
            {
                screen = 27;
                end= 1;
            }


        }
    }

    al_destroy_bitmap(image);
    al_destroy_event_queue(queue);
	changescreen(screen);
}

void coursScreen(int id){
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    MYSQL_ROW row;

    int tx, ty, screenx, screeny,txImg,tyImg;

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    screenx = al_get_display_width(display);
    screeny = al_get_display_height(display);

    arial = al_load_font(fontName, 50, 0);

    image = al_load_bitmap(backgroundName);
    if(!image)
        erreur("error");

    int resCon = mysql_connection();
    if(resCon==0){
        char tmp[12];
        char query[255];
        strcpy(query,"SELECT lien FROM IMAGE INNER JOIN CONTIENT ON idCours = IMAGE.id WHERE idCours=");
        sprintf(tmp,"%d",id);
        strcat(query, tmp);
        strcat(query, ";");
        //printf("\n%s",query);
        if (mysql_query(&mysql,query)) {
               printf("\nSELECT ERROR");
               mysql_close(&mysql);
        }else{
        MYSQL_RES *result = mysql_store_result(&mysql);
        if(result == NULL){
            printf("\nTHERE IS NO LESSON IS THAT ID YET");
            mysql_free_result(result);
            mysql_close(&mysql);
        }
        if((row = mysql_fetch_row(result))){
            coursImg = al_load_bitmap(row[0]);
        }
        mysql_free_result(result);
        mysql_close(&mysql);
        }

    }

    //coursImg = al_load_bitmap("asset//cours//cours.jpg");

    tx = al_get_bitmap_width(image);
    ty = al_get_bitmap_height(image);

    txImg = al_get_bitmap_width(coursImg);
    tyImg = al_get_bitmap_height(coursImg);

    al_set_window_title( display, "COURS");

    int end = 0;
    int screen = -1;

    al_draw_scaled_bitmap(image, 0, 0, tx, ty, 0, 0, screenx, screeny, 0);

    al_draw_scaled_bitmap(coursImg, 0, 0, txImg, tyImg, screenx-(screenx/8)*7, 155, screenx-(screenx/8)*2, 425, 0);

    //affichage
    al_draw_rectangle(screenx-75,25,screenx-25,75,ROUGE,2);
    al_draw_text(arial,ROUGE,screenx-50,25,ALLEGRO_ALIGN_CENTRE,"X");
    al_draw_rectangle(75,25,25,75,NOIR,2);
    al_draw_text(arial,NOIR,50,25,ALLEGRO_ALIGN_CENTRE,"<");
    al_draw_text(arial,NOIR,screenx-(screenx/2),50,ALLEGRO_ALIGN_CENTRE,"COURS");

    switch (id) {
        case 1:al_draw_text(arial,NOIR,screenx-(screenx/2),95,ALLEGRO_ALIGN_CENTRE,"NOMBRES");
            break;
        case 2:al_draw_text(arial,NOIR,screenx-(screenx/2),95,ALLEGRO_ALIGN_CENTRE,"CARACTERE SPECIALE");
            break;
        case 3:al_draw_text(arial,NOIR,screenx-(screenx/2),95,ALLEGRO_ALIGN_CENTRE,"AURICULAIRE");
            break;
        case 4:al_draw_text(arial,NOIR,screenx-(screenx/2),95,ALLEGRO_ALIGN_CENTRE,"ANNULAIRE");
            break;
        case 5:al_draw_text(arial,NOIR,screenx-(screenx/2),95,ALLEGRO_ALIGN_CENTRE,"MAJEUR");
            break;
        case 6:al_draw_text(arial,NOIR,screenx-(screenx/2),95,ALLEGRO_ALIGN_CENTRE,"INDEX");
            break;
        case 7:al_draw_text(arial,NOIR,screenx-(screenx/2),95,ALLEGRO_ALIGN_CENTRE,"ACCENTS");
            break;
        case 8:al_draw_text(arial,NOIR,screenx-(screenx/2),95,ALLEGRO_ALIGN_CENTRE,"GENERALE");
            break;
    }
    al_draw_rectangle(screenx-(screenx/8)*5,600,screenx-(screenx/8)*3,675,NOIR,2);
    al_draw_text(arial,NOIR,screenx-(screenx/2),610,ALLEGRO_ALIGN_CENTRE,"SE TESTER");

    al_flip_display();

    while(!end){


        al_wait_for_event(queue, &event);

        //event

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            if (event.mouse.x >= (screenx-75) && event.mouse.x < (screenx-25) &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
              end= 1;
            }
            else if (event.mouse.x >= 25 && event.mouse.x < 75 &&
                event.mouse.y >= 25 && event.mouse.y < 75)
            {
                screen = 3;
                end= 1;
            }
            else if (event.mouse.x >= screenx-(screenx/8)*5 && event.mouse.x < screenx-(screenx/8)*3 &&
                event.mouse.y >= 600 && event.mouse.y < 675)
            {
                switch (id) {
                    case 1: screen = 50;
                        break;
                    case 2: screen = 51;
                        break;
                    case 3: screen = 52;
                        break;
                    case 4: screen = 53;
                        break;
                    case 5: screen = 54;
                        break;
                    case 6: screen = 55;
                        break;
                    case 7: screen = 56;
                        break;
                    case 8: screen = 57;
                        break;
                }
                end= 1;
            }
        }
    }

    al_destroy_bitmap(image);
    al_destroy_event_queue(queue);
	changescreen(screen);
}
void changescreen(int screen){
    if(screen == 0){
        firstScreen();
    }else if(screen == 1){
        loginScreen();
    }else if(screen == 2){
        registerScreen();
    }else if (screen == 3){
        menuScreen();
    }else if (screen == 4){
        testScreen(8);
    }else if (screen == 5){
        interCoursScreen();
    }else if (screen == 6){
        interStatScreen();
    }else if(screen == 7){
        scoreScreen();
    }else if (screen == 8){
        leaderboardScreen();
    }else if(screen == 9){
        coursScreen(8);
    }else if (screen == 10){
        listeScreen();
    }else if (screen == 20){
        coursScreen(1);
    }else if (screen == 21){
        coursScreen(2);
    }else if(screen == 22){
        coursScreen(3);
    }else if(screen == 23){
        coursScreen(4);
    }else if(screen == 24){
        coursScreen(5);
    }else if(screen == 25){
        coursScreen(6);
    }else if(screen == 26){
        coursScreen(7);
    }else if(screen == 27){
        coursScreen(8);
    }else if(screen == 50){
        testScreen(1);
    }else if(screen == 51){
        testScreen(2);
    }else if(screen == 52){
        testScreen(3);
    }else if(screen == 53){
        testScreen(4);
    }else if(screen == 54){
        testScreen(5);
    }else if(screen == 55){
        testScreen(6);
    }else if(screen == 56){
        testScreen(7);
    }else if(screen == 57){
        testScreen(8);
    }else if(screen == -1){
        al_destroy_display(display);
    }

}

int main(int argc, char **argv)
{
    char fullscreen[10];
    char tmp[255];
    char color[12];
    int counter=0;
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    findData("fullscreen",fullscreen);
    //printf("\nfullscreen %s",fullscreen);
    if(strcmp(fullscreen,"yes")==0){
        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    }

    display=al_create_display(1280,720);

    findData("font",fontName );
    strcpy(tmp, "asset/font/");
    strcat(tmp, fontName);
    strcpy(fontName, tmp);

    findData("background",backgroundName);
    strcpy(tmp, "asset/background/");
    strcat(tmp, backgroundName);
    strcpy(backgroundName, tmp);

    findData("rouge",color);
    if(strcmp(color, "")!=0){
        redColor=atoi(color);
        ++counter;
    }
    findData("bleu",color);
    if(strcmp(color, "")!=0){
         blueColor=atoi(color);
        ++counter;
    }
    findData("vert",color);
    if(strcmp(color, "")!=0){
        greenColor=atoi(color);
        ++counter;
    }

    if(strcmp(fontName,"")!=0 && strcmp(backgroundName,"")!=0 && strcmp(fullscreen,"")!=0 && counter == 3 ){
        firstScreen();
    }else{
        printf("\nLes paramètres du fichier config.txt doivent être correctement rempli");
    }

    return 0;
}
