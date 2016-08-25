#include <cstdio>
#include <cstdlib>
#include <vector>
#include "NMEA_Coord.h"

#define NMEA_PARSER_VER "001\0"
#define NMEA_PARSER_VER_LENGTH 4
#define NMEA_LOG_FILE_FULLPATH_LENGTH 80

struct ParserFormatHeader {

	char version[NMEA_PARSER_VER_LENGTH];
	char nmea_file_full_path[NMEA_LOG_FILE_FULLPATH_LENGTH];
	int nmea_coord_count;
};

char* convertWideCharToMultibyte(wchar_t* w_file_path) {
	//printf ("wchar_t string: %ls \n",w_file_path);
	char m_file_path[80];
	int ret = wcstombs ( m_file_path, w_file_path, sizeof(m_file_path) );
	if (ret == 80) m_file_path[79]='\0';
	//printf("multibyte string: %s \n",m_file_path);
	return m_file_path;
}

int main() {

	wchar_t w_file_path[80];
	printf("Input NMEA LogFile Path: ");
	wscanf(L"%ls", w_file_path);
	fflush(stdin);

	ParserFormatHeader formatHeader;
	memcpy(formatHeader.version, NMEA_PARSER_VER, NMEA_PARSER_VER_LENGTH);
	memcpy(formatHeader.nmea_file_full_path, convertWideCharToMultibyte(w_file_path), NMEA_LOG_FILE_FULLPATH_LENGTH);
	//printf("version: %s\n", formatHeader.version);
	//printf("nmea_file_full_path: %s\n", formatHeader.nmea_file_full_path);

	int file_size = 0;
	int offset = 0;
	char sentenceBuf[82] = {0,};

	FILE* nmeaLogFile = fopen(formatHeader.nmea_file_full_path, "rb");

	if(nmeaLogFile == NULL) {
		printf("NMEA Log Text File OPEN FAILED.\n");
		return 0;
	} 

	fseek(nmeaLogFile, 0, SEEK_SET);
	fseek(nmeaLogFile, 0, SEEK_END);
	file_size = ftell(nmeaLogFile);
	fseek(nmeaLogFile, 0, SEEK_SET);

	char* nmeaLogBuf = NULL;
	nmeaLogBuf = new char[file_size];
	if( nmeaLogBuf == NULL ) {
		fclose(nmeaLogFile);
		return 0;
	}
	memset(nmeaLogBuf, 0x00, file_size);
	
	size_t st = fread(nmeaLogBuf, 1, file_size, nmeaLogFile);
	if(st != file_size) {
		printf("ferror: %d \t size_t: %d\n", ferror(nmeaLogFile), st);
		fclose(nmeaLogFile);
		return 0;
	}

	fclose(nmeaLogFile);

	int nmeaLogBuf_idx = 0;
	int sentenceBuf_idx = 0;
	std::vector<NMEA_Coord> nmea_coord_vec;

	while(nmeaLogBuf[nmeaLogBuf_idx] != NULL) {

		sentenceBuf[sentenceBuf_idx] = nmeaLogBuf[nmeaLogBuf_idx];
		sentenceBuf_idx++;
		nmeaLogBuf_idx++;

		if( (nmeaLogBuf[nmeaLogBuf_idx] == '$') || (nmeaLogBuf_idx == file_size) ) {

			offset = nmeaLogBuf_idx - sentenceBuf_idx;

			if( (sentenceBuf[3] == 'R') && (sentenceBuf[4] == 'M') && (sentenceBuf[5] == 'C') ) {

				RMC_Coord rmc;
				if(rmc.setMemberVariables(sentenceBuf, offset)) 
					nmea_coord_vec.push_back(rmc);

			} else if( (sentenceBuf[3] == 'G') && (sentenceBuf[4] == 'G') && (sentenceBuf[5] == 'A') ) {

				GGA_Coord gga;
				if(gga.setMemberVariables(sentenceBuf, offset))
					nmea_coord_vec.push_back(gga);
			}
			memset(sentenceBuf, 0x00, sentenceBuf_idx);
			sentenceBuf_idx = 0;
		}
	}

	if( nmeaLogBuf ) {
		delete []nmeaLogBuf;
		nmeaLogBuf = NULL;
	}

	/*std::vector<NMEA_Coord>::iterator it;
	for(it = nmea_coord_vec.begin(); it != nmea_coord_vec.end(); ++it) {
		printf("[ofs:%d  ID:%s  lat:%f  lon:%f  utc:%s  kor:%s]\n", 
		(*it).getOffset(), (*it).getSentenceID(), (*it).getLatitude(), 
		(*it).getLongitude(), (*it).getUTCtime(), (*it).getKORtime());
	}*/

	formatHeader.nmea_coord_count = nmea_coord_vec.size();
	printf("version: %s\n", formatHeader.version);
	printf("nmea_coord_count: %d\n", formatHeader.nmea_coord_count);
	printf("nmea_coord_vec_size: %d\n", nmea_coord_vec.size());
	printf("nmea_file_full_path: %s\n", formatHeader.nmea_file_full_path);

	FILE* convertBinFile = fopen("nmea_convert.bin", "wb");
	if(convertBinFile == NULL) {
		printf("NMEA Convert Binary File OPEN FAILED.\n");
	} else {

		size_t st_c = fwrite(&formatHeader, sizeof(formatHeader), 1, convertBinFile);
		if(st_c != 1) printf("fwrite error code: %d\ncount: %d\n", ferror(convertBinFile), st_c);

		size_t st_v = fwrite(&nmea_coord_vec[0], sizeof(NMEA_Coord)*nmea_coord_vec.size(), 1, convertBinFile);
		if(st_v != 1) printf("fwrite error code: %d\ncount: %d\n", ferror(convertBinFile), st_v);

		fclose(convertBinFile);
	}

	return 0;
}