#include "DBclient.h"

DBclient::DBclient(const std::string& connection_str)
	: connection_(std::make_shared<pqxx::connection>(connection_str))
{
	try {
		if (connection_->is_open()) {
			std::cout << "Open db successfullt: " << connection_->dbname() << std::endl;
			//connection_->prepare("", "");
		}
		else {
			std::cerr << "Can't open database\n";
			throw std::runtime_error("Can't open db");
		}
	}
	catch (const std::exception e) {
		std::cout << e.what() << std::endl;
		throw;
	}
}

void DBclient::delPostPhotoById(const std::string& photo_id) {
	try{
		pqxx::work work(*connection_);
		connection_->prepare("delPostPhotoById",
			"DELETE FROM post_photos"
			"WHERE id = $1"
		);
		work.exec_prepared0("delPostById", photo_id);
		work.commit();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("500");
	}
}

void DBclient::delPostById(const std::string& post_id, const std::string& id) {
	try {
		connection_->prepare("delPostById", 
			R"(
				DO $$
				DECLARE
					photo_id_del INT;
				BEGIN
					IF EXISTS(
						SELECT 1
						FROM posts
						WHERE id = $1 AND user_id = $2
					)THEN
						DELETE FROM likes
						WHERE post_id = $1;
		
						SELECT photo_id
						INTO photo_id_del
						FROM posts
						WHERE id = $1;
		 
						DELETE FROM posts
						WHERE id = $1;
		
						DELETE FROM post_photos
						WHERE id = photo_id_del;
					END IF;
				END$$;
			)"
		);

		pqxx::work work(*connection_);
		work.exec_prepared0("delPostById", post_id, id);
		work.commit();
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("500");
	}
}

void DBclient::addNewPost(const std::string& user_id, std::unordered_map<std::string, std::string>& property_map) {
	bool is_add_photo = false;
	std::string photo_id;
	try {
		connection_->prepare("addNewPostPhoto", "INSERT INTO post_photos(photo_type, photo)"
			"VALUES($1, $2)"
			"RETURNING id;");
		connection_->prepare("addNewPostInformation",
			"INSERT INTO posts(user_id, date_posted, post_description, photo_id) "
			"VALUES ($1, LOCALTIMESTAMP, $2, $3);");

		pqxx::work work(*connection_);
		pqxx::row res;

		std::size_t size_photo = property_map["photo"].size();
		std::vector<std::byte> binaryData(size_photo);
		std::memcpy(binaryData.data(), property_map["photo"].data(), size_photo);

		res = work.exec_prepared1("addNewPostPhoto",
			property_map["Content-Type"],
			pqxx::binarystring(reinterpret_cast<const char*>(binaryData.data()), binaryData.size())
		);
		is_add_photo = true;
		
		photo_id = res[0].c_str();
		work.exec_prepared0("addNewPostInformation",
			user_id,
			property_map["description"],
			photo_id);

		work.commit();
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
		if (is_add_photo) {
			delPostPhotoById(photo_id);
		}
		throw std::runtime_error("500");
	}
}

void DBclient::getPostsList(const std::string& id, const std::string& LIMIT, const std::string& OFFSET, json& json_) {
	try {
		pqxx::work work(*connection_);
		pqxx::result result;

		connection_->prepare("getPostsList",R"(
			WITH selected_users AS(
				SELECT friend_id
				FROM friends
				WHERE user_id = $1
			)
			SELECT po.id, u.nickname, po.date_posted, po.post_description, po.like_count, po.photo_id, u.photo_id AS profile_picture_id,
			CASE
			WHEN EXISTS(
				SELECT 1
				FROM likes
				WHERE(post_id = po.id AND user_id = $1)
			) THEN 'true'
			ELSE 'false'
			END AS is_liked
			FROM posts po
			JOIN users u
			on u.id = po.user_id
			WHERE
			po.user_id IN(SELECT friend_id FROM selected_users)
			OR user_id = $1

			ORDER BY date_posted DESC
			LIMIT $2 OFFSET $3;)"
		);

		result = work.exec_prepared("getPostsList",
			id,
			LIMIT,
			OFFSET);
		if (result.empty()) return;

		work.commit();

		for (const auto& row : result) {
			json jsonElement;
			jsonElement["post_id"] = row[0].c_str();
			jsonElement["nickname"] = row[1].c_str();
			jsonElement["date_posted"] = row[2].c_str();
			jsonElement["description"] = row[3].c_str();
			jsonElement["like_count"] = row[4].c_str();
			jsonElement["photo_id"] = row[5].c_str();
			jsonElement["profile_picture_id"] = row[6].c_str();
			jsonElement["is_liked"] = row[7].c_str();
			json_.push_back(jsonElement);
		}
	}
	catch (const std::exception e) {
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("500");
	}
}

std::vector<char> DBclient::getPostPhotoById(const std::string& photo_id, std::string& type) {
	try{
		pqxx::work work(*connection_);
		pqxx::row row;

		connection_->prepare("getPostPhotoById",
			"SELECT photo_type, photo FROM post_photos "
			"WHERE id = $1");

		row = work.exec_prepared1("getPostPhotoById",
			photo_id);
		work.commit();

		type = row[0].c_str();

		pqxx::binarystring binData(row[1]);

		return std::vector<char>(binData.begin(), binData.end());
	}
	catch (const std::exception e) {
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("500");
	}
}

void DBclient::likeClicked(const std::string& post_id, const std::string& user_id) {
	try{
		connection_->prepare("addLikeFirst",
			"INSERT INTO likes(post_id, user_id) "
			"VALUES($1, $2); "
		);
		connection_->prepare("addLikeSecond",
			"UPDATE posts "
			"SET like_count = COALESCE(like_count, 0) + 1 "
			"WHERE id = $1; "
		);
		connection_->prepare("delLikeFirst",
			"DELETE FROM likes "
			"WHERE post_id = $1 AND user_id = $2; "
		);
		connection_->prepare("delLikeSecond",
			"UPDATE posts "
			"SET like_count = like_count - 1 "
			"WHERE id = $1; "
		);
		connection_->prepare("isExists",
			R"(
			SELECT 
			CASE
				WHEN EXISTS(
					SELECT 1
					FROM likes
					WHERE post_id = $1 AND user_id = $2
				)THEN 'true'
				ELSE 'false'
			END AS is_exist
			)");
		pqxx::work work(*connection_);
		pqxx::row row;
		row = work.exec_prepared1("isExists", post_id, user_id);
		std::string val = row[0].c_str();
		if (val == "false") {
			work.exec_prepared0("addLikeFirst", post_id, user_id);
			work.exec_prepared0("addLikeSecond", post_id);
		}
		else {
			work.exec_prepared0("delLikeFirst", post_id, user_id);
			work.exec_prepared0("delLikeSecond", post_id);
		}

		work.commit();
	}
	catch (const std::exception e) {
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("500");
	}
}