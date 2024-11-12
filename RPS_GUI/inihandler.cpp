#include "inihandler.h"

IniHandler::IniHandler() {}

int ParseIniResponce(char * data)
{
    CSimpleIniA ini;
    ini.SetUnicode();

    SI_Error rc = ini.LoadFile("example.ini");
}
