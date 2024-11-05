#include "FormData.h"
#include <iostream>

std::string FormData::getSeparator(std::string & request) {
	return "--" + utils.getValueSomeHeader(request, "boundary=-");
}

void FormData::parse(std::string& request,
	std::unordered_map<std::string, std::string>& userProperties, 
	std::unordered_map<std::string, std::pair<std::size_t, std::size_t>> & FilesCoordinates) 
{
	std::string separator;
	separator = getSeparator(request);
	separatorToPropertis(request, separator, userProperties, FilesCoordinates);
}

void FormData::addPhoto(std::string& request, size_t & argPostion, size_t startPositon, size_t finishPositon,
	std::unordered_map<std::string, std::pair<std::size_t, std::size_t>>& filesCoordinates,
	std::unordered_map<std::string, std::string>& userProperties)
{
	size_t endPhotoHeaders = request.find("\r\n\r\n", startPositon, sizeof("\r\n\r\n") - 1);
	std::string photoHeaders = request.substr(startPositon, endPhotoHeaders - startPositon);
	std::string photo_type = utils.getType(photoHeaders, "Content-Type: ");
	userProperties["photo_type"] = photo_type;
	argPostion = photoHeaders.find("Content-Type: ");
	size_t startPhoto = startPositon + argPostion + sizeof("Content-Type: ") + photo_type.size() - 1 + 4;
	size_t sizePhoto = finishPositon - startPhoto - 2;
	std::string name = utils.getPropertisName(request, startPositon, "filename");

	filesCoordinates[name] = { startPhoto, sizePhoto };
}

void FormData::separatorToPropertis(std::string & request, std::string & separator,
	std::unordered_map<std::string, std::string>& userProperties, 
	std::unordered_map<std::string, std::pair<std::size_t, std::size_t>>& filesCoordinates) 
{
	size_t startPositon;
	if ((startPositon = request.find(separator)) == std::string::npos) {
		throw std::runtime_error("400");
	}
	startPositon += separator.size() + 2;
	size_t finishPositon = startPositon;
	size_t argPostion;
	size_t isNextPhoto = false;
	while ((finishPositon = request.find(separator, finishPositon + separator.size())) != std::string::npos) {
		if (isNextPhoto) {
			addPhoto(request, argPostion, startPositon, finishPositon, filesCoordinates, userProperties);
		}
		else if (request.find("Content-Type:", startPositon, finishPositon - startPositon) == std::string::npos) {
			std::cout << request.substr(startPositon, finishPositon - startPositon);
			std::string name = utils.getPropertisName(request, startPositon);
			size_t startValue = request.find("\r\n\r\n", startPositon);
			startValue += sizeof("\r\n\r\n") - 1;
			size_t endValue = request.find("\r\n", startValue);

			userProperties[name] = request.substr(startValue, endValue - startValue);
			
			if (name == "password") isNextPhoto = true;
		}

		startPositon = finishPositon + separator.size() - 1 + 2;
	}
}

void FormData::parsePhotoOnly(std::string& request,
	std::unordered_map<std::string, std::string>& userProperties,
	std::unordered_map<std::string, std::pair<std::size_t, std::size_t>>& filesCoordinates)
{
	std::string separator = getSeparator(request);
	
	size_t startPositon;
	if ((startPositon = request.find(separator)) == std::string::npos) {
		throw std::runtime_error("400");
	}
	startPositon += separator.size() + 2;
	size_t finishPositon = startPositon;
	size_t argPostion;
	size_t isNextPhoto = false;
	if ((finishPositon = request.find(separator, finishPositon + separator.size())) != std::string::npos) {
		addPhoto(request, argPostion, startPositon, finishPositon, filesCoordinates, userProperties);
	}
	else throw std::runtime_error("400");
}