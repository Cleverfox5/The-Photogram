#pragma warning(disable : 4996)
#include "WorkWithPostgreSQL.h"
#include <iostream>
#include <jwt-cpp/jwt.h>

WorkWithPostgreSQL::WorkWithPostgreSQL(const std::string & connection_str) 
	: connect(new pqxx::connection (connection_str))
{
	try {
		if (connect->is_open()) {
			std::cout << "Opened database successfully: " << connect->dbname() << std::endl;
			connect->prepare("addPhoto", "INSERT INTO profile_picture(photo_type, photo) "
				"VALUES ($1, $2) "
				"RETURNING id;");
			connect->prepare("addNewUser", 
				"INSERT INTO users(nickname, first_name, last_name, email, is_male, birthdate, password) "
				"VALUES ($1, $2, $3, $4, $5, $6, $7)"
				"RETURNING id");
			connect->prepare("getHashPassword",
				"SELECT password, id FROM users WHERE UPPER(email) = UPPER($1)");
			connect->prepare("getNickname",
				"SELECT nickname FROM users WHERE id = $1");
			connect->prepare("getProfileInformation",
				"SELECT first_name, last_name, email, is_male, birthdate, id, nickname FROM users WHERE nickname = $1");
			connect->prepare("getProfilePhotoId",
				"SELECT photo_id FROM users WHERE nickname = $1");
			connect->prepare("getProfilePhoto",
				"SELECT photo_type, photo FROM profile_picture WHERE id = $1");
			connect->prepare("connectPropertiesAndPhoto",
				"UPDATE users SET photo_id = $1 WHERE id = $2");
			connect->prepare("deleteRow",
				"DELETE FROM users WHERE id = $1");
			connect->prepare("getStatus",
				"SELECT "
				"CASE "
				"WHEN EXISTS( "
				"	SELECT 1 "
				"	FROM friends "
				"	WHERE user_id = $1 AND friend_id = $2 "
				"	)THEN 'true' "
				"ELSE 'false' "
				"END AS is_friend "
			);
			connect->prepare("deletePhoto",
				"DELETE FROM profile_picture WHERE id = $1");
			connect->prepare("getUsersList",
					 "SELECT "
				"	nickname, "
				"	photo_id, "
				"	id, "
				"	CASE "
				"		WHEN EXISTS( "
				"			SELECT 1, user_id, friend_id "
				"			FROM friends "
				"			WHERE(user_id = $1 AND friend_id = id) "
				"		) THEN 'true' "
				"		ELSE 'false' "
				"	END AS is_friend "
				"FROM users "
				"WHERE "
				"	UPPER(nickname) LIKE UPPER('%' || $2 || '%') AND id != $1 "
				"LIMIT $3 OFFSET $4;");
			connect->prepare("makeFriend",
				"INSERT INTO Friends(user_id, friend_id) "
				"VALUES($1, $2)");
			connect->prepare("deleteFriend",
				"DELETE FROM Friends "
				"WHERE user_id = $1 AND friend_id = $2"
			);

			connect->prepare("updateProfilePhoto",
				"UPDATE profile_picture SET photo_type = $1, photo = $2 "
				"WHERE id = (SELECT photo_id FROM users WHERE id = $3);"
			);
			connect->prepare("updateProfileDate",
				"UPDATE users SET nickname = $1, first_name = $2, last_name = $3, email = $4, is_male = $5, birthdate = $6 "
				"WHERE id = $7; "
			);
			connect->prepare("updatePassword",
				"UPDATE users SET password = $1 "
				"WHERE id = $2; "
			);
		}
		else {
			std::cerr << "Can't open database\n";
			throw std::runtime_error("Can't open db");
		}
	}
	catch (const std::exception& e) {
		throw;
	}
}

std::string WorkWithPostgreSQL::addNewPhoto(std::string & request, std::unordered_map<std::string, std::pair<std::size_t, std::size_t>> & filesCoordinates, std::unordered_map<std::string, std::string> & userProperties) {
	try {
		pqxx::work work(*connect);
		pqxx::row res;


		auto element = filesCoordinates.begin();
		std::string file = request.substr(element->second.first, element->second.second);
		
		std::vector<std::byte> binaryData(file.size());
		std::memcpy(binaryData.data(), file.data(), file.size());

		res = work.exec_prepared1("addPhoto",
			userProperties["photo_type"],
			pqxx::binarystring(reinterpret_cast<const char*>(binaryData.data()), binaryData.size())
		);

		
		std::string id = res[0].c_str();

		work.commit();

		return id;
	}
	catch (const std::exception & e) {
		throw std::runtime_error("Can't add photo");
	}
}

std::string WorkWithPostgreSQL::getNicknameById(std::string id) {
	try {
		pqxx::work work(*connect);
		pqxx::row res;

		res = work.exec_prepared1("getNickname",
			id);
		work.commit();
		return res[0].c_str();
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Can't getNickname");
	}
}

nlohmann::json WorkWithPostgreSQL::getProfileInformation(std::string nickname) {
	try {
		pqxx::work work(*connect);
		pqxx::row res;

		res = work.exec_prepared1("getProfileInformation",
			nickname);

		work.commit();

		if (res.empty()) throw std::runtime_error("404");

		nlohmann::json responseBody;
		responseBody["first_name"] = res[0].c_str();
		responseBody["last_name"] = res[1].c_str();
		responseBody["email"] = res[2].c_str();
		responseBody["is_male"] = res[3].as<bool>();
		responseBody["birthdate"] = res[4].c_str();
		responseBody["id"] = res[5].c_str();
		responseBody["nickname"] = res[6].c_str();
		return responseBody;

	}
	catch (const std::exception& e) {
		std::cerr << e.what();
		if (std::string(e.what()).find("got 0") != std::string::npos) throw std::runtime_error("404");
		throw;
	}
}

std::vector<char> WorkWithPostgreSQL::getProfilePhoto(std::string nickname, std::string & type) {
	try {
		pqxx::work work(*connect);
		pqxx::row res;

		res = work.exec_prepared1("getProfilePhotoId",
			nickname);
		res = work.exec_prepared1("getProfilePhoto", res[0].c_str());

		work.commit();
		type = res[0].c_str();
		pqxx::binarystring binData(res[1]);

		return std::vector<char>(binData.begin(), binData.end());
	}
	catch (const std::exception& e) {
		std::cerr << e.what();
		if (std::string(e.what()).find("got 0") != std::string::npos) throw std::runtime_error("404");
		throw;
	}
}

std::string WorkWithPostgreSQL::checkPassword(std::string login, std::string password) {
	try {
		pqxx::work work(*connect);
		pqxx::row res;

		/*res = work.exec_prepared1("getNickname",
			18);

		std::cout << res[0].c_str();*/

		res = work.exec_prepared1("getHashPassword",
			login);
		
		std::string hashPassword = res[0].c_str();

		if (!hashTools.get())	hashTools = std::make_shared<HashTools>();

		work.commit();

		if (!hashTools->verify(password, hashPassword)) 
			throw std::runtime_error("Invalid login or password");

		return res[1].c_str();//если пароль неправильный, то вылетает исключение

	} catch(const std::exception & e) {
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("Invalid login or password");
	}
}

std::string WorkWithPostgreSQL::addProperties(std::unordered_map<std::string, std::string> & userProperties) {
	try {
		pqxx::work work(*connect);
		pqxx::row res;

		res = work.exec_prepared1("addNewUser",
			userProperties["nickname"],
			userProperties["first_name"],
			userProperties["last_name"],
			userProperties["email"],
			userProperties["is_male"],
			userProperties["birthdate"],
			userProperties["password"]
		);

		work.commit();

		return res[0].c_str();
	}
	catch (const std::exception & e) {
		if (std::string(e.what()).find("users_nickname_key") != std::string::npos)
			throw std::runtime_error("The nickname is already occupied");
		else if (std::string(e.what()).find("users_email_key") != std::string::npos)
			throw std::runtime_error("This email is already occupied");
		else throw;
	}
}

void WorkWithPostgreSQL::connectPropertiesAndPhoto(std::string userId, std::string photoId) {
	try {
		pqxx::work work(*connect);
		
		work.exec_prepared0("connectPropertiesAndPhoto",
			photoId, userId);

		work.commit();
	}
	catch (const std::exception & e) {
		throw e;
	}
}

void WorkWithPostgreSQL::deleteRow(std::string userId) {
	try {
		pqxx::work work(*connect);

		work.exec_prepared0("deleteRow",
			userId);
		work.commit();
	}
	catch (const std::exception & e) {
		std::cerr << "Ќе получилось удалить строку\n";
	}
}

void WorkWithPostgreSQL::deletePhoto(std::string PhotoId) {
	try {
		pqxx::work work(*connect);

		work.exec_prepared0("deletePhoto",
			PhotoId);
		work.commit();
	}
	catch (const std::exception& e) {
		std::cerr << "Ќе получилось удалить строку\n";
	}
}

std::string WorkWithPostgreSQL::addNewUser(std::string& request, nlohmann::json& registrAnswerJson) {
	bool isAddProperties = false;
	bool isAddPhoto = false;
	std::string photoId;
	std::string userId = "";
	std::unordered_map<std::string, std::string> userProperties;
	std::unordered_map<std::string, std::pair<std::size_t, std::size_t>> filesCoordinates;
	try {
		formData.parse(request, userProperties, filesCoordinates);

		userId = addProperties(userProperties);
		isAddProperties = true;
		photoId = addNewPhoto(request, filesCoordinates, userProperties);
		isAddPhoto = true;

		connectPropertiesAndPhoto(userId, photoId);

		return userId;
	}
	catch (std::exception const & e) {
		std::cerr << e.what() << std::endl;
		if (isAddPhoto) deletePhoto(photoId);
		if (isAddProperties) deleteRow(userId);
		throw;
	}
}

void WorkWithPostgreSQL::getUsers(std::unordered_map<std::string, std::string>& properties, nlohmann::json& json, std::string id) {
	try {
		pqxx::work work(*connect);
		pqxx::result result;

		result = work.exec_prepared("getUsersList",
			id,
			properties["substr"],
			properties["LIMIT"],
			properties["OFFSET"]
		);


		if (result.empty())
			return;

		work.commit();
		
		for (const auto & row : result) {
			nlohmann::json jsonElement;
			jsonElement["nickname"] = row[0].c_str();
			jsonElement["photo_id"] = row[1].c_str();
			jsonElement["friend_id"] = row[2].c_str();
			jsonElement["is_friend"] = row[3].c_str();
			json.push_back(jsonElement);
		}

	}
	catch (const std::exception & e) {
		std::cerr << e.what();
		throw;
	}
}

std::vector<char> WorkWithPostgreSQL::getPhotoById(std::string id, std::string& type) {
	try {
		pqxx::work work(*connect);
		pqxx::row res;

		res = work.exec_prepared1("getProfilePhoto", id);

		work.commit();

		type = res[0].c_str();

		pqxx::binarystring binData(res[1]);

		return std::vector<char>(binData.begin(), binData.end());
	}
	catch (const std::exception& e) {
		std::cerr << e.what();
		if (std::string(e.what()).find("got 0") != std::string::npos) throw std::runtime_error("404");
		throw;
	}
}

void WorkWithPostgreSQL::makeFriend(std::string& user_id, std::string& friend_id) {
	try {
		pqxx::work work(*connect);

		work.exec_prepared0("makeFriend",
			user_id, friend_id);

		work.commit();
	}
	catch (const std::exception& e) {
		throw std::runtime_error("404");
	}
}

void WorkWithPostgreSQL::deleteFriend(std::string& user_id, std::string& friend_id) {
	try {
		pqxx::work work(*connect);

		work.exec_prepared0("deleteFriend",
			user_id, friend_id);

		work.commit();
	}
	catch (const std::exception& e) {
		throw std::runtime_error("404");
	}
}

void WorkWithPostgreSQL::getStatus(const std::string& user_id, const std::string& friend_id, nlohmann::json& json) {
	try {
		pqxx::work work(*connect);
		pqxx::row row;
		std::string status;

		row = work.exec_prepared1("getStatus",
			user_id,
			friend_id
		);

		std::string result = row[0].c_str();

		if (result == "true")
			status = "friend";
		else
			status = "stranger";

		work.commit();

		json["status"] = status;
	}
	catch (const std::exception & e) {
		throw;
	}
}

void WorkWithPostgreSQL::updatePhoto(std::string& request, std::unordered_map<std::string, std::pair<std::size_t, std::size_t>>& filesCoordinates, std::unordered_map<std::string, std::string>& userProperties, std::string & id) {
	try {
		pqxx::work work(*connect);

		auto element = filesCoordinates.begin();
		std::string file = request.substr(element->second.first, element->second.second);

		std::vector<std::byte> binaryData(file.size());
		std::memcpy(binaryData.data(), file.data(), file.size());

		work.exec_prepared0("updateProfilePhoto",
			userProperties["photo_type"],
			pqxx::binarystring(reinterpret_cast<const char*>(binaryData.data()), binaryData.size()),
			id
		);


		work.commit();
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Can't add photo");
	}
}

void WorkWithPostgreSQL::updateProfilePhoto(std::string& request, std::string& id) {
	std::unordered_map<std::string, std::string> userProperties;
	std::unordered_map<std::string, std::pair<std::size_t, std::size_t>> filesCoordinates;
	try {
		formData.parsePhotoOnly(request, userProperties, filesCoordinates);

		updatePhoto(request, filesCoordinates, userProperties, id);
	}
	catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
		throw;
	}
}

void WorkWithPostgreSQL::updateProfileData(nlohmann::json& jsonBody, std::string& id) {
	try {
		
		updateData(jsonBody, id);
	}
	catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
		throw;
	}
}

void WorkWithPostgreSQL::updateProfilePassword(std::string& password, std::string& id) {
	try {
		updatePassword(password, id);
	}
	catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
		throw;
	}
}

void WorkWithPostgreSQL::updateData(nlohmann::json& jsonBody, std::string & id) {
	try {
		pqxx::work work(*connect);

		std::string nickname = jsonBody["nickname"];
		std::string first_name = jsonBody["firstname"];
		std::string last_name = jsonBody["lastname"];
		std::string email = jsonBody["email"];
		bool is_male = jsonBody["is_male"];
		std::string birthdate = jsonBody["birthdate"];

		work.exec_prepared0("updateProfileDate",
			nickname,
			first_name,
			last_name,
			email,
			is_male,
			birthdate,
			id
		);

		work.commit();
	}
	catch (const std::exception& e) {
		if (std::string(e.what()).find("users_nickname_key") != std::string::npos)
			throw std::runtime_error("The nickname is already occupied");
		else if (std::string(e.what()).find("users_email_key") != std::string::npos)
			throw std::runtime_error("This email is already occupied");
		else throw;
	}
}

void WorkWithPostgreSQL::updatePassword(std::string& password, std::string& id) {
	try {
		pqxx::work work(*connect);

		work.exec_prepared0("updatePassword",
			password,
			id
		);

		work.commit();
	}
	catch (const std::exception& e) {
		throw;
	}
}