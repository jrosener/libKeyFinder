/*************************************************************************

  Copyright 2011-2014 Ibrahim Sha'ath

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

#include "keyfinder.h"

namespace KeyFinder {

  key_t KeyFinder::keyOfAudio(const AudioData& originalAudio) {

    Workspace workspace;
    progressiveChromagram(originalAudio, workspace);
    finalChromagram(workspace);

    return keyOfChromaVector(workspace.chromagram->collapseToOneHop(), std::vector<double>(BANDS, 0.0), std::vector<double>(BANDS, 0.0));
  }

  void KeyFinder::progressiveChromagram(
    AudioData audio,
    Workspace& workspace
  ) {
    preprocess(audio, workspace);
    workspace.preprocessedBuffer.append(audio);
    chromagramOfBufferedAudio(workspace);
  }

  void KeyFinder::finalChromagram(Workspace& workspace) {
    // flush remainder buffer
    if (workspace.remainderBuffer.getSampleCount() > 0) {
      AudioData flush;
      preprocess(flush, workspace, true);
    }
    // zero padding
    unsigned int paddedHopCount = ceil(workspace.preprocessedBuffer.getSampleCount() / (double)HOPSIZE);
    unsigned int finalSampleLength = FFTFRAMESIZE + ((paddedHopCount - 1) * HOPSIZE);
    workspace.preprocessedBuffer.addToSampleCount(finalSampleLength - workspace.preprocessedBuffer.getSampleCount());
    chromagramOfBufferedAudio(workspace);
  }

  void KeyFinder::preprocess(
    AudioData& workingAudio,
    Workspace& workspace,
    bool flushRemainderBuffer
  ) {
    workingAudio.reduceToMono();

    if (workspace.remainderBuffer.getChannels() > 0) {
      workingAudio.prepend(workspace.remainderBuffer);
      workspace.remainderBuffer.discardFramesFromFront(workspace.remainderBuffer.getFrameCount());
    }

    // TODO: there is presumably some good maths to determine filter frequencies.
    // For now, this approximates original experiment values for default params.
    double lpfCutoff = getLastFrequency() * 1.012;
    double dsCutoff = getLastFrequency() * 1.10;
    unsigned int downsampleFactor = (int) floor(workingAudio.getFrameRate() / 2 / dsCutoff);

    if (!flushRemainderBuffer && workingAudio.getSampleCount() % downsampleFactor != 0) {
      AudioData* remainder = workingAudio.sliceSamplesFromBack(workingAudio.getSampleCount() % downsampleFactor);
      workspace.remainderBuffer.append(*remainder);
      delete remainder;
    }

    // get filter
    const LowPassFilter* lpf = lpfFactory.getLowPassFilter(160, workingAudio.getFrameRate(), lpfCutoff, 2048);
    lpf->filter(workingAudio, workspace, downsampleFactor); // downsampleFactor shortcut
    // note we don't delete the LPF; it's stored in the factory for reuse

    workingAudio.downsample(downsampleFactor);
  }

  void KeyFinder::chromagramOfBufferedAudio(Workspace& workspace) {
    if (workspace.fftAdapter == NULL) {
      workspace.fftAdapter = new FftAdapter(FFTFRAMESIZE);
    }
    SpectrumAnalyser sa(workspace.preprocessedBuffer.getFrameRate(), &ctFactory, &twFactory);
    Chromagram* c = sa.chromagramOfWholeFrames(workspace.preprocessedBuffer, workspace.fftAdapter);
    workspace.preprocessedBuffer.discardFramesFromFront(HOPSIZE * c->getHops());
    if (workspace.chromagram == NULL) {
      workspace.chromagram = c;
    } else {
      workspace.chromagram->append(*c);
      delete c;
    }
  }

  key_t KeyFinder::keyOfChromaVector(
    const std::vector<double>& chromaVector,
    const std::vector<double>& majorProfile,
    const std::vector<double>& minorProfile
  ) const {

    // get key estimate
    KeyClassifier classifier(majorProfile, minorProfile);
    return classifier.classify(chromaVector);
  }

}
