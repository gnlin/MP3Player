#ifndef __MP3PLAYER_H
#define __MP3PLAYER_H

#include "stm32f10x.h"

#define MUSIC_DIR     "/sys/music"

void play_music(TCHAR *music_name);
BOOLEAN check_suffix(u8 *str, u8 *suffix, u8 suffixlen);
INT8U Page_Music_Player(TCHAR *music_name);
INT8U Page_Music_List(TCHAR *music_name);

#endif /* __MP3PLAYER_H */
