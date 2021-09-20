

#include <iostream>
#include <iterator>
#include <string>
#include <sstream>

#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>

#include <math.h>
#include <nlohmann/json.hpp>
#include "streamdata.h"

using namespace Poco;
using namespace Poco::Net;
using json = nlohmann::json;

std::string accountId = "101-012-18350061-001";
std::string accessToken = "8707e5210dbd4db4748393c07c067e59-f818d153a948b855cad05bc30e66e498";
std::string instruments = "USD_SEK";
std::string domain = "https://stream-fxpractice.oanda.com";

StreamDataWorker::StreamDataWorker() {
};

void StreamDataWorker::startDataStream(const QString instrument, const int chartResolution) {
  this -> chartResolution = chartResolution;
  try {
      const Context::Ptr context = new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_NONE, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
      URI uri(domain + std::string("/v3/accounts/") + accountId + std::string("/pricing/stream?instruments=") + instrument.toStdString());
      // For testing purposes, multiple instruments.
      //URI uri(domain + std::string("/v3/accounts/") + accountId + std::string("/pricing/stream?instruments=USD_SEK,USD_CAD"));
      HTTPSClientSession session(uri.getHost(), uri.getPort(), context);
      session.setKeepAlive(true);

      std::string path(uri.getPathAndQuery());
      if (path.empty()) 
        path = "/";

      // send request
      HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
      req.set("Authorization", std::string("Bearer ") + accessToken);
      req.set("Accept-Datetime-Format", "UNIX");
      session.sendRequest(req);

      // get response
      HTTPResponse res;
      std::istream& rs = session.receiveResponse(res);

      // handle response
      std::ostringstream out_string_stream;
      streamBuffer(rs.rdbuf());
    }

    catch (const Exception &e) {
      std::cerr << e.displayText() << std::endl;
    }
}

void StreamDataWorker::streamBuffer(std::streambuf* stream_buffer) {
  std::istreambuf_iterator<char> eos; // end-of-range iterator
  std::istreambuf_iterator<char> iit (stream_buffer); // stream iterator
  std::string str;

  while (iit != eos && !QThread::currentThread() -> isInterruptionRequested()) {
    std::ostringstream oss;

    while (*iit != '\n') {
      oss << *iit++;
    }

    //print the tick 
    //std::cout << oss.str() << std::endl;
    json jsonTick = json::parse(oss.str());

    const QString type = QString::fromStdString(jsonTick["type"].get<std::string>());
    // When receiving a heartbeat, status will not exist and we need it further down below, so checking.
    QString status = "";
    if (type != "HEARTBEAT") {
      status = QString::fromStdString(jsonTick["status"].get<std::string>());
    }
    double time = std::stod(jsonTick["time"].get<std::string>());

    // Need to change this to handle dynamic resolution.
    StreamData streamData;
    streamData.type = type;
    streamData.time = time;
    streamData.status = status;

    // Use this to keep track if we need a new bar in the graph.
    streamData.resolutionCount = time / this -> chartResolution;

    // The connections will regularly send heartbeats in order to keep the connection active,
    // these heartbeats are not of interest and will be disregarded. Furthermore, when connecting
    // to the stream, multiple snapshots is sent, meaning when starting the program during e.g.,
    // weekends when no streaming prices should come, some snapshot will, causing the latest price
    // to become wrong. Therefore we only send prices that are tradeable, since we aren't interested in
    // something else.
    if (type != "HEARTBEAT" && status == "tradeable") {
      streamData.asksLiquidity = jsonTick["asks"][0]["liquidity"];
      streamData.asksPrice = std::stod(jsonTick["asks"][0]["price"].get<std::string>());
      streamData.bidsLiquidity = jsonTick["bids"][0]["liquidity"];
      streamData.bidsPrice = std::stod(jsonTick["bids"][0]["price"].get<std::string>());
      streamData.closePrice = (streamData.asksPrice + streamData.bidsPrice) / 2.0;
      streamData.closeoutBid = std::stod(jsonTick["closeoutBid"].get<std::string>());
      streamData.closeoutAsk = std::stod(jsonTick["closeoutAsk"].get<std::string>());
      streamData.tradeable = jsonTick["tradeable"];
      streamData.instrument = QString::fromStdString(jsonTick["instrument"].get<std::string>());
      streamData.validData = true;
      emit emitStreamData(streamData);
    }
    
    *iit++;
  }
}