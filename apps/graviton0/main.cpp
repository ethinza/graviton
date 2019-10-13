///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2019 Ethinza Inc, All rights reserved
//
// This file contains TRADE SECRETs of ETHINZA INC and is CONFIDENTIAL.
// Without written consent from ETHINZA INC, it is prohibited to disclose or
// reproduce its contents, or to manufacture, use or sell it in whole or part.
// Any reproduction of this file without written consent of ETHINZA INC is a 
// violation of the copyright laws and is subject to civil liability and 
// criminal prosecution.
//
///////////////////////////////////////////////////////////////////////////////

#include <ethinza.h>
#include <utility/osinterface.h>
#include <utility/stringbuilder.h>
#include <utility/stringutility.h>
#include <utility/fileutility.h>
#include <service/logmanager.h>

///////////////////////////////////////////////////////////////////////////////

// This implementation is based on following articles:
// https://www.digitalocean.com/community/tutorials/how-to-install-and-configure-ansible-on-centos-7
// https://gdeploy.readthedocs.io/en/latest/howitworks.html
// https://github.com/oVirt/ovirt-ansible
// http://www.chrisj.cloud/?q=node/8
// https://www.ovirt.org/documentation/install-guide/chap-System_Requirements.html

#define PRODUCT_NAME "graviton0"
#define PRODUCT_VERSION "1.3"

#define PRINT_VALID(flag) ((flag) ? '+' : '-')
#define SHARED_PATH "/usr/share/ethinza"
#define ANSIBLE_HOSTS "/etc/ansible/hosts"
#define ETHINZA_SSH_KEY "ethinza.ssh"


///////////////////////////////////////////////////////////////////////////////

// Initialization sequence
int InitSteps(void)
{
    LogM.AddTarget(LogTarget::File);
    LogM.AddTarget(LogTarget::Shell);

    CommonLog.Info("%s version %s built %s", PRODUCT_NAME, PRODUCT_VERSION, __TIMESTAMP__);

    return 0;
}

// Removing sequence
int RemoveSteps(void)
{
    CommonLog.Info("Exiting . . .");
    return 0;
}

bool CheckFolderSize(const char* folder, u64_t minSize)
{
    u64_t total;
    u64_t free;

    FileUtility::GetFolderSize(folder, total, free);
    bool validFolder = (total >= (minSize * GB));
    CommonLog.Info("[%c] %s folder size = %lldGB (%lld) (>= %lldGB)", PRINT_VALID(validFolder),
                folder, total / GB, total, minSize);

    return validFolder;
}

// Verify oVirt needed resources
bool VerifyHciRequirements(bool isMaster)
{
    CommonLog.Info("Run update and reboot if kernel is updated if you haven't done so already");
    CommonLog.Info("    e.g. sudo yum update -y\n");
    CommonLog.Info("\n[.] Starting tool setup for %s node . . .", isMaster ? "master" : "client");

    Osi.SetSystemDetails();

    CommonLog.Info("Checking system requirements . . .");
    bool validProcessor = (Osi.GetProcessorType() == ProcessorType::Intel64)
                            || (Osi.GetProcessorType() == ProcessorType::Amd64);
    CommonLog.Info("[%c] CPU type = %s (Intel/Amd 64-bit)", PRINT_VALID(validProcessor), Osi.GetCpuModel().c_str());

    bool validCores = Osi.GetTotalProcessors() >= 4;
    CommonLog.Info("[%c] Total CPU cores = %d (>= 4)", PRINT_VALID(validCores), Osi.GetTotalProcessors());
    
    bool validVirt = Osi.IsVirtualizationSupported();
    CommonLog.Info("[%c] Virtualization support (VMX/SVM & NX flags)", PRINT_VALID(validVirt));
    
    bool validOs = ((Osi.GetOsType() == OsType::Centos) || (Osi.GetOsType() == OsType::Rhel))
                    && (Osi.GetOsVersion() >= 7);
    CommonLog.Info("[%c] OS found = %s (Centos/RHEL 7 or later)", PRINT_VALID(validOs), Osi.GetOsName().c_str());
    
    bool validRam = Osi.GetRamSize() >= ((u64_t)4 * GB);
    CommonLog.Info("[%c] RAM size = %lldGB (%lld) (>= 4GB)", PRINT_VALID(validRam),
                Osi.GetRamSize() / GB, Osi.GetRamSize());
    
    bool validStorage = Osi.GetHddSize() >= ((u64_t)45 * GB);
    CommonLog.Info("[%c] HDD size = %lldGB (%lld) (>= 45GB)", PRINT_VALID(validStorage),
                Osi.GetHddSize() / GB, Osi.GetHddSize());

    validStorage = validStorage && CheckFolderSize("/", 6)
                                && CheckFolderSize("/home", 1)
                                && CheckFolderSize("/tmp", 1)
                                && CheckFolderSize("/var", 15)
                                && CheckFolderSize("/var/log", 8)
                                && CheckFolderSize("/var/log/audit", 2)
                                && CheckFolderSize("/var/tmp", 5)
                                && CheckFolderSize("/boot", 1)
                                // && CheckFolderSize("/swap", 1)
                                ;

    // TODO https://www.ovirt.org/documentation/install-guide/chap-System_Requirements.html
    
    bool ret = validCores && validOs && validProcessor && validRam && validStorage && validVirt;
    if (!ret) CommonLog.Warning("\n[.] ===> Not all requirements are met. Please, contact Ethinza.");
    return ret;
}

// Makes ethinza folder and downloads ethinza product applicable to the product build
void LoadEthinza(void)
{
    String output;
    String path = SHARED_PATH;

    if (FileUtility::FolderExists(path.c_str())) {
        CommonLog.Info("[+] %s exists", path.c_str());
    } else {
        CommonLog.Info("[.] Creating %s", path.c_str());
        Osi.Execute((String("sudo mkdir ") + path).c_str(), output);
    }

    CommonLog.Info("[.] Copying %s to %s", PRODUCT_NAME, path.c_str());
    Osi.Execute(StringUtility::Format("sudo cp %s %s", PRODUCT_NAME, path.c_str()), output);

    if (FileUtility::FileExists(StringUtility::Format("%s/%s", path.c_str(), PRODUCT_NAME).c_str())) {
        CommonLog.Info("[+] %s copied", PRODUCT_NAME);
    } else {
        CommonLog.Error("[-] Error copying %s", PRODUCT_NAME);
    }
}

void InstallAnsible(void)
{
    String output;

    if (Osi.IsExecutable("ansible")) {
        CommonLog.Info("[+] Ansible found");
    } else {
        CommonLog.Info("[.] Installing ansible");
        Osi.Execute("sudo yum install epel-release", output);
        Osi.Execute("sudo yum install ansible", output);
    }
}

void InstallGdeploy(void)
{
    String output;

    if (Osi.IsExecutable("gdeploy")) {
        CommonLog.Info("[+] gdeploy found");
    } else {
        CommonLog.Info("[.] Installing gdeploy");
        Osi.Execute("wget https://download.gluster.org/pub/gluster/gdeploy/LATEST/CentOS7/gdeploy-2.0.1-9.noarch.rpm", output);
        Osi.Execute("sudo yum install ./gdeploy-2.0.1-9.noarch.rpm", output);
    }
}

void InstallOvirtEngine(void)
{
    String output;
    String command;

    CommonLog.Info("[.] Installing oVirt repository");
    Osi.Execute("sudo yum install https://resources.ovirt.org/pub/yum-repo/ovirt-release43.rpm -y", output);
    if (Osi.GetOsType() == OsType::Rhel) {
        CommonLog.Info("[.] Installing additional repositories on RHEL");
        Osi.Execute("sudo subscription-manager repos --enable=\"rhel-7-server-rpms\"", output);
        Osi.Execute("sudo subscription-manager repos --enable=\"rhel-7-server-optional-rpms\"", output);
        Osi.Execute("sudo subscription-manager repos --enable=\"rhel-7-server-extras-rpms\"", output);
    }
    
    CommonLog.Info("[.] Installing cockpit, ovirt dashboard and graviton dashboard");
    Osi.Execute("sudo yum install cockpit cockpit-ovirt-dashboard -y", output);

    command = "cockpit-graviton-latest.bz2";

    Osi.Execute(StringUtility::Format("wget http://www.ethinza.com/repository/graviton/%s", command.c_str()), output);
    Osi.Execute(StringUtility::Format("sudo tar -xjvf %s -C /usr/share/cockpit", command.c_str()), output);
    Osi.Execute(StringUtility::Format("rm %s", command.c_str()), output);

    CommonLog.Info("[.] Enabling cockpit");
    Osi.Execute("sudo systemctl enable --now cockpit.socket", output);
    Osi.Execute("sudo firewall-cmd --add-service=cockpit", output);
    Osi.Execute("sudo firewall-cmd --add-service=cockpit --permanent", output);
    
    CommonLog.Info("[.] Installing ovirt engine setup roles");
    // installing ovirt-ansible-roles include all the galaxy packages for ovirt
    Osi.Execute("yum install ovirt-ansible-roles", output); // https://github.com/oVirt/ovirt-ansible
    //Osi.Execute("ansible-galaxy install ovirt.repositories", output);
    //Osi.Execute("ansible-galaxy install ovirt.engine-setup", output);
}

// Checks if sshd is active/running, and starts it if needed
void CheckAndStartSshd(void)
{
    String output;
    
    Osi.Execute("systemctl status sshd.service", output);
    if (!output.find("active")) {
        CommonLog.Info("[.] Starting sshd service");
        Osi.Execute("systemctl start sshd.service", output);
    } else {
        CommonLog.Info("[+] Found sshd service running");
    }
    
    CommonLog.Info("[.] Preparing for password less ssh communication");
    Osi.Execute(StringUtility::Format("ssh-keygen -t rsa -N \'\' -f %s", ETHINZA_SSH_KEY), output);
}

// Gets passowrdless ssh ready between master and other hosts in network
void PrepareSshCommunication(String user, String host)
{
    String output;
    
    CommonLog.Info("[.] Preparing passwordless ssh to/from %s for %s",
            host.c_str(), user.c_str());
    Osi.Execute(StringUtility::Format("ssh-copy-id %s@%s", user.c_str(), host.c_str()), output);
}

void PrintEndNotes(void)
{
    CommonLog.Info("\n===> Log in to Cockpit as root at https://[Host IP or FQDN]:9090 and click Virtualization → Hosted Engine");
    CommonLog.Info("Click Start under the Hosted Engine option");
    CommonLog.Info("Complete the setup wizard. As part of the setup, you will enter the Hosted Engine's name");
    CommonLog.Info("oVirt requires a central shared storage system for Virtual Machine disk images, ISO files, and snapshots");
    CommonLog.Info("As part of the setup wizard, you will need to provide the location of your storage");
    CommonLog.Info("Once the installation completes, oVirt's web UI management interface will start. Browse to https://[Hosted Engine's name]/ to begin using oVirt");

    CommonLog.Info("\nYou must install at least one additional Host for advanced features like migration and high-availability");
    CommonLog.Info("Use the oVirt Administration Portal to add them to the Engine. Navigate to Compute → Hosts → New and enter the Host details");

    CommonLog.Info("\n===> Contact us via http://www.ethinza.com for additional questions, comments or concerns\n");
}

// Installs all necessary tools
void InstallMaster(void)
{
    VerifyHciRequirements(true);
    LoadEthinza();
    InstallAnsible();
    InstallGdeploy();

    InstallOvirtEngine();

    CheckAndStartSshd();

    PrintEndNotes();
}

// Client node installation
void InstallClient(void)
{
    VerifyHciRequirements(false);
    LoadEthinza();
    InstallAnsible();
    InstallGdeploy();

    CheckAndStartSshd();
}

// Check all the required tools
void CheckTools(void)
{
    String output;
    
    Osi.Execute("which ansible", output);
    CommonLog.Info("Found %s", output.c_str());

    Osi.Execute("which gdeploy", output);
    CommonLog.Info("Found %s", output.c_str());
}

// Checks if local node has DNS assigned name
void CheckHostName(String& output)
{
    Osi.Execute("hostname", output);
}

// Adds nodes to host list
void AddNodes(const char* prefix, const char* domain, const char* ipAddresses)
{
    String hostname;

    CheckHostName(hostname);
    CommonLog.Info("Currently, hostname is set to '%s'", hostname.c_str());

    StringList ipList;
    StringList hostList;
    StringList fqdnList;

    StringBuilder sb;
    sb.AppendLine("[servers]");

    if (StringUtility::SplitIpv4(ipAddresses, ipList) > 0)
    {
        int idx = 1;

        CommonLog.Info("Total IP addresses supplied: %ld", ipList.size());

        for (StringList::iterator it = ipList.begin(); it != ipList.end(); ++it, ++idx)
        {
            String host = StringUtility::Format("%s%d", prefix, idx);
            String fqdn = StringUtility::Format("%s%d.%s", prefix, idx, domain);

            CommonLog.Info("IP='%s' Host='%s' FQDN='%s'", it->c_str(), host.c_str(), fqdn.c_str());
            hostList.push_back(host);
            fqdnList.push_back(fqdn);
            
            PrepareSshCommunication("root", fqdn);
            sb.AppendLine("%s ansible_ssh_host=%s", host.c_str(), it->c_str());
        }
        
        if (Osi.UpdateEtcHosts(ipList, hostList, fqdnList))
        {
            CommonLog.Info("[+] Host updates successful");
        }
        else
        {
            CommonLog.Error("[-] Host updates failed");
        }

        FileUtility::WriteToFile(ANSIBLE_HOSTS, sb.GetText(), false);
        CommonLog.Info("[+] " ANSIBLE_HOSTS " updated");
    }
    else
    {
        CommonLog.Error("[-] Error interpreting '%s'", ipAddresses);
    }
}

// Help on tool
void PrintHelp(const char* command)
{
    // follow http://www.chrisj.cloud/?q=node/8
    StringBuilder sb;

    sb.AppendLine("Available options are:");
    sb.AppendLine("");
    sb.AppendLine("    install <master/client> - installs all necessary tools for infrastructure");
    sb.AppendLine("");

    printf("%s", sb.GetText());
}

// main entry point
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        PrintHelp(nullptr);
        return -1;
    }

    if (InitSteps())
    {
        RemoveSteps();
        return -1;
    }

    // Process commands now ----------------------------------------------------
    String command(argv[1]);

    if (command == "install") {
        if (argc >= 3)
        {
            if (strcmp(argv[2], "master") == 0) InstallMaster();
            else if (strcmp(argv[2], "client") == 0) InstallClient();
            else {
                printf("Missing valid option : master or client\n\n");
                PrintHelp(nullptr);
            }
        } else {
            printf("Missing argument(s), see help\n\n");
            PrintHelp(nullptr);
        }
    } else if ((command == "help") || (command == "?")) {
        PrintHelp(nullptr);
    } else {
        printf("Unsupported command: %s\n", command.c_str());
        PrintHelp(nullptr);
    }

    // Ends processing commands ------------------------------------------------

    RemoveSteps();
    return 0;
}
