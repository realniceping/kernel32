using System.Runtime.InteropServices;
using System.IO;
using System;
using System.Diagnostics;

public class ProcessMemoryScanner{
    const int PROCESS_QUERY_INFORMATION = 0x0400;
    const int MEM_COMMIT = 0x00001000;
    const int PAGE_READWRITE = 0x04;
    const int PROCESS_WM_READ = 0x0010;

    public struct MEMORY_BASIC_INFORMATION
    {
        public int BaseAddress;
        public int AllocationBase;
        public int AllocationProtect;
        public int RegionSize;
        public int State;
        public int Protect;
        public int lType;
    }

    public struct SYSTEM_INFO
    {
        public ushort processorArchitecture;
        ushort reserved;
        public uint pageSize;
        public IntPtr minimumApplicationAddress;
        public IntPtr maximumApplicationAddress;
        public IntPtr activeProcessorMask;
        public uint numberOfProcessors;
        public uint processorType;
        public uint allocationGranularity;
        public ushort processorLevel;
        public ushort processorRevision;
    }        


    [DllImport("kernel32.dll")]
    static extern void GetSystemInfo(out SYSTEM_INFO lpSystemInfo);

    [DllImport("kernel32.dll")]
    public static extern IntPtr OpenProcess
    (int dwDesiredAccess, bool bInheritHandle, int dwProcessId);

    [DllImport("kernel32.dll")]
    public static extern bool ReadProcessMemory
    (int hProcess, int lpBaseAddress, byte[] lpBuffer, int dwSize, ref int lpNumberOfBytesRead);

    [DllImport("kernel32.dll", SetLastError=true)]
    static extern int VirtualQueryEx(IntPtr hProcess, IntPtr lpAddress, out MEMORY_BASIC_INFORMATION lpBuffer, uint dwLength);

    [DllImport("kernel32.dll")]
    static extern int GetLastError();

    [DllImport("kernel32.dll")]
    static extern void HRESULT_FROM_WIN32(int x);

    public byte[ , ] getProcessMemoryMap(Process p, int width){
        

            SYSTEM_INFO sys_info = new SYSTEM_INFO();
        GetSystemInfo(out sys_info);

        IntPtr processPrt = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_WM_READ | 0x0010,  false,  p.Id);
        
            MEMORY_BASIC_INFORMATION readed_memory_information = new MEMORY_BASIC_INFORMATION();
            long min_app_ad = (long)(sys_info.minimumApplicationAddress);
        int queryResult = VirtualQueryEx(processPrt, sys_info.minimumApplicationAddress, out readed_memory_information, 28);
        if(queryResult == 0){
            int a = GetLastError();
            Console.WriteLine($"Error code : {a}, see - https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes");
            
            throw new Exception("Can't read query");
        }
        
        
            byte[] buffer = new byte[readed_memory_information.RegionSize];
            int bytesRead = 0;
        ReadProcessMemory((int)processPrt, readed_memory_information.BaseAddress, buffer, readed_memory_information.RegionSize, ref bytesRead);

        var buffer_to_return = new byte[readed_memory_information.RegionSize/width, width];
        for(int i = 0; i < readed_memory_information.RegionSize/width; i++){
            for(int j = 0; j < width; j++){
                buffer_to_return[i, j] = buffer[(i * width) + j];
            } 
        }
        return buffer_to_return;
    }   

}


public class Program {
    
    
    public static void Main(){

        var memory_reader = new ProcessMemoryScanner();

        var notepad = Process.GetProcessesByName("chrome")[0];
        var notepad_Buffer = memory_reader.getProcessMemoryMap(notepad, 8);
        for(int i = 0; i < notepad_Buffer.Length/8; i++)
        {
            for(int j = 0; j < 8; j++)
            {
                Console.Write((char)notepad_Buffer[i, j]);
            }
            Console.WriteLine();
        }

    }

}