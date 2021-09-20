

#ifndef GETDATA_H
#define GETDATA_H

#include <QDebug>
#include <nlohmann/json.hpp>
#include "../third-party/httplib.h"

using json = nlohmann::json;

inline json getData(const char *endpoint, httplib::Headers header = {{}}) {
  const char *token = "8707e5210dbd4db4748393c07c067e59-f818d153a948b855cad05bc30e66e498";
  const char *base = "https://api-fxpractice.oanda.com";

  httplib::Client client(base);
  client.set_bearer_token_auth(token);

  auto result = client.Get(endpoint, header);
  json parsed;

  if (result -> status == 200) {
    parsed = json::parse(result -> body);
  }

  return parsed;
}


#endif