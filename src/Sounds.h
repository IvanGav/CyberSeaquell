#pragma once
#include "DrillLib.h"
#include "CyberSeaquell_decl.h"
#include "stb_vorbis.h"

namespace Sounds {

struct AudioSource {
	F32* data;
	U32 sampleCount;
	U32 sampleRate;
	F64 length;
};

struct AudioInstance {
	AudioSource* src;
	F64 startTime;
};

ArenaArrayList<AudioInstance> instances;

void load_source(AudioSource* srcOut, const char* path) {
	int channels, sampleRate;
	short* output;
	int dataLength = stb_vorbis_decode_filename(path, &channels, &sampleRate, &output);
	if (dataLength > 0) {
		F32* data = globalArena.alloc<F32>(dataLength);
		for (U32 i = 0; i < dataLength; i++) {
			data[i] = F32(output[i * channels]) / 65535.0F;
		}
		free(output);
		*srcOut = AudioSource{ data, U32(dataLength), U32(sampleRate), F32(dataLength) / F32(sampleRate)};
	} else {
		*srcOut = AudioSource{};
	}
}

AudioSource bees;
AudioSource clickPen;
AudioSource seagulls;
AudioSource bg;

void load_sources() {
	load_source(&bees, "./resources/sounds/bees-swarming.ogg");
	load_source(&clickPen, "./resources/sounds/click-pen.ogg");
	load_source(&seagulls, "./resources/sounds/seagulls.ogg");
	load_source(&bg, "./resources/sounds/bg.ogg");

}

void play_sound(AudioSource& src) {
	instances.push_back(AudioInstance{ &src, CyberSeaquell::audioPlaybackTime });
}

void mix_into_buffer(F32* buffer, U32 numSamples, U32 numChannels, F32 timeAmount) {
	U32 toRemove = 0;
	memset(buffer, 0, numSamples * numChannels * sizeof(F32));
	for (U32 i = 0; i < instances.size; i++) {
		AudioInstance& inst = instances.data[i];
		if (CyberSeaquell::audioPlaybackTime >= inst.startTime + inst.src->length) {
			toRemove = i + 1;
		} else {
			F32* buf = buffer;
			for (U32 j = 0; j < numSamples; j++) {
				F64 dt = F64(j) / F64(numSamples) * F64(timeAmount);
				F64 t = (CyberSeaquell::audioPlaybackTime + dt - inst.startTime) * F64(inst.src->sampleRate);
				F32 val = inst.src->data[U32(t)];
				for (U32 k = 0; k < numChannels; k++) {
					*buf++ += val;
				}
			}
		}
	}
	if (toRemove) {
		memcpy(instances.data, instances.data + toRemove, (instances.size - toRemove) * sizeof(AudioInstance));
		instances.resize(instances.size - toRemove);
	}
	
}

}