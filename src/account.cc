
#include <iostream>
#include <stdlib.h>

#include <nlohmann/json.hpp>
#include "account.h"
#include "getData.h"

using json = nlohmann::json;

Account::Account() {
	updateAccount();
}

// Updates account properties.
void Account::updateAccount() {
	/*
	json parsed = getData(endpoint)["account"];
	props.nav = std::stod(parsed["NAV"].get<std::string>());
	props.balance = std::stod(parsed["balance"].get<std::string>());
	props.id = parsed["id"].get<std::string>();
	props.openPositions = parsed["openPositionCount"];
	props.pl = std::stod(parsed["pl"].get<std::string>());
	*/
}


