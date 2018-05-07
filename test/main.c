#include <stdio.h>
#include <stdlib.h>
#include <vlc/vlc.h>

int main(int argc, char **argv){
    printf("print me-3");

    libvlc_instance_t *inst;
    libvlc_media_player_t *mp;
    libvlc_media_t *m;

    printf("print me-2");
    // load the vlc engine
    inst = libvlc_new(0, NULL);
    printf("print me-1");
    // create a new item
    //m = libvlc_media_new_path(inst, "root/home/debian/sound/walle3.mp3");
    m = libvlc_media_new_path(inst, "walle3.mp3");
    printf("print me0");

    // create a media play playing environment
    mp = libvlc_media_player_new_from_media(m);
    printf("print me1");

    // no need to keep the media now
    libvlc_media_release(m);
    printf("print me2");

    // play the media_player
    libvlc_media_player_play(mp);
    printf("print me3");

    sleep(100);//play the audio 100s

    // stop playing
    libvlc_media_player_stop(mp);
    printf("print me4");

    // free the media_player
    libvlc_media_player_release(mp);

    printf("print me5");

    libvlc_release(inst);


    return 0;
}
