//#include "caching_interceptor.h"

// #ifdef BAZEL_BUILD
// #include "examples/protos/keyvaluestore.grpc.pb.h"
// #else
// #include "../kvstore/keyvaluestore.grpc.pb.h"
// #endif

//#include "../kvstore/kvstore.grpc.pb.h"
#include "kvstore_client.h"
#include "kvstore.grpc.pb.h"
namespace dylanwarble
{

bool KeyValueStoreClient::Put(const std::string &key,
                              const std::string &value)
{
  // Data we are sending to the server.
  PutRequest request;
  request.set_value(value);
  request.set_key(key);

  // Container for the data we expect from the server.
  PutReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->put(&context, request, &reply);

  // Act upon its status.
  if (status.ok())
    return true;

  return false;
}

bool KeyValueStoreClient::Remove(const std::string &key)
{
  // Data we are sending to the server.
  RemoveRequest request;
  request.set_key(key);

  // Container for the data we expect from the server.
  RemoveReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->remove(&context, request, &reply);

  // Act upon its status.
  if (status.ok())
    return true;

  return false;
}

std::vector<std::string> KeyValueStoreClient::Get(const std::string &key)
{

  ClientContext context;

  std::shared_ptr<ClientReaderWriter<GetRequest, GetReply>> stream(
      stub_->get(&context));

  GetRequest request;
  request.set_key(key);
  stream->Write(request);
  stream->WritesDone();

  GetReply reply;
  std::vector<std::string> response;
  while (stream->Read(&reply))
  {
    response.push_back(reply.value());
  }
  Status status = stream->Finish();

  if (status.ok())
    return response;

  std::vector<std::string> empty;
  return empty;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  // ClientContext context;
  // auto stream = stub_->get(&context);

  // GetRequest request;
  // request.set_key(key);
  // stream->Write(request);

  // // Get the value for the sent key
  // GetReply reply;
  // stream->Read(&reply);
  // // std::cout << key << " : " << reply.value() << "\n";

  // //std::vector<std::string> value = reply.value();
  // std::string value = reply.value();

  // stream->WritesDone();

  // Status status = stream->Finish();

  // if (status.ok())
  //   return reply.value();

  // return "";
}

} // namespace dylanwarble

int main(int argc, char **argv)
{
  dylanwarble::KeyValueStoreClient kvclient(grpc::CreateChannel(
      "localhost:50001", grpc::InsecureChannelCredentials()));

  //Random order of functions for quick initial test
  std::vector<std::string> response = kvclient.Get("hep");
  for (int i = 0; i < response.size(); i++)
    std::cout << response[i] << std::endl;
  kvclient.Put("hi", "hi");
  kvclient.Put("hi", "hello");
  response = kvclient.Get("hi");
  for (int i = 0; i < response.size(); i++)
    std::cout << response[i] << std::endl;
  kvclient.Remove("hi");

  response = kvclient.Get("hi");
  for (int i = 0; i < response.size(); i++)
    std::cout << response[i] << std::endl;

  return 0;
}
