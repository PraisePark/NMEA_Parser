#pragma pack(4)
#ifndef NMEA_Coord_h
#define NMEA_Coord_h

class NMEA_Coord {

private:

	char sentenceID[4];
	double longitude;
	double latitude;
	int offset;
	char utcTime[4];
	char korTime[4];

public:

	bool setMemberVariables(char* sentence, int offset);
	char* getToken(char* sentence, char delimiter, int count);

	double transformCoordinate(char* str);

	void setSentenceID(char* sentenceID);
	void setLongitude(double lng);
	void setLatitude(double lat);
	void setOffset(int offset);
	void ConvertAndSetTime(char* utc_str);

	char* getSentenceID();
	double getLongitude();
	double getLatitude();	
	int getOffset();
	char* getUTCtime();
	char* getKORtime();
	
};

class RMC_Coord : public NMEA_Coord {

public:
	bool setMemberVariables(char* sentence, int offset);

};

class GGA_Coord : public NMEA_Coord {

public:
	bool setMemberVariables(char* sentence, int offset);

};

#endif