#pragma once
#include "../list.h"

#include "../../audio/audio.h"

template class Array<Audio::BasicAudioSource*>;
template class List<Audio::BasicAudioSource*>;

template class Array<Audio::BasicAudioDestination*>;
template class List<Audio::BasicAudioDestination*>;

template class Array<Audio::AudioInputDevice*>;
template class List<Audio::AudioInputDevice*>;

template class Array<Audio::AudioOutputDevice*>;
template class List<Audio::AudioOutputDevice*>;