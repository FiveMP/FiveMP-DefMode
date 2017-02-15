#include <iostream>
#include <sstream>
#include <fstream>
#include <locale>

#include "api.h"

//JsonCpp
#include "json/json.h"

// Math
#include "sdk/CVector3.h"
#include "sdk/CVector4.h"

// API Function Imports
#include "sdk/APIServer.h"
#include "sdk/APIWorld.h"
#include "sdk/APIEntity.h"
#include "sdk/APIVehicle.h"
#include "sdk/APIVisual.h"
#include "sdk/APIPlayer.h"
#include "sdk/APIObject.h"

bool to_bool(std::string str)
{
	std::locale loc;
	for (std::string::size_type i = 0; i<str.length(); ++i)
		std::cout << std::tolower(str[i], loc);

	std::istringstream is(str);
	bool b;
	is >> std::boolalpha >> b;
	return b;
};

// When Plugin gets loaded
extern "C" DLL_PUBLIC bool API_Initialize(void)
{
	API::Server::PrintMessage("Gamemode Initializing...");

	API::Vehicle::CreateVehicle("elegy", CVector3{ 1533.53f, 3282.39f, 52.5f }, 195.0f);
	API::Vehicle::CreateVehicle("comet3", CVector3{ 1527.65f, 3296.66f, 52.5f }, 195.0f);
	API::Vehicle::CreateVehicle("blazer5", CVector3{ 1519.32f, 3280.2f, 52.5f }, 195.0f);
	API::Vehicle::CreateVehicle("voltic2", CVector3{ 1516.84f, 3293.88f, 52.5f }, 195.0f);

	API::World::SetTime(13, 0, 0);

	int hour, minute, second;
	API::World::GetTime(&hour, &minute, &second);
	std::cout << "Time: " << hour << ":" << minute << ":" << second << std::endl;

	// Load Objects
	Json::Value root;
	Json::Reader reader;

	std::ifstream stream("./maps/objects.json", std::ifstream::binary);
	stream >> root;

	if (!reader.parse(stream, root, false))
	{
		// report to the user the failure and their locations in the document.
		std::cout << reader.getFormatedErrorMessages() << std::endl;;
	}

	const int objectcount = root["Map"]["Objects"]["MapObject"].size();

	for (int i = 0; i < objectcount; i++)
	{
		CVector3 position{
			(float)std::atof(root["Map"]["Objects"]["MapObject"][i]["Position"]["X"].asCString()),
			(float)std::atof(root["Map"]["Objects"]["MapObject"][i]["Position"]["Y"].asCString()),
			(float)std::atof(root["Map"]["Objects"]["MapObject"][i]["Position"]["Z"].asCString())
		};

		CVector4 quaternion = {
			(float)std::atof(root["Map"]["Objects"]["MapObject"][i]["Quaternion"]["X"].asCString()),
			(float)std::atof(root["Map"]["Objects"]["MapObject"][i]["Quaternion"]["Y"].asCString()),
			(float)std::atof(root["Map"]["Objects"]["MapObject"][i]["Quaternion"]["Z"].asCString()),
			(float)std::atof(root["Map"]["Objects"]["MapObject"][i]["Quaternion"]["W"].asCString())
		};

		API::Object::CreateObjectWithHash(atoi(root["Map"]["Objects"]["MapObject"][i]["Hash"].asCString()), position, quaternion, to_bool(root["Map"]["Objects"]["MapObject"][i]["Dynamic"].asCString()));
	}
	// END Load Objects

	API::Server::PrintMessage("Gamemode Initialized!");
	return true;
}

// When plugin gets unloaded
extern "C" DLL_PUBLIC bool API_Close(void)
{
	std::cout << "close" << std::endl;
	return true;
}

// Every server tick this gets called
extern "C" DLL_PUBLIC bool API_OnTick(void)
{
	//API::Visual::ShowMessageAboveMap("I don't like people!", "CHAR_DEFAULT", 1, "Jack", "");
	return true;
}

// Player Connecting
extern "C" DLL_PUBLIC bool API_OnPlayerConnecting(const std::string guid)
{
	std::ostringstream oss;
	oss << "Player connecting with [guid: " << guid << "]";
	API::Server::PrintMessage(oss.str());

	oss.str(std::string());
	oss.clear();

	oss << "~p~A Player is connecting [" << guid << "]";
	API::Visual::ShowMessageAboveMap(oss.str(), "CHAR_DEFAULT", 1, "Server", "Player Joining");
	return true;
}

// Player Connected
extern "C" DLL_PUBLIC bool API_OnPlayerConnected(int entity, int playerid)
{
	std::ostringstream oss;
	oss << "~g~You Connected! ~o~[~w~ID: " << playerid << "~o~]";
	API::Visual::ShowMessageAboveMapToPlayer(entity, oss.str().c_str(), "CHAR_CREATOR_PORTRAITS", 1, "Server", "");

	API::Entity::SetPosition(entity, CVector3{ 1527.62f, 3274.39f, 53.0f });


	CVector3 position = API::Entity::GetPosition(entity);

	oss.str(std::string());
	oss.clear();

	oss << "~p~Position: " << position.fX << " " << position.fY << " " << position.fZ;
	API::Visual::ShowMessageAboveMapToPlayer(entity, oss.str().c_str(), "CHAR_CREATOR_PORTRAITS", 5, "Server", "Position");
	return true;
}