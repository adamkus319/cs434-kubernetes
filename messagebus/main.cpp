#include "messagebus.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	string selfAddress;
	int selfPort;

	if(argc == 1){  // default (no arguments)
		selfAddress = "127.0.0.1";
		selfPort = 8080;	
	}
    else if(argc == 3){  // read in IP address and port
		selfAddress = argv[1];
		selfPort = stoi(argv[2]);
    }
	else{
        cerr << "Usage: " << argv[0] << " <ip> <port>" << endl;
	}

    std::vector<std::string> addresses = {"127.0.0.1", "127.0.0.1"};
    std::vector<int> ports = {11000, 11001};

    messagebus mb(addresses, ports, selfAddress, selfPort);  // set up threads to receive messages

    // pass control to terminal
    string rule;
    while(true){
		cout << "Enter a rule (type 'exit' to quit): ";
		getline(cin, rule);

		if(rule == "exit") {
			break;
		}

		cout << "Sending rule to other nodes: " << rule << endl;
		mb.sendMessage(rule);
    }

    
    return 0;
}
