// gcc -O2 -pthread -o mscplayer.out mscplayer.c -lmpg123 -lao
#include <ao/ao.h>
#include <mpg123.h>

#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <fcntl.h>
#include <dirent.h>
#include <termios.h>

#define BITS 8

int status;
int st_pause = 0;
int st_stop = 0;

pthread_t tid[9999];
// static char *dirpath = "/home/irshadrasyidi/Documents/SISOP/FP/musicplayer";
static char *dirpath = "/home/irshadrasyidi/Documents/SISOP/FP/musicplayer";

char inp;
char indexlagu[100][250];
char nama_lagu[250];
char rd2ple[250];
int num;

int kbhit()
{
    struct termios oldt, newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    
    ch = getchar();
    
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
}

int kbhit2(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

// void listDir(char* path){
//     DIR* dir;
//     struct dirent *ent;
//     if((dir=opendir(path)) != NULL){
//         while (( ent = readdir(dir)) != NULL){
//             if(ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0  && strcmp(ent->d_name, "..") != 0){	
//                 printf("%s\n", ent->d_name);
//                 listDir(ent->d_name);
//             }
//             else{
//                 if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")){
//                     printf("%s\n", ent->d_name);
//                 }
//             }
//         }
//         closedir(dir);
//     }
// }

void list(char* path){
    num = 0;
    DIR* dir;
    struct dirent *ent;
    if((dir=opendir(path)) != NULL){
        while (( ent = readdir(dir)) != NULL){
            printf("%d. %s\n", num + 1, indexlagu[num]);
            num++;
        }
        closedir(dir);
    }
}

void nomerin(char* path){
    num = 0;
    DIR* dir;
    struct dirent *ent;
    if((dir=opendir(path)) != NULL){
        while (( ent = readdir(dir)) != NULL){
            
            strcat(indexlagu[num], ent->d_name);
            // printf("%d. %s\n", num + 1, indexlagu[num]);

            num++;
        }
        closedir(dir);
    }
}

void* play()
{
    while(1){
        while(wait(status) > 0){

        }

        if(strlen(rd2ple) > 0){
            mpg123_handle *mh;
            unsigned char *buffer;
            size_t buffer_size;
            size_t done;
            int err;

            int driver;
            ao_device *dev;

            ao_sample_format format;
            int channels, encoding;
            long rate;

            /* initializations */
            ao_initialize();
            driver = ao_default_driver_id();
            mpg123_init();
            mh = mpg123_new(NULL, &err);
            buffer_size = mpg123_outblock(mh);
            buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

            /* open the file and get the decoding format */
            mpg123_open(mh, rd2ple);
            mpg123_getformat(mh, &rate, &channels, &encoding);

            /* set the output format and open the output device */
            format.bits = mpg123_encsize(encoding) * BITS;
            format.rate = rate;
            format.channels = channels;
            format.byte_format = AO_FMT_NATIVE;
            format.matrix = 0;
            dev = ao_open_live(driver, &format, NULL);

            /* decode and play */
            while(1){
                if(st_pause == 0){
                    if(mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK){
                        ao_play(dev, buffer, done);
                        printf("ngeple~ %d\n", st_pause);
                    }
                    else{ 
                        break;
                    }
                }
                else if(st_pause == 1){
                    printf("ngepos!! %d\n", st_pause);
                }
                else if(st_stop == 1){
                    st_pause = 0;
                    st_stop = 0;
                    break;
                }
            }

            /* clean up */
            free(buffer);
            ao_close(dev);
            mpg123_close(mh);
            mpg123_delete(mh);
            mpg123_exit();
            ao_shutdown();
        }

        
    }

    
    
}

int main()
{
    // play("/home/irshadrasyidi/Downloads/indoraya.mp3");
    // listDir(dirpath);

    st_pause = 0;
    pthread_create(&(tid[0]), NULL, play, NULL);

    nomerin(dirpath);

    while(1){
        // system("clear");
        // strcat(rd2ple, dirpath);

        printf("menu: (input nomer)\n");
        printf("1. Play/Stop\n");
        printf("2. Pause/Resume\n");
        printf("3. Next\n");
        printf("4. Prev\n");
        printf("5. List Lagu\n");
        inp = kbhit();

        if(inp == '1' && st_pause == 0 && st_stop == 0){
            st_pause = 0;
            st_stop = 0;
            memset(&rd2ple, 0, sizeof(rd2ple));
            printf("Judulnya (pake .mp3) :\n");
            scanf("%s", nama_lagu);
            
            strcat(rd2ple, dirpath);
            strcat(rd2ple, "/");
            strcat(rd2ple, nama_lagu);

        }
        else if(inp == '2'){
            // st_pause = 1;
            if(st_pause == 0){
                st_stop = 0;
                st_pause = 1;
            }
            else if(st_pause == 1){
                st_stop = 0;
                st_pause = 0;
            }
            
        }
        else if(inp == '3'){
            st_pause = 0;
            st_stop = 1;
        }
        else if(inp == '4'){
            st_pause = 0;
            st_stop = 1;
        }
        else if(inp == '5'){
            list(dirpath);
        }

    }

    return 0;
}
