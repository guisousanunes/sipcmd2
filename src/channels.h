#ifndef CS_CHANNELS_H
#define CS_CHANNELS_H

#include <ptlib/syncpoint.h>
#include <ptclib/delaychan.h>
#include "includes.h"

class AutoSync
{
  private:
    PSemaphore &sync;

  public:
    AutoSync(PSemaphore &s) : sync(s) { sync.Wait(); }
    ~AutoSync() { sync.Signal(); }
};

class TestChanAudio
{
  public:
    TestChanAudio() : playback(false), record(false),
                      stop_recording_when_silent(false), recordmillisec(0U),
                      playfile(NULL), recfile(NULL), playsync(), recsync(),
                      sync(1U, 1U)
    {
        std::cout << __func__ << std::endl;
    }

    ~TestChanAudio()
    {
        std::cout << __func__ << std::endl;
        AutoSync a(sync);
        StopAudioPlayback();
        StopAudioRecording();
    }

    // playback
    bool PlaybackAudioBuffer(PBYTEArray &buffer);
    bool PlaybackAudioFile(PString &filename);
    void FillPlaybackBuffer(char *buf, size_t len);
    void StopPlayback(bool ioerror)
    {
        AutoSync a(sync);
        StopAudioPlayback(ioerror);
    }

    // record
    bool RecordAudioFile(PString &filename, bool append_file,
                         bool stop_on_silence, int max_millis);

    void RecordFromBuffer(
        const char *buf, size_t len, bool currently_silent);

    void StopRecording(bool ioerror)
    {
        AutoSync a(sync);
        StopAudioRecording(ioerror);
    }

    // other
    void CloseChannel()
    {
        cout << "TestChanAudio::CloseChannel" << endl;
        AutoSync a(sync);
        StopAudioPlayback();
        StopAudioRecording();
    }

  private:
    volatile bool playback;
    volatile bool record;
    volatile bool stop_recording_when_silent;
    size_t recordmillisec;
    PFile *playfile;
    PFile *recfile;
    PSyncPoint playsync;
    PSyncPoint recsync;
    PSemaphore sync;

    bool PlaybackAudio(bool raw_rtp);
    void StopAudioPlayback(bool ioerror = false);
    void StopAudioRecording(bool ioerror = false);
};

class TestChannel : public PIndirectChannel
{
    PCLASSINFO(TestChannel, PIndirectChannel)

  public:
    TestChannel(OpalConnection &conn, TestChanAudio &chan) : connection(conn), audiohandle(chan), is_open(false),
                                                             readDelay(), writeDelay()
    {
        std::cout << __func__ << "[ " << this->connection << " - " << this << " ]" << std::endl;
    }

    ~TestChannel()
    {
        std::cout << __func__ << "[ " << this->connection
                  << " - " << this << " ]" << std::endl;
        Close();
    }

    virtual bool Close();
    virtual bool IsOpen() const;
    virtual bool Read(void *, PINDEX);
    virtual bool Write(const void *, PINDEX);

  private:
    OpalConnection &connection;
    TestChanAudio &audiohandle;
    bool is_open;
    PAdaptiveDelay readDelay;
    PAdaptiveDelay writeDelay;
};

class RawMediaStream : public OpalRawMediaStream
{
    PCLASSINFO(RawMediaStream, OpalRawMediaStream);

  public:
    RawMediaStream(
        OpalConnection &connection,
        const OpalMediaFormat &mediaFormat,
        unsigned sessionID,
        bool isSource,
        PChannel *channel,
        bool autoDelete)
        : OpalRawMediaStream(
              connection, mediaFormat, sessionID,
              isSource, channel, autoDelete) {}

    virtual bool IsSynchronous() const { return true; }

    virtual bool ReadData(
        BYTE *data, PINDEX size, PINDEX &length);

    virtual bool WriteData(
        const BYTE *data, PINDEX length, PINDEX &written);
};

class LocalEndPoint;
class LocalConnection : public OpalLocalConnection
{
    PCLASSINFO(LocalConnection, OpalLocalConnection);

  public:
    LocalConnection(
        OpalCall &call,
        LocalEndPoint &ep,
        void *userData,
        unsigned opts,
        OpalConnection::StringOptions *stropts);

    ~LocalConnection();

    virtual OpalMediaStream *CreateMediaStream(
        const OpalMediaFormat &mediaFormat,
        unsigned sessionID,
        bool isSource);
};

#endif
