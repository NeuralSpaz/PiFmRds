
/* by Josh Gardiner 2014 04 07

*/

extern int gen_wave_table(int *wavetable,int sample_rate, int debug);
extern snd_seq_t *open_seq();
extern int multicast_midi_setup();
extern int midi_read(snd_seq_t *seq_handle, int *note_active);
extern int playback_callback();
extern int envelope();
extern int connectkeyboard(snd_seq_t *seq_handle);
