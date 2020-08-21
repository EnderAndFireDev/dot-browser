/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set sw=2 ts=8 et tw=80 : */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_net_HttpBackgroundChannelChild_h
#define mozilla_net_HttpBackgroundChannelChild_h

#include "mozilla/net/PHttpBackgroundChannelChild.h"
#include "nsIRunnable.h"
#include "nsTArray.h"

using mozilla::ipc::IPCResult;

namespace mozilla {
namespace net {

class BackgroundDataBridgeChild;
class HttpChannelChild;

class HttpBackgroundChannelChild final : public PHttpBackgroundChannelChild {
  friend class BackgroundChannelCreateCallback;
  friend class PHttpBackgroundChannelChild;
  friend class HttpChannelChild;
  friend class BackgroundDataBridgeChild;

 public:
  explicit HttpBackgroundChannelChild();

  NS_INLINE_DECL_THREADSAFE_REFCOUNTING(HttpBackgroundChannelChild, final)

  // Associate this background channel with a HttpChannelChild and
  // initiate the createion of the PBackground IPC channel.
  nsresult Init(HttpChannelChild* aChannelChild);

  // Callback while the associated HttpChannelChild is not going to
  // handle any incoming messages over background channel.
  void OnChannelClosed();

  // Return true if OnChannelClosed has been called.
  bool ChannelClosed();

  // Callback when OnStartRequest is received and handled by HttpChannelChild.
  // Enqueued messages in background channel will be flushed.
  void OnStartRequestReceived(Maybe<uint32_t> aMultiPartID);

#ifdef MOZ_DIAGNOSTIC_ASSERT_ENABLED
  bool IsQueueEmpty() const { return mQueuedRunnables.IsEmpty(); }
#endif

 protected:
  IPCResult RecvOnStartRequest(const nsHttpResponseHead& aResponseHead,
                               const bool& aUseResponseHead,
                               const nsHttpHeaderArray& aRequestHeaders,
                               const HttpChannelOnStartRequestArgs& aArgs);

  IPCResult RecvOnTransportAndData(const nsresult& aChannelStatus,
                                   const nsresult& aTransportStatus,
                                   const uint64_t& aOffset,
                                   const uint32_t& aCount,
                                   const nsCString& aData,
                                   const bool& aDataFromSocketProcess);

  IPCResult RecvOnStopRequest(
      const nsresult& aChannelStatus, const ResourceTimingStructArgs& aTiming,
      const TimeStamp& aLastActiveTabOptHit,
      const nsHttpHeaderArray& aResponseTrailers,
      const nsTArray<ConsoleReportCollected>& aConsoleReports);

  IPCResult RecvOnAfterLastPart(const nsresult& aStatus);

  IPCResult RecvOnProgress(const int64_t& aProgress,
                           const int64_t& aProgressMax);

  IPCResult RecvOnStatus(const nsresult& aStatus);

  IPCResult RecvFlushedForDiversion();

  IPCResult RecvDivertMessages();

  IPCResult RecvNotifyClassificationFlags(const uint32_t& aClassificationFlags,
                                          const bool& aIsThirdParty);

  IPCResult RecvNotifyFlashPluginStateChanged(
      const nsIHttpChannel::FlashPluginState& aState);

  IPCResult RecvSetClassifierMatchedInfo(const ClassifierInfo& info);

  IPCResult RecvSetClassifierMatchedTrackingInfo(const ClassifierInfo& info);

  IPCResult RecvAttachStreamFilter(
      Endpoint<extensions::PStreamFilterParent>&& aEndpoint);

  void ActorDestroy(ActorDestroyReason aWhy) override;

  void CreateDataBridge();

 private:
  virtual ~HttpBackgroundChannelChild();

  // Initiate the creation of the PBckground IPC channel.
  // Return false if failed.
  bool CreateBackgroundChannel();

  // Check OnStartRequest is sent by parent process over main thread IPC
  // but not yet received on child process.
  // return true before RecvOnStartRequestSent is invoked.
  // return false if RecvOnStartRequestSent is invoked but not
  // OnStartRequestReceived.
  // return true after both RecvOnStartRequestSend and OnStartRequestReceived
  // are invoked.
  bool IsWaitingOnStartRequest();

  // Associated HttpChannelChild for handling the channel events.
  // Will be removed while failed to create background channel,
  // destruction of the background channel, or explicitly dissociation
  // via OnChannelClosed callback.
  RefPtr<HttpChannelChild> mChannelChild;

  // True if OnStartRequest is received by HttpChannelChild.
  // Should only access on STS thread.
  bool mStartReceived = false;

  // Store pending messages that require to be handled after OnStartRequest.
  // Should be flushed after OnStartRequest is received and handled.
  // Should only access on STS thread.
  nsTArray<nsCOMPtr<nsIRunnable>> mQueuedRunnables;
};

}  // namespace net
}  // namespace mozilla

#endif  // mozilla_net_HttpBackgroundChannelChild_h