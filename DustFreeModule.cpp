#define MODULE_VERSION_MAJOR     1
#define MODULE_VERSION_MINOR     0
#define MODULE_VERSION_REVISION  0
#define MODULE_VERSION_BUILD     0
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2021
#define MODULE_RELEASE_MONTH     4
#define MODULE_RELEASE_DAY       3

#include "DustFreeModule.h"
#include "DustFreeProcess.h"
#include "DustFreeInterface.h"

namespace pcl
{

DustFreeModule::DustFreeModule()
{
}

const char* DustFreeModule::Version() const
{
    return PCL_MODULE_VERSION(MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE);
}

IsoString DustFreeModule::Name() const
{
    return "DustFree";
}

String DustFreeModule::Description() const
{
    return "PixInsight DustFree Module";
}

String DustFreeModule::Company() const
{
    return "N/A";
}

String DustFreeModule::Author() const
{
    return "Johnny Qiu";
}

String DustFreeModule::Copyright() const
{
    return "Copyright (c) 2021 Johnny Qiu";
}

String DustFreeModule::TradeMarks() const
{
    return "JQ";
}

String DustFreeModule::OriginalFileName() const
{
#ifdef __PCL_LINUX
    return "dustfree-pxm.so";
#endif
#ifdef __PCL_FREEBSD
    return "dustfree-pxm.so";
#endif
#ifdef __PCL_MACOSX
    return "dustfree-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
    return "dustfree-pxm.dll";
#endif
}

void DustFreeModule::GetReleaseDate(int& year, int& month, int& day) const
{
    year = MODULE_RELEASE_YEAR;
    month = MODULE_RELEASE_MONTH;
    day = MODULE_RELEASE_DAY;
}

}   // namespace pcl

PCL_MODULE_EXPORT int InstallPixInsightModule(int mode)
{
    new pcl::DustFreeModule;

    if (mode == pcl::InstallMode::FullInstall) {
        new pcl::DustFreeProcess;
        new pcl::DustFreeInterface;
    }

    return 0;
}
