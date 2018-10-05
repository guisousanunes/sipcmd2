#ifndef CS_MANAGER_H
#define CS_MANAGER_H

#include "includes.h"

class Manager;

class LocalEndPoint : public OpalLocalEndPoint
{

        PCLASSINFO(LocalEndPoint, OpalLocalEndPoint)

      public:
        LocalEndPoint(Manager &);
        ~LocalEndPoint() {}

        virtual PSafePtr<OpalConnection> MakeConnection(
            OpalCall &call,
            const PString &party,
            void *userData = NULL,
            unsigned int options = 0,
            OpalConnection::StringOptions *opts = NULL);

        virtual OpalLocalConnection *CreateConnection(
            OpalCall &call,
            void *userData,
            unsigned options,
            OpalConnection::StringOptions *strOptions);

        // get media data for transmission
        virtual bool OnReadMediaData(
            const OpalLocalConnection &connection,
            const OpalMediaStream &mediaStream,
            void *data,
            PINDEX size,
            PINDEX &length);

        // receive media data
        virtual bool OnWriteMediaData(
            const OpalLocalConnection &connection,
            const OpalMediaStream &mediaStream,
            const void *data,
            PINDEX length,
            PINDEX &written);

      private:
        Manager &m_manager;
        std::string gatekeeper;
};

class RTPSession : public RTP_UDP
{
        PCLASSINFO(RTPSession, RTP_UDP);

      public:
        enum Payload
        {
                PCM16,
                G711_ALAW,
                G711_ULAW
        };

        RTPSession(const Params &options);

        virtual SendReceiveStatus OnReceiveData(
            RTP_DataFrame &frame);

        virtual SendReceiveStatus OnSendData(
            RTP_DataFrame &frame);

        virtual SendReceiveStatus OnReadTimeout(
            RTP_DataFrame &frame);

        void SelectAudioFormat(const Payload p);
        OpalAudioFormat &GetAudioFormat() const { return *m_audioformat; }

      private:
        OpalAudioFormat *m_audioformat;
        // xxx
};

class RTPUserData : public RTP_UserData
{
      public:
        RTPUserData() : RTP_UserData() {}

        virtual void OnTxStatistics(const RTP_Session &session);
};

class Manager : public OpalManager
{
        PCLASSINFO(Manager, OpalManager);

      public:
        Manager();
        ~Manager();

        bool Init(PArgList &args);
        void Main(PArgList &args);
        bool StartListener();
        bool MakeCall(const PString &remoteParty);
        bool SendDTMF(const PString &dtmf);
        void SetListenMode(const bool m)
        {
                listenmode = m;
        }
        bool IsListenerUp() { return listenerup; }

        bool RegisterSIP();

        // Media streams management
        virtual bool OnOpenMediaStream(
            OpalConnection &connection,
            OpalMediaStream &stream);

        virtual void OnClosedMediaStream(
            const OpalMediaStream &stream);

        // Connection Management
        virtual bool OnIncomingConnection(
            OpalConnection &connection,
            unsigned opts,
            OpalConnection::StringOptions *stropts);

        virtual void OnEstablished(
            OpalConnection &connection);

        virtual void OnReleased(
            OpalConnection &connection);

        // Call Management
        virtual void OnEstablishedCall(
            OpalCall &call);

        virtual void OnClearedCall(
            OpalCall &call);

        virtual OpalConnection::AnswerCallResponse OnAnswerCall(
            OpalConnection &connection,
            const PString &caller);

        bool WriteFrame(RTP_DataFrame &f);
        bool ReadFrame(RTP_DataFrame &f);

        unsigned CalculateTimestamp(size_t sz);

      private:
        LocalEndPoint *localep;
        SIPEndPoint *sipep;
        H323EndPoint *h323ep;
        RTPSession *m_rtpsession;

        std::string currentCallToken;
        std::string inputfile;
        std::string outputfile;

        PString login_sip;
        PString password_sip;
        PString servername_sip;

        SIPRegister::Params param;

        bool listenmode;
        bool listenerup;
        bool pauseBeforeDialing;

        Manager(const Manager &);
        Manager operator=(Manager &);
};

class TestProcess : public PProcess
{
        PCLASSINFO(TestProcess, PProcess);

      public:
        TestProcess();
        void Main();

      protected:
        Manager *manager;
};

#endif
