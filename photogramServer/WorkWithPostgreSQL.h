#pragma once
#include <memory>
#include <unordered_map>
#include "FormData.h"
#include <pqxx/pqxx>
#include <nlohmann/json.hpp>
#include "HashTools.h"

class WorkWithPostgreSQL {
	FormData formData;
	std::shared_ptr<pqxx::connection> connect;
	std::shared_ptr<HashTools> hashTools;

	std::string addNewPhoto(std::string& request, std::unordered_map<std::string, std::pair<std::size_t, std::size_t>>& filesCoordinates, std::unordered_map<std::string, std::string>& userProperties);
	std::string addProperties(std::unordered_map<std::string, std::string>& userProperties);
	void connectPropertiesAndPhoto(std::string userId, std::string photoId);
	void deleteRow(std::string userId);
	void deletePhoto(std::string PhotoId);
	void updatePhoto(std::string& request, std::unordered_map<std::string, std::pair<std::size_t, std::size_t>>& filesCoordinates, std::unordered_map<std::string, std::string>& userProperties, std::string& id);
	void updateData(nlohmann::json& jsonBody, std::string& id);
	void updatePassword(std::string & password, std::string& id);
public:
	WorkWithPostgreSQL(const std::string & connection_str);
	
	std::string getNicknameById(std::string id);
	std::string addNewUser(std::string& request, nlohmann::json& registrAnswerJson);
	std::string checkPassword(std::string login, std::string password);
	nlohmann::json getProfileInformation(std::string nickname);
	std::vector<char> getProfilePhoto(std::string nickname, std::string& type);
	void getUsers(std::unordered_map<std::string, std::string>& properties, nlohmann::json& json, std::string id);
	std::vector<char> getPhotoById(std::string id, std::string& type);
	void makeFriend(std::string & user_id, std::string & friend_id);
	void deleteFriend(std::string& user_id, std::string& friend_id);
	void getStatus(const std::string & user_id, const std::string & friend_id, nlohmann::json& json);
	void updateProfilePhoto(std::string& request, std::string& id);
	void updateProfileData(nlohmann::json& jsonBody, std::string& id);
	void updateProfilePassword(std::string& password, std::string& id);
};