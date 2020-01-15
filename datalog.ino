#include <SPI.h>
#include <SdFat.h>
//#include <Streaming.h>

#define PIN_SD_CS	SS

#define LOGFILENAME	"datalog"
#define LOGFILEEXT 	".csv"

SdFat Sd; // File system object.
SdFile Logfile; // Log file.

bool SdOK=false, FileOK=false; //some flags

/////////////////////////datalog state machine//////////////////////////////////
void datalog_wait()
{
	if(datalog.elapsed(P_periode_enregistrement * 1000UL) )
		datalog.next(datalog_write);
}


void datalog_start()
{
	SdOK=Sd.begin(PIN_SD_CS, SPI_HALF_SPEED);
	if(!SdOK) //something gone wrong so retry later
	{
		datalog.next(datalog_start);
		return;
	}
 	
 	datalog.next(datalog_write);				
}



void datalog_write()
{
 	String logfilename;
 	
// 	logfilename+=LOGFILENAME;
// 	logfilename+=DateTime.year;
// 	logfilename+='-';
// 	logfilename+=DateTime.month;
// 	logfilename+=LOGFILEEXT;
// 	FileOK=Logfile.open(logfilename.c_str(), O_RDWR | O_CREAT | O_AT_END);
// 	if(!FileOK)
// 	{
// 		SdOK=false;
// 		datalog.next(datalog_wait_card);
// 		return;
// 	}
// 	//Add date and time starting the line
// 	Logfile<<RTC.getS(DS1307_STR_DATE,0)<<F(" ")<<RTC.getS(DS1307_STR_TIME,0)<<F(";");
// 	//add the temperatures
// 	for(byte i=0;i<SENSOR_NBR;i++) 
// 	{
// 		TForce[i]?Logfile<<F("F;"):Logfile<<F(" ;");
// 		Logfile<<TAge[i]<<F(";");
// 		Logfile<<T[i]<<F(";");
// 	}
// 	//add the heating relevant data : Setpoint temperature, water calculated temperature, valve position
// 	Logfile<<TinSet<<F(";")<<Hon<<F(";")<<Wsetpoint<<F(";")<<MVWantedPos<<F(";")<<(int)MVActualPos<<F(";");
// 	//and the relays status
// 	Logfile<<(byte)Pwm0<<F(";");	
// 	(RF&BIT_R0)?Logfile<<F("F;"):Logfile<<F(" ;");
// 	(R&BIT_R0)?Logfile<<F("1;"):Logfile<<F("0;");
// 	(RF&BIT_R1)?Logfile<<F("F;"):Logfile<<F(" ;");
// 	(R&BIT_R1)?Logfile<<F("1;"):Logfile<<F("0;");
// 	(RF&BIT_R2)?Logfile<<F("F;"):Logfile<<F(" ;");
// 	(R&BIT_R2)?Logfile<<F("1;"):Logfile<<F("0;");	
// 	(RF&BIT_R3)?Logfile<<F("F;"):Logfile<<F(" ;");
// 	(R&BIT_R3)?Logfile<<F("1;"):Logfile<<F("0;");
// 	(RF&BIT_R4)?Logfile<<F("F;"):Logfile<<F(" ;");
// 	(R&BIT_R4)?Logfile<<F("1;"):Logfile<<F("0;");
// 
// 	Logfile<<_endl;
// 	Logfile.close();
	
	datalog.next(datalog_wait);
}      
