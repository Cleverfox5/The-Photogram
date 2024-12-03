# The-Photogram

## 1 сервер (PhotogramServer):
Отвечает за добавление/удаление/изменение/получение данных о пользователе, добавление/удаление/постраничная выгрузка друзей, авторизация, обнавление access токена.
Используемые технологии:
WinSock2, http, JWT(access and refresh), thread, PostgreSQL, thread, хеширование паролей.

Дополнительные библиотеки:
nlohmann/json.hpp, sodium.h, jwt-cpp/jwt.h, pqxx/pqxx.

## 2 сервер (PostsServer):
Отвечает за добавление/удаление/постраничная выгрузка постов, обращение к первому серверу для проверки access токена, добавление/удаление лайков.
Используемые технологии:
Boost::asio,boost::beast.

Дополнительные библиотеки:
nlohmann/json.hpp, boost/asio.hpp, boost/beast.hpp, boost/json.hpp, pqxx/pqxx.

## база данных
PostgreSQL

## клиент
React

демонстрация результата: https://youtu.be/nM7AjFNQq_s
