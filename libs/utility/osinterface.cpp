///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2018-2019 Ethinza Inc, All rights reserved
//
// This file contains TRADE SECRETs of ETHINZA INC and is CONFIDENTIAL.
// Without written consent from ETHINZA INC, it is prohibited to disclose or
// reproduce its contents, or to manufacture, use or sell it in whole or part.
// Any reproduction of this file without written consent of ETHINZA INC is a 
// violation of the copyright laws and is subject to civil liability and 
// criminal prosecution.
//
///////////////////////////////////////////////////////////////////////////////

#include "osinterface.h"
#include "stringutility.h"
#include "fileutility.h"

#include <ctime>
#include <iostream>
#include <fstream>
#include <pwd.h>
#include <unistd.h>
#include <mntent.h>
#include <thread>
#include <sys/sysinfo.h>

#define MAX_READ 512
#define HOSTS_FILE "/etc/hosts"
#define CPU_INFO_FILE "/proc/cpuinfo"
#define MEMORY_INFO_FILE "/proc/meminfo"
#define CPU_STATS_FILE "/proc/stats"
#define OS_RELEASE_FILE "/etc/os-release"

// default ctor
OsInterface::OsInterface()
{
    // find endian
    union
    {
        u32_t i;
        char c[4];
    } e = { 0x01020304 };

    m_bigEndian = (e.c[0] == 0x01);

    // find user
    m_uid = FindUser();
}

uid_t OsInterface::FindUser(void)
{
    struct passwd* p;
    uid_t uid = getuid();

    if ((p = getpwuid(uid)) == NULL)
        perror("getpwuid() error");
    else {
        /*
        puts("getpwuid() returned the following info for your userid:");
        printf("  pw_name  : %s\n",       p->pw_name);
        printf("  pw_uid   : %d\n", (int) p->pw_uid);
        printf("  pw_gid   : %d\n", (int) p->pw_gid);
        printf("  pw_dir   : %s\n",       p->pw_dir);
        printf("  pw_shell : %s\n",       p->pw_shell);
         */
    }

    return uid;
}

void OsInterface::SetSystemDetails(void)
{
    // find processor information
    m_totalProcessors = std::thread::hardware_concurrency(); //get_nprocs_conf(); //sysconf(_SC_NPROCESSORS_ONLN);
    m_ticksPerSec = sysconf(_SC_CLK_TCK);

    char str[1024];
    FILE* fp;
    bool isIntel = false;
    bool isAmd = false;
    bool isIbm = false;

    if ((fp = fopen(CPU_INFO_FILE, "r"))) {
        memset(&str, 0, sizeof(str));

        while (fgets(str, sizeof str, fp)) {
            if (std::memcmp(str, "model name", 10) == 0)
            {
                m_cpuModel = std::strstr(str, ":") + 2;
                if (m_cpuModel.back() == '\n') m_cpuModel.erase(m_cpuModel.length() - 1, 1);

                if (std::strstr(str, "Intel")) isIntel = true;
                else if (std::strstr(str, "AMD")) isAmd = true;
                else if (std::strstr(str, "IBM")) isIbm = true;
            }
            else if (std::memcmp(str, "flags", 5) == 0)
            {
                if (std::strstr(str, " lm ")) {
                    if (isIntel) m_processorType = ProcessorType::Intel64;
                    else if (isAmd) m_processorType = ProcessorType::Amd64;
                    else if (isIbm) m_processorType = ProcessorType::IbmPower8;
                }

                m_isVirtualizationSupported = (std::strstr(str, "nx") != NULL)
                    && ((std::strstr(str, "svm") != NULL) || (std::strstr(str, "vmx") != NULL));

                break;
            }
        }
    
        fclose(fp);
    }

    // find memory info
    struct sysinfo sysInfo;
    sysinfo(&sysInfo);
    m_ramSize = sysInfo.totalram * sysInfo.mem_unit;

    // find disk info
    FileUtility::GetFolderSize("/", m_hddSize, m_hddFreeSize);

    // find os
    String output;
    Execute("hostnamectl", output);
    if (std::strstr(output.c_str(), "CentOS") != NULL) {
        m_osType = OsType::Centos;
    } else if (std::strstr(output.c_str(), "Red Hat") != NULL) {
        m_osType = OsType::Rhel;
    }

    if ((fp = fopen(OS_RELEASE_FILE, "r"))) {
        memset(&str, 0, sizeof(str));

        while (fgets(str, sizeof str, fp)) {
            if (std::strstr(str, "VERSION_ID") != NULL)
            {
                m_osVersion = (int)strtoul((char*)&str[strlen("VERSION_ID=") + 1], NULL, 10);
            }
            else if (std::strstr(str, "PRETTY_NAME") != NULL)
            {
                m_osName = (char*)&str[strlen("PRETTY_NAME=") + 1];
                if ((m_osName.back() == '\n') || (m_osName.back() == '\"')) m_osName.erase(m_osName.length() - 1, 1);
            }
        }

        fclose(fp);
    }

    /*
    https://www.cyberciti.biz/faq/find-linux-distribution-name-version-number/
    https://www.ovirt.org/documentation/install-guide/chap-System_Requirements.html

[chirag@ethinza ~]$ hostnamectl
   Static hostname: ethinza.scb
         Icon name: computer-desktop
           Chassis: desktop
        Machine ID: 0f77415e8460461e8c2d153aed3498d4
           Boot ID: ff483a1411704bb9b22942074d107f99
  Operating System: CentOS Linux 7 (Core)
       CPE OS Name: cpe:/o:centos:centos:7
            Kernel: Linux 3.10.0-957.10.1.el7.x86_64
      Architecture: x86-64
       * ---------------------------------------
[chirag@ethinza ~]$ cat /etc/ *release
CentOS Linux release 7.6.1810 (Core)
NAME="CentOS Linux"
VERSION="7 (Core)"
ID="centos"
ID_LIKE="rhel fedora"
VERSION_ID="7"
PRETTY_NAME="CentOS Linux 7 (Core)"
ANSI_COLOR="0;31"
CPE_NAME="cpe:/o:centos:centos:7"
HOME_URL="https://www.centos.org/"
BUG_REPORT_URL="https://bugs.centos.org/"

CENTOS_MANTISBT_PROJECT="CentOS-7"
CENTOS_MANTISBT_PROJECT_VERSION="7"
REDHAT_SUPPORT_PRODUCT="centos"
REDHAT_SUPPORT_PRODUCT_VERSION="7"

CentOS Linux release 7.6.1810 (Core)
CentOS Linux release 7.6.1810 (Core)

 * 
 * -----------------------------------
char str[256];
int procCount = 0;
FILE *fp;

if( (fp = fopen("/proc/cpuinfo", "r")) )
{
  while(fgets(str, sizeof str, fp))
  if( !memcmp(str, "processor", 9) ) procCount++;
}

if ( !procCount ) 
{ 
printf("Unable to get proc count. Defaulting to 2");
procCount=2;
}

printf("Proc Count:%d\n", procCount);
return 0;
}
 * ---------------------------------------
[chirag@ethinza ~]$ cat /proc/meminfo
MemTotal:        7892928 kB
MemFree:         6482540 kB
MemAvailable:    6766976 kB

 * ---------------------------------------
ifstream cpu_freq("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq");
strStream << cpu_freq.rdbuf();
std::string  cpufrequency = strStream.str();
//---
strStream.str("");
ifstream cpu_temp("/sys/class/thermal/thermal_zone0/temp");
strStream << cpu_temp.rdbuf();
strConvert<< fixed << setprecision(2) << std::stof(strStream.str());
std::string cputemp = strConvert.str();
 * ----------------------------------------
[chirag@ethinza ~]$ lscpu
Architecture:          x86_64
CPU op-mode(s):        32-bit, 64-bit
Byte Order:            Little Endian
CPU(s):                4
On-line CPU(s) list:   0-3
Thread(s) per core:    2
Core(s) per socket:    2
Socket(s):             1
NUMA node(s):          1
Vendor ID:             GenuineIntel
CPU family:            6
Model:                 158
Model name:            Intel(R) Core(TM) i3-7100 CPU @ 3.90GHz
Stepping:              9
CPU MHz:               3900.000
CPU max MHz:           3900.0000
CPU min MHz:           800.0000
BogoMIPS:              7824.00
Virtualization:        VT-x
L1d cache:             32K
L1i cache:             32K
L2 cache:              256K
L3 cache:              3072K
NUMA node0 CPU(s):     0-3
Flags:                 fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc aperfmperf eagerfpu pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch epb intel_pt ssbd ibrs ibpb stibp tpr_shadow vnmi flexpriority ept vpid fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt xsaveopt xsavec xgetbv1 dtherm arat pln pts hwp hwp_notify hwp_act_window hwp_epp spec_ctrl intel_stibp flush_l1d

 */
}

/**
 * @brief Periodically checks if user has pressed keyboard button
 * @param waitMicros
 * @return true if there is keyboard event
 */
bool OsInterface::IsStdinEvent(u32_t waitMicros)
{
    fd_set rfds;
    struct timeval tv;
    int retval;

    // Watch stdin (fd 0) to see when it has input.
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    // Wait time
    tv.tv_sec = waitMicros / 1000000;
    tv.tv_usec = waitMicros % 1000000;

    retval = select(1, &rfds, NULL, NULL, &tv);
    // Don't rely on the value of tv now!

    return retval > 0;
}

/**
 * @brief Returns total ticks since the app started
 * @return ticks count
 */
u64_t OsInterface::CurrentTicks(void)
{
    struct timespec ts;

    clock_gettime( CLOCK_REALTIME, &ts );
#if 1
    static long int s_orgSec = 0;
    if ( s_orgSec == 0 ) s_orgSec = ts.tv_sec;

    return ( ( u64_t )( ts.tv_sec - s_orgSec ) * 1000000000LL ) + ts.tv_nsec;
#else
    static long int s_orgNsec = 0;
    if ( s_orgNsec == 0 )
        s_orgNsec = ts.tv_nsec;
    return ( u64_t )( ts.tv_nsec - s_orgNsec );
#endif
}

/**
 * @brief Gets current time from local timezone
 * @return Ethinza time object
 */
EthTime OsInterface::CurrentTime(void)
{
    time_t rawTime;
    time(&rawTime);

    struct tm tmtmp;
    return EthTime(localtime_r(&rawTime, &tmtmp));
}

/**
 * @brief Gets current time from UTC timezone
 * @return Ethinza time object
 */
EthTime OsInterface::CurrentUtcTime(void)
{
    time_t rawTime;
    time(&rawTime);

    struct tm tmtmp;
    return EthTime(gmtime_r(&rawTime, &tmtmp));
}

/**
 * @brief Returns list of all available drives
 * @param driveList returned list
 * @return total drives found
 */
int OsInterface::GetDriveList(StringList& driveList)
{
    driveList.clear();

    struct mntent* m;
    FILE* f;
    int ret = 0;

    f = setmntent(_PATH_MOUNTED, "r");
    while ((m = getmntent(f)))
    {
        //printf("Drive: %s      Name: %s\n", m->mnt_dir, m->mnt_fsname);

        driveList.push_back(String(m->mnt_dir));
        ++ret;
    }
    endmntent(f);

    return ret;
}

/**
 * @brief Executes specified command and returns output capture
 * @param command
 * @param output
 * @return 0 on success
 */
int OsInterface::Execute(const String& command, String& output)
{
    FILE* fp;
    int status;
    char path[MAX_READ];
    int retErr = -1;
    
    output.clear();

    fp = popen(command.c_str(), "r");
    if (fp != NULL)
    {
        while (fgets(path, MAX_READ, fp) != NULL)
            output += path;

        // Remove last NEWLINE character if it is there
        if (output.back() == '\n') output.erase(output.length() - 1, 1);

        status = pclose(fp);
        if (status == -1)
        {
            retErr = -1;
        }
        else
        {
            /* Use macros described under wait() to inspect `status' in order
            to determine success/failure of command executed by popen() */
            retErr = 0;
        }
    }
    
    return retErr;
}

/**
 * @brief Finds if executable exist
 * @param executable
 * @return true if found
 */
bool OsInterface::IsExecutable(const String& executable)
{
    String output;
    Osi.Execute(String("command -v ") + executable, output);
    
    return output.find(executable) != String::npos;
}

/**
 * @brief Updates /etc/hosts file with list of hosts provided
 * @param ipList
 * @param hostList
 * @param fqdnList
 * @return true on success
 */
bool OsInterface::UpdateEtcHosts(StringList& ipList, StringList& hostList, StringList& fqdnList)
{
    String line;
    StringList fileContent;
    bool success = false;
    std::ifstream inf(HOSTS_FILE);

    // read content of file first
    if (inf.is_open())
    {
        while (getline(inf, line))
        {
            bool useLine = true;

            // skip line with matching ip in the list
            for (String ip : ipList)
            {
                if (line.find(ip) != String::npos)
                {
                    useLine = false;
                    break;
                }
            }

            // skip line with matching host in the list
            if (useLine)
            {
                for (String host : hostList)
                {
                    if (line.find(host) != String::npos)
                    {
                        useLine = false;
                        break;
                    }
                }
            }

            if (useLine) fileContent.push_back(line);
        }

        inf.close();

        // recreate file content
        std::ofstream outf(HOSTS_FILE);

        if (outf.is_open())
        {
            // first write previous unskipped content
            for (String newLine : fileContent)
            {
                outf.write(newLine.c_str(), newLine.length());
                outf.write("\n", 1);
            }

            // write ip mappings now
            StringList::iterator itHost = hostList.begin();
            StringList::iterator itFqdn = fqdnList.begin();

            for (StringList::iterator itIp = ipList.begin(); itIp != ipList.end(); ++itIp, ++itHost, ++itFqdn)
            {
                String ipLine = StringUtility::Format("%s %s %s\n",
                                                        (*itIp).c_str(),
                                                        (*itHost).c_str(),
                                                        (*itFqdn).c_str());
                outf.write(ipLine.c_str(), ipLine.length());
            }

            outf.close();
            success = true;
        }
    }

    return success;
}
