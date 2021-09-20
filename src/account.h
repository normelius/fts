#ifndef ACCOUNT_H
#define ACCOUNT_H


struct AccountProperties {
	double nav = 0.0;
	double balance = 0.0;
	std::string id = "";
	int openPositions = 0;
	double pl = 0.0;
};

class Account {
  public:
    Account();
		void updateAccount();
    AccountProperties props;

	private:
		const char *endpoint = "/v3/accounts/101-012-18350061-001";

};


#endif
