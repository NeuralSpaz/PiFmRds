
/* by Josh Gardiner 2014 04 07

*/

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <unistd.h>

int gen_wave_table(int *wavetable,int sample_rate, int debug){

    double phase, delta_phase;

    int num_samples;
    num_samples= sample_rate;

    delta_phase = (M_PI * 2) / sample_rate ;
    phase = 0;

    int i;
    for (i=0; i<num_samples; i++ ){
      if (phase > 2 * M_PI)
          phase -= 2 * M_PI;
      wavetable[i] = sin(phase) * 16 * 1024;
      phase += delta_phase;
	  if (debug==1) {	  printf("Sample %d = %d \n", i, wavetable[i]);}
    }
    return(0);
}
snd_seq_t *open_seq(){
    snd_seq_t *seq_handle;

    if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
        fprintf(stderr, "Error opening ALSA sequencer.\n");
        exit(1);
    }
    snd_seq_set_client_name(seq_handle, "LinzerSchnitteMIDI");
    if (snd_seq_create_simple_port(seq_handle, "LinzerSchnitteMIDI",
        SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
        SND_SEQ_PORT_TYPE_APPLICATION) < 0) {
        fprintf(stderr, "Error creating sequencer port.\n");
        exit(1);
    }
    return (seq_handle);
}
int multicast_midi_setup(){
    return 0;
}
int midi_read(snd_seq_t *seq_handle, int *note_active){

    snd_seq_event_t *ev;
    int l1;
//    int note_active[512];
    double midichannel[128] = {0};
    int note[128] = {0};
    double velocity[128] = {0};
    int poly = 128;

    do {
        snd_seq_event_input(seq_handle, &ev);
        switch (ev->type) {
            case SND_SEQ_EVENT_NOTEON:
                for (l1 = 0; l1 < poly; l1++) {
                    if (!note_active[l1]) {
                        note[l1] = ev->data.note.note;
                        midichannel[l1] = ev->data.note.channel;
                        velocity[l1] = ev->data.note.velocity;
                        velocity[l1] = velocity[l1] / 127;
                        printf("CH %2.0f ", midichannel[l1]+1);
                        printf("Note %3d ON  ", note[l1]);
                        printf("Vel %3.0f \n", velocity[l1]*127);
                        note_active[l1] = 1;
                        break;
                    }
                }
                break;
            case SND_SEQ_EVENT_NOTEOFF:
                for (l1 = 0; l1 < poly; l1++) {
                    if (note_active[l1]) {
                        note[l1] = ev->data.note.note;
                        midichannel[l1] = ev->data.note.channel;
                        velocity[l1] = ev->data.note.velocity;
                        velocity[l1] = velocity[l1] / 127;
                        printf("CH %2.0f ", midichannel[l1]+1);
                        printf("Note %3d OFF ", note[l1]);
                        printf("Vel %3.0f \n", velocity[l1]*127);
                        note_active[l1] = 0;
                    }
                }
                break;
        }
        snd_seq_free_event(ev);
    } while (snd_seq_event_input_pending(seq_handle, 0) > 0);
    return (0);
}
int playback_callback(){
    return 0;
}
int envelope(){
    return 0;
}
int connectkeyboard(snd_seq_t *seq_handle){
        snd_seq_addr_t sender, dest;
        snd_seq_port_subscribe_t *subs;
        sender.client = 14;
        sender.port = 0;
        dest.client = 128;
        dest.port = 0;
        snd_seq_port_subscribe_alloca(&subs);
        snd_seq_port_subscribe_set_sender(subs, &sender);
        snd_seq_port_subscribe_set_dest(subs, &dest);
        snd_seq_port_subscribe_set_queue(subs, 1);
        snd_seq_port_subscribe_set_time_update(subs, 1);
        snd_seq_port_subscribe_set_time_real(subs, 1);
        snd_seq_subscribe_port(seq_handle, subs);
    return (0);
}
