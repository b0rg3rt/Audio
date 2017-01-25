/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "mixer_float.h"
#include "arm_math.h"

#ifdef AUDIO_FLOAT


static void applyGain(float *data, float mult) {
	for (int i = 0; i < AUDIO_BLOCK_SAMPLES/2; i++) {
		data[i] = data[i] * mult;
	}
}

static void applyGainThenAdd(float *data, const float *in, float mult) {
	for (int i = 0; i < AUDIO_BLOCK_SAMPLES/2; i++) {
		data[i] = data[i] + in[i] * mult;
	}
}

void AudioMixerFloat4::update(void)
{
	audio_block_t *in, *out=NULL;
	unsigned int channel;

	for (channel=0; channel < 4; channel++) {
		if (!out) {
			out = receiveWritableFloat(channel);
			if (out) {
				float mult = multiplier[channel];
				applyGain((float *)out->data, mult);
				applyGain((float *)out->nextBlock->data, mult);
			}
		} else {
			in = receiveReadOnlyFloat(channel);
			if (in) {
				applyGainThenAdd((float *)out->data, (float *)in->data, multiplier[channel]);
				applyGainThenAdd((float *)out->nextBlock->data, (float *)in->nextBlock->data, multiplier[channel]);
				release(in);
			}
		}
	}
	if (out) {
		transmit(out);
		release(out);
	}
}

#endif

