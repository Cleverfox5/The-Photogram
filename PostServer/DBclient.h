#pragma once
#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <memory>
#include <libpq/libpq-fs.h>
#include <pqxx/pqxx>
#include <ctime>
#include <unordered_map>
#include <nlohmann/json.hpp>

using nlohmann::json;

class DBclient {
	std::shared_ptr<pqxx::connection> connection_;
public:
	DBclient(const std::string& connection_str);
	void addNewPost(const std::string& user_id, std::unordered_map<std::string, std::string>& property_map);
	void delPostPhotoById(const std::string& photo_id);
	void delPostById(const std::string& post_id, const std::string& id);
	void getPostsList(const std::string& id, const std::string& LIMIT, const std::string& OFFSET, json& json_);
	void getPostsListById(const std::string& author_id, const std::string& user_id, const std::string& LIMIT, const std::string& OFFSET, json& json_);
	std::vector<char> getPostPhotoById(const std::string& user_id, std::string& type);
	void likeClicked(const std::string& post_id, const std::string& user_id);
};