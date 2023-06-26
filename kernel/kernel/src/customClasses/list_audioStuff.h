
#pragma once
#include "list.h"

#include "../audio/audio.h"

template class Array<Audio::BasicAudioSource*>;
template class List<Audio::BasicAudioSource*>;

template class Array<Audio::BasicAudioDestination*>;
template class List<Audio::BasicAudioDestination*>;

