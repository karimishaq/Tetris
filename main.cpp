#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>
#include <ctime>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#define nkolom 10
#define nbaris 18
#define plevel 0
#define pkecepatan 1
#define pnilai 2
#define batas_baris 3

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

char kotak[5]="1111";
char galah[17]="0200020002000200";
char letert[10]="000030333";
char leterl1[10]="050050550";
char leterl2[10]="600600660";
char leterz1[10]="000077770";
char leterz2[10]="000880088";
char arr[300];
typedef char stringku[20];

stringku stulisan[4]={"Level : ", "Kecepatan : ", "Nilai : ", "berikutnya"};
stringku ntulisan[4]={"1", "1", "0", "0"};

int utama(SDL_Event *evt);
SDL_Renderer*  grenderer = NULL;
TTF_Font *huruf;
SDL_Color warna;

struct status{
    char ckepingan[25];
    char *nkepingan;
    int main;
    int skor;
    int cek;
    int jeda;
    int jkolom;
    int kecepatan=1;
    int level;
    int tombol;
    int x;
    int y;
    int cx;
    int cy;
    int wtulisan;
    int htulisan;
    SDL_Texture* gambar = NULL;
    SDL_Texture* kalah = NULL;
    Mix_Music *musik=NULL;
    Mix_Chunk *suara_runtuh=NULL;
    Mix_Chunk *suara_jatuh=NULL;
}stat;

int ganti_tombol(int x){
    stat.tombol=x;
    return 0;
}

int gantistatus(SDL_Renderer * latar, struct status *stt){
int i=0;
char s[100]="";
SDL_Rect rct, rcd;
SDL_Surface *tstatus, *tulisan;
SDL_Texture *status;
    Uint32 rm, gm, bm, am;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rm = 0xff000000;
        gm = 0x00ff0000;
        bm = 0x0000ff00;
        am = 0x000000ff;
    #else
        rm = 0x000000ff;
        gm = 0x0000ff00;
        bm = 0x00ff0000;
        am = 0xff000000;
    #endif

    rct.y=0;
    rct.x=0;
    rct.w=200;
    rct.h=200;

    tstatus=SDL_CreateRGBSurface(SDL_SWSURFACE, 200, 200, 32, rm, gm, bm, am);
    for(i=0;i<3;i++){
        strcpy(s, stulisan[i]);
        strcat(s, ntulisan[i]);
        tulisan=TTF_RenderText_Blended(huruf, s, warna);
        SDL_BlitSurface(tulisan, 0, tstatus, &rct);
        SDL_FreeSurface(tulisan);
        rct.y+=50;
    }

    status=SDL_CreateTextureFromSurface(latar, tstatus);
    rct.x=0;
    rct.y=0;
    rct.w=200;
    rct.h=200;
    rcd.x=nkolom*stat.cx+2;
    rcd.y=0;
    rcd.w=200;
    rcd.h=200;
    SDL_RenderCopy(latar, status, &rct, &rcd);
    SDL_FreeSurface(tstatus);
    SDL_DestroyTexture(status);
    return 0;
}

int acak(struct status *stt){
int x;
char *tmp;
    srand(time(0)+SDL_GetTicks());
    x=rand();
    switch(x%7){
        case 0:
            tmp=kotak;
            break;
        case 1:
            tmp=galah;
            break;
        case 2:
            tmp=letert;
            break;
        case 3:
            tmp=leterl1;
            break;
        case 4:
            tmp=leterl2;
            break;
        case 5:
            tmp=leterz1;
            break;
        case 6:
            tmp=leterz2;
            break;
        default:
            tmp=kotak;
    }
    stt->nkepingan=tmp;
    return 0;
}

int runtuh(char *a, int nx, int ny){
int x, y, nsama;
int i=0;
int j=0;
int nilai=0;
    for(y=0;y<ny;y++){
    nsama=0;
        for(x=0;x<nx;x++){
            if(a[i]!=0){
                nsama++;
            }
            i++;
        }
        if(nsama==stat.jkolom){
            for(j=i-1;j>=stat.jkolom;j--){
                a[j]=a[j-nkolom];
            }
            nilai++;
        }
    }
    return nilai;
}

int putar(char *t, char *s){
char x, y;
int i=0;
int n=0, nmax=4;
    switch(strlen(s)){
        case 4:
            nmax=2;
            break;
        case 9:
            nmax=3;
            break;
        case 16:
            nmax=4;
            break;
        default:
            nmax=1;
            break;
    }

    n=0;
    t[0]=0;
    for(y=0;y<nmax;y++){
        for(x=0;x<nmax;x++){
            i=((nmax-1-x)*nmax)+(y);
            t[n]=s[i];
            n++;
        }
    }
    t[n]=0;
    return 0;
}

int mulai_game(char *a){
int i=0;
    stat.ckepingan[0]=0;
    stat.nkepingan=galah;
    acak(&stat);
    for(i=0;i<200;i++){
        a[i]=0;
    }

    stat.x=nkolom/2-1;
    stat.y=0;
    stat.jkolom=nkolom;
    stat.jeda=0;
    stat.main=1;
    stat.level=1;
    stat.kecepatan=1;
    stat.skor=0;
    stat.tombol=0;

    strcpy(stat.ckepingan, stat.nkepingan);
    SDL_Delay(1);
    acak(&stat);
    return 0;
}

int gambarlatar(SDL_Renderer *latar, SDL_Texture *gambar, char *a, int nx, int ny){
int i=0;
int x, y;
SDL_Rect rct, rcd;
    i=batas_baris*nkolom;
    for(y=batas_baris;y<ny;y++){
        for(x=0;x<nx;x++){
            if(a[i]!=0){
                rcd.x=x*stat.cx;
                rcd.y=(y-batas_baris)*stat.cy;
                rcd.w=stat.cx;
                rcd.h=stat.cy;
                rct.x=a[i]*stat.cy;
                rct.y=0;
                rct.w=stat.cx;
                rct.h=stat.cy;

                SDL_RenderCopy(grenderer, gambar, &rct, &rcd);
            }
            i++;
        }
    }
    return 0;
}

int gambarkepingan(SDL_Renderer *latar, SDL_Texture *gambar, char *kepingan, int px, int py, int xgeser, int ygeser, int hilang){
SDL_Rect rct, rcd;
int i=0;
int tmp, x, y;
int n=1;
    switch(strlen(kepingan)){
        case 4:
            n=2;
            break;
        case 9:
            n=3;
            break;
        case 16:
            n=4;
            break;
        default:
            n=1;
            break;
    }

    for(y=0;y<n;y++){
        for(x=0;x<n;x++){
            if((kepingan[i]!='0') && (( y*nkolom+py)>=hilang)){
                tmp=kepingan[i]-'0';
                if(tmp!=0){
                    rcd.x=(px+x)*stat.cx+xgeser;
                    rcd.y=(py+y-batas_baris)*stat.cy+ygeser;
                    rcd.w=stat.cx;
                    rcd.h=stat.cy;
                    rct.x=tmp*stat.cy;
                    rct.y=0;
                    rct.w=stat.cx;
                    rct.h=stat.cy;

                    SDL_RenderCopy(grenderer, gambar, &rct, &rcd);
                }
            }
            i++;
        }
    }
    return 0;
}

int kosongkan(char *a, char nx, char ny){
int i=0;
int x, y, tx, ty;
    tx=nx-2;
    ty=ny-2;
    for(y=0;y<ny;y++){
        for(x=0;x<nx;x++){
            if((x==0) || (x>tx) || (y>ty))a[i]=4;
            else arr[i]=0;
            i++;
        }
    }
    return 0;
}

int cektabrakan(char *a, char *s, int xpos, int ypos){
int status=0;
int i, x, y;
int n=1;
    switch(strlen(s)){
        case 4:
            n=2;
            break;
        case 9:
            n=3;
            break;
        case 16:
            n=4;
            break;
        default:
            n=1;
            break;
    }

    for(y=0;y<n;y++){
        for(x=0;x<n;x++){
            i=((y+ypos)*stat.jkolom)+xpos+x;
            if(s[y*n+x]!='0' && a[i]!=0){
                status=1;
                break;
            }
        }
    }

    return status;
}

int gerak(SDL_Renderer *latar, SDL_Texture *gambar, char *tempat, char *kepingan, int kode){
int i, x, y, n, geser, nruntuh;
int tabrakan=1;
char stmp[20]="";
    switch(kode){
        case SDLK_LEFT:
        case SDLK_a:
            geser=stat.x-1;
            tabrakan=cektabrakan(tempat, kepingan, geser, stat.y);
            if(tabrakan==0){
                stat.x=geser;
            }
            break;
        case SDLK_RIGHT:
        case SDLK_d:
            geser=stat.x+1;
            tabrakan=cektabrakan(tempat, kepingan, geser, stat.y);
            if(tabrakan==0){
                stat.x=geser;
            }
            break;
        case SDLK_w:
        case SDLK_UP:
            putar(stmp, stat.ckepingan);
            tabrakan=cektabrakan(tempat, stmp, stat.x, stat.y);
            if(tabrakan==0){
                strcpy(stat.ckepingan, stmp);
            }
            break;
        case SDLK_DOWN:
        case SDLK_s:
            geser=stat.y+1;
            tabrakan=cektabrakan(tempat, kepingan, stat.x, geser);
            if(tabrakan==0){
                stat.y=geser;
                stat.jeda=0;
            }
            break;
    }

    stat.tombol=0;

    if(stat.main!=0 && kepingan!=0){
        if(stat.jeda==(7-stat.kecepatan)){
            n=4;
            switch(strlen(kepingan)){
                case 4:
                    n=2;
                    break;
                case 9:
                    n=3;
                    break;
                case 16:
                    n=4;
                    break;
                default:
                    n=1;
                    break;
            }
            if(stat.y<nbaris){
                stat.y++;
                if(cektabrakan(tempat, kepingan, stat.x, stat.y)!=0){
                    stat.cek=1;
                    stat.y--;
                }
            }else{
                stat.cek=1;
            }
            if(stat.cek!=0){
                for(y=0;y<n;y++){
                    for(x=0;x<n;x++){
                        i=((y+stat.y)*stat.jkolom)+stat.x+x;
                        if(stat.ckepingan[y*n+x]!='0')arr[i]=stat.ckepingan[y*n+x]-'0';
                    }
                }

                n=stat.jkolom-1;
                for(x=1;x<n;x++)
                    if(arr[batas_baris*nkolom+x]!=0){
                        stat.main=0;
                        break;
                    }

                nruntuh=runtuh(arr, nkolom, nbaris-1);
                stat.skor+=nruntuh;
                sprintf(ntulisan[pnilai], "%d", stat.skor);
                //efek suara
                if(nruntuh>0){
                    Mix_Pause(1);
                    Mix_PlayChannel(1, stat.suara_runtuh, 1);
                }else{
                    Mix_Pause(2);
                    Mix_PlayChannel(2, stat.suara_jatuh, 0);
                }
                //
                stat.kecepatan=(stat.skor/20)+1;
                sprintf(ntulisan[pkecepatan], "%d", stat.kecepatan);
                if(stat.kecepatan>5){
                    stat.main=0;
                    stat.level++;
                    sprintf(ntulisan[plevel], "%d", stat.level);

                    stat.skor=0;
                    stat.kecepatan=0;
                }
                gambarlatar(latar, gambar, arr, stat.jkolom, 20);
                if(stat.nkepingan!=0)strcpy(stat.ckepingan, stat.nkepingan);
                acak(&stat);
                stat.x=nkolom/2-1;
                stat.y=0;
                stat.cek=0;
            }
            stat.jeda=0;
        }else{
            stat.jeda++;
        }
    }
    return 0;
}

int main( int argc, char* args[] ){
	SDL_Window *window = NULL;
	SDL_Surface *gambar1=NULL;
	SDL_Surface *gambar2=NULL;
	SDL_Rect rcd;
	Uint32 wberikutnya;

	if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 && IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) <0){
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}else{
        mulai_game(arr);
        kosongkan(arr, nkolom, nbaris);

		window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

		if( window != NULL ){
            SDL_Event sevent;

			//Get window surface
			TTF_Init();
			Mix_Init(MIX_INIT_MP3);
			Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
			grenderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
			SDL_SetRenderDrawColor( grenderer, 0xFF, 0x00, 0xFF, 0xFF );
            SDL_RenderClear(grenderer);

            huruf=TTF_OpenFont("FreeSans.ttf", 24);
            stat.suara_jatuh=Mix_LoadWAV("efek_jatuh.wav");
            stat.suara_runtuh=Mix_LoadWAV("efek_runtuh.wav");
            stat.musik=Mix_LoadMUS("musik.mp3");
            gambar1=IMG_Load("box.jpg");

            if(huruf!=NULL){
                warna.r=0xFF;
                warna.g=0x00;
                warna.b=0x00;
                gambar2=TTF_RenderText_Blended(huruf, "Sampeyan kalah", warna);
                stat.wtulisan=gambar2->w;
                stat.htulisan=gambar2->h;
            }

            if(stat.musik!=NULL && gambar1!=NULL && gambar2!=NULL && stat.suara_jatuh!=NULL && stat.suara_runtuh!=NULL){
                Mix_VolumeMusic(5);
                Mix_VolumeChunk(stat.suara_jatuh, 5);
                Mix_VolumeChunk(stat.suara_runtuh, 5);
                Mix_PlayMusic(stat.musik, -1);
                stat.gambar=SDL_CreateTextureFromSurface(grenderer, gambar1);
                stat.cx=gambar1->w/9;
                stat.cy=gambar1->h;

                SDL_FreeSurface(gambar1);

                stat.kalah=SDL_CreateTextureFromSurface(grenderer, gambar2);
                SDL_FreeSurface(gambar2);

                gambarlatar(grenderer, stat.gambar, arr, nkolom, nbaris);
                wberikutnya=SDL_GetTicks()+(300/stat.level);
                while(sevent.type!=SDL_QUIT){
                    while(SDL_PollEvent(&sevent)){
                        utama(&sevent);
                    }

                    //Clear screen
                    SDL_SetRenderDrawColor( grenderer, 0xFF, 0xFF, 0x00, 0xFF );
                    SDL_RenderClear(grenderer);

                    gambarlatar(grenderer, stat.gambar, arr, nkolom, nbaris);
                    if(stat.ckepingan[0]!=0)gambarkepingan(grenderer, stat.gambar, stat.ckepingan, stat.x, stat.y, 0, 0, batas_baris);
                    if(stat.nkepingan!=0)gambarkepingan(grenderer, stat.gambar, stat.nkepingan, 0, 0, 340, 250, 0);

                    gantistatus(grenderer, &stat);

                    if(stat.main==0){
                        rcd.x=(stat.cx*nkolom/2)-(stat.wtulisan/2);
                        rcd.y=(stat.cy*nbaris/2)-(stat.htulisan/2);
                        rcd.w=stat.wtulisan;
                        rcd.h=stat.htulisan;
                        SDL_RenderCopy(grenderer, stat.kalah, 0, &rcd);
                    }

                    if(SDL_GetTicks()>wberikutnya){
                        gerak(grenderer, stat.gambar, arr, stat.ckepingan, stat.tombol);
                        wberikutnya=SDL_GetTicks()+100;
                    }
                    SDL_RenderPresent( grenderer );
                }

                SDL_DestroyTexture(stat.gambar);
                SDL_DestroyTexture(stat.kalah);

                Mix_PauseMusic();
                Mix_CloseAudio();
                Mix_FreeMusic(stat.musik);
                Mix_FreeChunk(stat.suara_jatuh);
                Mix_FreeChunk(stat.suara_runtuh);
                Mix_CloseAudio();
                IMG_Quit();
            }
            SDL_DestroyRenderer( grenderer );
            SDL_DestroyWindow( window );
		}
	}

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}

int utama(SDL_Event *evt){
    switch(evt->type){
        case SDL_MOUSEBUTTONUP:
            if(stat.main==0)mulai_game(arr);
            break;
        case SDL_KEYDOWN:
            if(stat.main==0 && evt->key.keysym.sym==SDLK_RETURN){
                mulai_game(arr);
            }else stat.tombol=evt->key.keysym.sym;
            break;
        default:
            break;
    }
    return 0;
}
