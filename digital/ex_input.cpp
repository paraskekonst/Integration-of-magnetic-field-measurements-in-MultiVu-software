#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <algorithm>
using namespace std;

typedef int (__stdcall *f_funci)();
typedef int (__stdcall *func_ptr)(unsigned int,char*,char*,int);
typedef void(__stdcall *func_close)(unsigned int);
unsigned int id;
HINSTANCE hGetProcIDDLL = LoadLibrary("F:\\Program Files (x86)\\FW Bell\\PC5180\\usb5100.dll");
f_funci open = (f_funci)GetProcAddress(hGetProcIDDLL, "openUSB5100");
func_ptr data =(func_ptr)GetProcAddress(hGetProcIDDLL,"scpiCommand");
func_close close = (func_close)GetProcAddress(hGetProcIDDLL,"closeUSB5100");

 void openusb(){
    id = open();
    if (!id) {
        cout << "could not locate the function" << endl;
  }
}

void closeusb(){
    close(id);
}

char* readdata(){
    char* ac;
    data(id,(char*)":MEASURE:FLUX?",ac,80);
    return ac;
}

string returnNumber(char* a){
    string b(a);

    int counta=0;
    int countb=0;
    b.erase(remove(b.begin(),b.end(),'G'),b.end());
    b.erase(remove(b.begin(),b.end(),' '),b.end());
    return b;
}





string lastf()
{
    WIN32_FIND_DATAW ffd;
    wchar_t const* directory = L"C:\\QdVersaLab\\Data\\";
    wchar_t currentFile[MAX_PATH], lastModifiedFilename[MAX_PATH];
    FILETIME currentModifiedTime, lastModified;
    HANDLE hFile;
    bool first_file = true;

    HANDLE hFind = FindFirstFileW( L"C:\\QdVersaLab\\Data\\*", &ffd );

    if ( INVALID_HANDLE_VALUE == hFind )
    {
       cout<<"cant find path";
    }

    do
    {
        if ( !( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
        {
            wcscpy( currentFile, directory );
            wcscat( currentFile, ffd.cFileName );
            // open file to read it's last modified time
            hFile = CreateFileW( currentFile, GENERIC_READ, FILE_SHARE_READ, NULL,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
            if( INVALID_HANDLE_VALUE != hFile )
            {
                // get it's last write time
                if( GetFileTime( hFile, NULL, NULL, &currentModifiedTime ) != 0 )
                {
                    if( first_file )
                    {
                        lastModified = currentModifiedTime;
                        wcscpy( lastModifiedFilename, ffd.cFileName );
                        first_file = false;
                    }
                    else
                    {
                        // First file time is earlier than second file time.
                        if( CompareFileTime( &lastModified, &currentModifiedTime ) == -1 )
                        {
                            lastModified = currentModifiedTime;
                            wcscpy( lastModifiedFilename, ffd.cFileName );
                        }
                    }
                }
                CloseHandle( hFile );
            }
        }
    }
    while ( FindNextFileW( hFind, &ffd ) != 0 );

    FindClose( hFind );
    wstring ws(lastModifiedFilename);
    wstring s(directory);
    // your new String
    string str1(s.begin(), s.end());
    string str2(ws.begin(), ws.end());
    string filename=str1+str2;
    return filename;
}
void copydata(const char* dest ,const char* source){
    std::ifstream  src(source );
    std::ofstream  dst(dest);
    dst << src.rdbuf();
    dst.close();
    src.close();
}
int main(){

  //  openusb();
    char *aaa;
    string bbb;
	streampos size,begin,end,logsize,prev,endlog;
	string str= lastf();

	//string line;
	const char * c = str.c_str();
	const char* tempfile="C:\\Documents and Settings\\Administrator\\My Documents\\temp.dat";
	copydata(tempfile,c);
    ifstream file(c);
    ifstream log("C:\\Documents and Settings\\Administrator\\My Documents\\eomlog.txt");
    ofstream temp(tempfile,ios::app);
    file.seekg( 0, ios::end );
    size=file.tellg();
    prev=size;
    log.seekg( 0, ios::end );
    logsize=log.tellg();
    begin=logsize;
    file.close();
    log.close();
    int pos;
    while(TRUE){
    	Sleep(1);
        fstream file(c);
        file.seekg( 0, ios::end );
        end=file.tellg();
    	ifstream log("C:\\Documents and Settings\\Administrator\\My Documents\\eomlog.txt");
    	log.seekg(  0, ios::end );
    	endlog=log.tellg();
    	if (prev<end){
            string line;
            aaa=readdata();
            bbb=returnNumber(aaa);
    		file.seekg(prev );
	    	getline (file,line);
	    	pos=line.find(",,");
            line.insert(pos+1,bbb);
            file.seekg(prev );
            file<<line<<endl;
	    	temp<<line<<endl;
		    prev=file.tellg();
		    line.clear();

		}else if (endlog>begin){
			file.close();
			log.close();
			temp.close();
			break;
		}
		file.close();
		log.close();

    }
    closeusb();
   //copydata(c,tempfile);

}
