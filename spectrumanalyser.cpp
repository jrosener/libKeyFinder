/*************************************************************************

  Copyright 2011-2013 Ibrahim Sha'ath

  This file is part of LibKeyFinder.

  LibKeyFinder is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LibKeyFinder is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with LibKeyFinder.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************/

#include "spectrumanalyser.h"

namespace KeyFinder {

  SpectrumAnalyser::SpectrumAnalyser(unsigned int f, const Parameters& params, ChromaTransformFactory* spFactory) {
    octaves = params.getOctaves();
    bandsPerSemitone = params.getBandsPerSemitone();
    hopSize = params.getHopSize();
    ct = spFactory->getChromaTransform(f, params);
    unsigned int fftFrameSize = params.getFftFrameSize();
    WindowFunction win;
    temporalWindow = std::vector<float>(fftFrameSize);
    std::vector<float>::iterator twIt = temporalWindow.begin();
    for (unsigned int i = 0; i < fftFrameSize; i++) {
      *twIt = win.window(params.getTemporalWindow(), i, fftFrameSize);
      std::advance(twIt, 1);
    }
  }

  Chromagram SpectrumAnalyser::chromagram(
    AudioData& audio,
    FftAdapter* const fft
  ) const {
    if (audio.getChannels() != 1)
      throw Exception("Audio must be monophonic to be analysed");
    unsigned int sampleCount = audio.getSampleCount();
    unsigned int hops = ceil((float)sampleCount / (float)hopSize);
    Chromagram ch(hops, octaves, bandsPerSemitone);
    for (unsigned int hop = 0; hop < hops; hop++) {
      audio.resetIterators();
      audio.advanceReadIterator(hop * hopSize);
      std::vector<float>::const_iterator twIt = temporalWindow.begin();
      for (unsigned int sample = 0; sample < frmSize; sample++) {
        fft->setInput(sample, audio.getSampleAtReadIterator() * *twIt);
        audio.advanceReadIterator();
        std::advance(twIt, 1);
      }
      fft->execute();
      std::vector<float> cv = ct->chromaVector(fft);
      std::vector<float>::const_iterator cvIt = cv.begin();
      for (unsigned int band = 0; band < ch->getBands(); band++) {
        ch->setMagnitude(hop, band, *cvIt);
        std::advance(cvIt, 1);
      }
    }
    return ch;
  }

}
