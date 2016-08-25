#include <cstdio>
//#include <cstring>
#include <cstdlib>
#include "NMEA_Coord.h"


bool RMC_Coord::setMemberVariables(char* sentence, int offset) {

	/*char* checkSum = getToken(sentence, '*', 2);
	printf("checksum: %s\n", checkSum);*/

	sentence = getToken(sentence, '*', 1);

	char* lng = getToken(sentence, ',', 6);
	if(lng == NULL) return false;
	char* lat = getToken(sentence, ',', 4);
	if(lat == NULL) return false;
	char* time = getToken(sentence, ',', 2);
	if(time == NULL) return false;

	setSentenceID("RMC");

	double dLng = transformCoordinate(lng);
	setLongitude(dLng);

	double dLat = transformCoordinate(lat);
	setLatitude(dLat);

	setOffset(offset);

	ConvertAndSetTime(time);
	
	return true;
}

bool GGA_Coord::setMemberVariables(char* sentence, int offset) {

	sentence = getToken(sentence, '*', 1);

	char* lng = getToken(sentence, ',', 5);
	if(lng == NULL) return false;
	char* lat = getToken(sentence, ',', 3);
	if(lat == NULL) return false;
	char* time = getToken(sentence, ',', 2);
	if(time == NULL) return false;

	setSentenceID("GGA");
	double dLng = transformCoordinate(lng);
	setLongitude(dLng);

	double dLat = transformCoordinate(lat);
	setLatitude(dLat);

	setOffset(offset);

	ConvertAndSetTime(time);

	return true;
}

// sentence, delimiter가 같은 getToken()을 여러번 호출할 경우, turn을 내림차순으로 
char* NMEA_Coord::getToken(char* sentence, char delimiter, int turn) {

	/*char* str = new char[strlen(sentence)];
	strcpy(str, sentence);*/

	int count = 0;
	int tok_length = 0;

	while(*sentence != NULL) {

		if(*sentence == delimiter) {

			count++;

			if(count == turn) {

				*sentence = '\0';
				break;

			} else {

				tok_length = 0;
				sentence++;
				continue;
				//*(str-1) = '\0';
			}
		}

		tok_length++;
		sentence++;
	}

	if(tok_length == 0) return NULL;
	return sentence - tok_length;
}

double NMEA_Coord::transformCoordinate(char* str) {
	double d = atoi(str)/100;
	double m = atof(str) - d*100;
	return d + m/60.0;
}


void NMEA_Coord::setSentenceID(char* sentenceID) {
	int length = 0;
	while(sentenceID[length] != NULL) {
		NMEA_Coord::sentenceID[length] = sentenceID[length];
		length++;
	}
	NMEA_Coord::sentenceID[3] = '\0';
}

void NMEA_Coord::setLongitude(double lng) {
	NMEA_Coord::longitude = lng;
}

void NMEA_Coord::setLatitude(double lat) {
	NMEA_Coord::latitude = lat;
}

void NMEA_Coord::setOffset(int offset) {
	NMEA_Coord::offset = offset;
}

void NMEA_Coord::ConvertAndSetTime(char* utc_str) {

	int time = atoi(utc_str);

	int hour = time / 10000;
	time = time % 10000;
	int min = time / 100;
	int sec = time % 100;

	NMEA_Coord::utcTime[0] = hour;
	NMEA_Coord::utcTime[1] = min;
	NMEA_Coord::utcTime[2] = sec;
	NMEA_Coord::utcTime[3] = '\0';

	NMEA_Coord::korTime[0] = (hour+9) % 24;
	NMEA_Coord::korTime[1] = min;
	NMEA_Coord::korTime[2] = sec;
	NMEA_Coord::korTime[3] = '\0';
}


char* NMEA_Coord::getSentenceID() {
	return NMEA_Coord::sentenceID;
}

double NMEA_Coord::getLongitude() {
	return NMEA_Coord::longitude;
}

double NMEA_Coord::getLatitude() {
	return NMEA_Coord::latitude;
}

int NMEA_Coord::getOffset() {
	return NMEA_Coord::offset;
}

char* NMEA_Coord::getUTCtime() {
	return NMEA_Coord::utcTime;
}

char* NMEA_Coord::getKORtime() {
	return NMEA_Coord::korTime;
}