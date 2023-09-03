#pragma once

#include <string>
#include <algorithm>
#include <stdexcept>
#include "IPlugConstants.h"
#include "IPlugLogger.h"

#include "SA_Math.h"

#define DR_WAV_IMPLEMENTATION 
#include "dr_wav.h"

namespace SA {
  using namespace iplug;
  class FileLoader
  {
  public:
    FileLoader()
    {

      buffer = nullptr;
      nchannels = 0;
      buffersize = 0;
      sampleRate = 0;
      verbose = false;
      slength = 0;

    }

    ~FileLoader()
    {
      if (buffer != nullptr) {
        unload();
      }
    }

    /*!
  @brief loads the audio data from a file
  @param[in] filePath absolute or relative path to audio file
  */
    void    load(std::string path) {
      std::string extension = path.substr(path.find_last_of(".") + 1);
      std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

      if (extension == "wav") {
        load_wav(path);
      }
      else if (extension == "flac") {
        load_flac(path);
      }
      else if (extension == "mp3") {
        load_mp3(path);
      }
      /*else if (extension == "ogg") {
        load_ogg(path);
      }*/
      else {
        throw std::logic_error("ERROR -> unknown file format");
      }
    }

    /*!
    @brief unload the data from the buffer and free the allocated memory
    */
    void    unload() {
      if (buffer != nullptr) delete buffer;
      filePath = "file not loaded";
      buffer = nullptr;
      nchannels = 0;
      buffersize = 0;
      sampleRate = 11050;
    }

    /*!
    @brief returns true if data has been loaded into the ofxAudioFile
    */
    bool    loaded() const {
      if (buffer != nullptr) {
        return true;
      }
      else {
        return false;
      }
    };

    /*!
    @brief activate logging of file loading operations for this FileLoader
    @param[in] verbose
    */
    void    setVerbose(bool verbose) {
      this->verbose = verbose;
    };

    /*!
    @brief returns the path of loaded file, or contains diagnostic strings if a file is not loaded
    */
    std::string path() const {
      return filePath;
    }

    /*!
    @brief returns a pointer to the buffer, use with caution! the internal buffer is initializated to nullptr
    */
    float* data() {
      return buffer;
    }

    /*!
    @brief loads the audio data from a file
    @param[in] n position of the sample to load, must be less than length()
    @param[in] channel channel of the sample to load
    */
    float sample(int n, int channel) const {
      return buffer[n * nchannels + channel];
    }

    /*!
    @brief returns the sample rate of the sample
    */
    uint64_t samplerate() const {
      return sampleRate;
    }

    /*!
    @brief returns the number of channels
    */
    unsigned int channels() const {
      return nchannels;
    }

    /*!
    @brief returns the length of each sample channel
    */
    uint64_t length() const {
      return slength;
    }
    uint8_t channels()  {
      return nchannels;
    }
  private:
    void load_wav(std::string path) {
      unsigned int channels;
      unsigned int sampleRate;
      drwav_uint64 totalFrameCount;
      size_t totalSampleCount;

      float* pSampleData = drwav_open_file_and_read_pcm_frames_f32(path.c_str(), &channels, &sampleRate, &totalFrameCount, NULL);

      if (pSampleData == NULL) {
        DBGMSG("[ERROR] ERROR loading wav file");
      }
      else {
        if (buffer != nullptr) { delete buffer; }
        this->nchannels = channels;
        totalSampleCount = (size_t)(totalFrameCount * this->nchannels);
        buffer = new float[totalSampleCount];
        std::memcpy(buffer, pSampleData, totalSampleCount * sizeof(float));
        drwav_free(pSampleData, NULL);
        this->sampleRate = sampleRate;
        this->buffersize = totalSampleCount;
        this->slength = this->buffersize / this->nchannels;
        this->filePath = path;
        if (verbose) {
          std::string s = "[INFO] loading " + this->filePath
            + " | sample rate : " + std::to_string(this->sampleRate)
            + " | channels : " + std::to_string(this->nchannels)
            + " | length : " + std::to_string(this->slength) + "\n";
          DBGMSG(s.c_str());

        }
      }
    }
    void load_ogg(std::string path) {};
    void load_mp3(std::string path) {};
    void load_flac(std::string path) {};

    float* buffer;

    uint8_t         nchannels;
    uint64_t        buffersize;
    uint64_t        slength;
    unsigned int    sampleRate;
    std::string     filePath;
    bool            verbose;
  };
  class FilePlayer
  {
  public:
    FilePlayer()
    {
      mLoader.setVerbose(true);
    }

    ~FilePlayer()
    {
    }
    void loadFile(const std::string &path) { mLoader.load(path); }
    void seekPos(uint64_t position) {
      mPosition = SA::math::clamp(position, 0, mLoader.length());
    }
    void process(sample** buffer, const int &nFrames) {
      if (!mLoader.loaded())
      {
        return;
      }
      uint64_t currentPos = mPosition;
      for (int s = 0; s < nFrames; s++) {
        for (int c = 0; c < mLoader.channels(); c++) {
          currentPos = std::min(mLoader.length(), mPosition + s);
          if (loop_ && currentPos == mLoader.length()) {
            currentPos = 0;
          }
          buffer[c][s] = mLoader.sample(currentPos, c);
        }
      }
      mPosition = currentPos;
    }
    float process(const uint8_t &channel) {
      if (!mLoader.loaded()) {
        return 0;
      }
      
      return mLoader.sample(mPosition , std::min(channel, mLoader.channels()));
    }
    float* getData() {
      return mLoader.data();
    }
    float getPositionFract() {
      return (float)mPosition / (float)mLoader.length();
    }
    void nextFrame() {
      mPosition = std::min(mLoader.length(), mPosition++);
    }
    bool isLooping() const { return loop_; }
    void setLoop(const bool &val) { loop_ = val; }
    uint8_t channels() {
      if (mLoader.loaded()) {
        return mLoader.channels();
      }
      else
      {
        return 0;
      }
    }
    uint64_t length() {
      if (mLoader.loaded()) {
        return mLoader.length();
      }
      else
      {
        return 0;
      }
    }

    bool isLoaded() {
      return mLoader.loaded();
    }
  private:
    FileLoader mLoader;
    uint64_t   mPosition = 0;
    bool loop_ = false;
  };

}