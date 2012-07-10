// JsonTCPTest.cpp : Defines the entry point for the console application.
//
#include "JsonTCPTest.h"

int main(int argc, char **argv) {
	int exit_ret = 0;

	Json::Rpc::TcpClient tcpClient(std::string("127.0.0.1"), 7500);
	Json::Value query;
	Json::Value params;
	Json::FastWriter writer;
	std::string queryStr;
	std::string responseStr;

	/* avoid compilation warnings */
	argc = argc;
	argv = argv;

	if(!networking::init())
	{
		std::cerr << "Networking initialization failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	if(!tcpClient.Connect())
	{
		std::cerr << "Cannot connect to remote peer!" << std::endl;
		exit(EXIT_FAILURE);
	}

	/* build JSON-RPC query */
	query["jsonrpc"] = "2.0";
	query["id"] = 1;
	query["method"] = "nui_workflow_quit";

	//params["index"] = 3;

	//query["params"] = params;

	queryStr = writer.write(query);
	std::cout << "Query is: " << queryStr << std::endl;

	if(tcpClient.Send(queryStr) == -1)
	{
		std::cerr << "Error while sending data!" << std::endl;
		exit(EXIT_FAILURE);
	}

	/* wait the response */
	if(tcpClient.Recv(responseStr) != -1)
	{
		std::cout << "Received: " << responseStr << std::endl;
	}
	else
	{
		std::cerr << "Error while receiving data!" << std::endl;
	}


exit_standard:
	tcpClient.Close();
	networking::cleanup();
	return exit_ret;

exit_critical:
	exit_ret = 1;
	printf("%s", "critical exit");
	goto exit_standard;
}