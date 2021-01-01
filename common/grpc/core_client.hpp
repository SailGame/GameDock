#pragma once

#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

#include "core/core.grpc.pb.h"

namespace common {
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class CoreClient {
 public:
  CoreClient(std::shared_ptr<Channel> channel)
      : mStub(Core::GameCore::NewStub(channel)) {}

 private:
  std::unique_ptr<Core::GameCore::Stub> mStub;
};
}  // namespace common