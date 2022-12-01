#include <iostream>
#include <windows.h>
#include <minwindef.h>
#include <Psapi.h>
#include <fstream>

BOOL EnumProcesses(
  DWORD   *lpidProcess,
  DWORD   cb,
  LPDWORD lpcbNeeded
);

HANDLE OpenProcess(
  DWORD dwDesiredAccess,
  BOOL  bInheritHandle,
  DWORD dwProcessId
);

SIZE_T VirtualQueryEx(
    HANDLE                    hProcess,
    LPCVOID                   lpAddress,
    PMEMORY_BASIC_INFORMATION lpBuffer,
    SIZE_T                    dwLength
);

BOOL ReadProcessMemory(
  HANDLE  hProcess,
  LPCVOID lpBaseAddress,
  LPVOID  lpBuffer,
  SIZE_T  nSize,
  SIZE_T  *lpNumberOfBytesRead
);

#define PRDEBUG

#ifdef PRDEBUG
    #define LOG(x) std::cout << x;
    #define NEWLINE std::cout << std::endl;

#endif


int main(){
    std::ofstream logger;
    logger.open("log.txt");
    #ifndef PRDEBUG
        #define LOG(x) logger << x;
        #define NEWLINE logger<< "\n";
    #endif
    DWORD returnlpidProcess[1024];
    DWORD incb = 1024;
    DWORD lpcbout;

    BOOL status = EnumProcesses(returnlpidProcess, incb, &lpcbout);

    

    DWORD cProcesses = lpcbout / sizeof(DWORD);
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);

    logger << "processes readed " << cProcesses << std::endl;

    logger << "lp minimum application address : ";
    logger << sys_info.lpMinimumApplicationAddress;
    logger << std::endl; 
    logger << "page size: ";
    logger << sys_info.dwPageSize;
    logger << std::endl;
    
    
    byte * buffer;
    SIZE_T* readed_byte = new SIZE_T(0);

    for(int i = 0; i < cProcesses; i++){
        std::cout << returnlpidProcess[i] << std::endl;
    }

    int id_To_scan;
    std::cin >> id_To_scan; 
    //какой-то дескриптор надо получить сказали
    HANDLE descript_VM_READ = OpenProcess(PROCESS_VM_READ, false, id_To_scan);
    HANDLE descript_QUERY_INFORMATION =  OpenProcess(PROCESS_QUERY_INFORMATION , false, id_To_scan);
    //типо через это можно выдернуть информацию о памяти процесса
        MEMORY_BASIC_INFORMATION basicMemInfo;
    SIZE_T size = VirtualQueryEx(descript_QUERY_INFORMATION, sys_info.lpMinimumApplicationAddress, &basicMemInfo, sizeof(MEMORY_BASIC_INFORMATION));
    int pages = basicMemInfo.RegionSize / sys_info.dwPageSize;
    PVOID address = basicMemInfo.BaseAddress;

    //summary 
    //in address variable we have beggingnig process address 
    //in pages variable we have number of page that we need to read
    //each page (in my case) == 4096 byte
    //last problem - how to increment funcking PVOID 

    for(int o = 0; o < pages; o++){
        
        int buffer_size = sys_info.dwPageSize;
        buffer = new byte[buffer_size];
        PVOID read_address = PVOID((PINT32)address + (o * sys_info.dwPageSize));
        // address = (PVOID)((int*)address + ((int*)(o * sys_info.dwPageSize)));

        BOOL state = ReadProcessMemory(descript_VM_READ, read_address, buffer, buffer_size, readed_byte);


        // std::cout << basicMemInfo.AllocationBase << std::endl;
        logger << "alocation base : " ;
        logger << basicMemInfo.AllocationBase;
        logger << std::endl;
        LOG("allocation base: ")
        LOG(basicMemInfo.AllocationBase);
        NEWLINE;
        // std::cout << basicMemInfo.RegionSize << std::endl;
        logger << "Region size: ";
        logger << basicMemInfo.RegionSize;
        LOG("region size: ")
        LOG(basicMemInfo.RegionSize);
        logger << std::endl;
        NEWLINE;
    // std::cout << basicMemInfo.PartitionId << std::endl;
    // LOG(basicMemInfo.PartitionId);
    // NEWLINE;
    
        for(int i = 0; i < buffer_size; i++){
                
            //std::cout << (int*)address + i << " : " << buffer[i] << std::endl;
            LOG((char*)address + i);
            LOG(" : ");
            LOG((char)(buffer[i]));
            NEWLINE;
        }




    }
    
    
    logger.close();
    
    return 0;
}